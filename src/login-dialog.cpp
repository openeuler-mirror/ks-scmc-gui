/**
 * @file          /ks-scmc-gui/src/login-dialog.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "login-dialog.h"
#include <cryptopp/base64.h>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <QApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QPainter>
#include <QThreadPool>
#include <QTimer>

#include "common/message-dialog.h"
#include "global-application.h"
#include "main-window.h"
#include "pages/user/server-config-dialog.h"
#include "ui_login-dialog.h"
#include "user-configuration.h"

#define TIMEOUT 300

using namespace CryptoPP;
using namespace grpc;

LoginDialog::LoginDialog(QWidget *parent) : KiranTitlebarWindow(parent),
                                            ui(new Ui::LoginDialog),
                                            m_mainWindow(nullptr),
                                            m_serverCfgDlg(nullptr),
                                            m_activateDialog(nullptr),
                                            m_licenseProxy(nullptr),
                                            m_isLogin(false),
                                            m_isSessionExpired(false)
{
    ui->setupUi(getWindowContentWidget());

    m_objID = InfoWorker::generateId(this);

    m_serverCfgDlg = new ServerConfigDialog(this);
    m_serverCfgDlg->hide();

    initUI();
    initActivation();

    auto application = static_cast<GlobalApplication *>(QCoreApplication::instance());
    connect(application, &GlobalApplication::sessionExpired, this, &LoginDialog::sessionExpire);

    connect(&InfoWorker::getInstance(), &InfoWorker::loginFinished, this, &LoginDialog::getLoginResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::logoutFinished, this, &LoginDialog::getLogoutResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::sessionExpire, this, &LoginDialog::sessionExpire);
}

LoginDialog::~LoginDialog()
{
    delete ui;
    if (m_mainWindow)
    {
        delete m_mainWindow;
        m_mainWindow = nullptr;
    }
    if (m_serverCfgDlg)
    {
        delete m_serverCfgDlg;
        m_serverCfgDlg = nullptr;
    }
    if (m_activateDialog)
    {
        delete m_activateDialog;
        m_activateDialog = nullptr;
    }
}

void LoginDialog::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    KiranTitlebarWindow::paintEvent(event);
}

void LoginDialog::keyPressEvent(QKeyEvent *event)
{
    if (Qt::Key_Return == event->key() || Qt::Key_Enter == event->key())
    {
        if (ui->lineEdit_username->hasFocus())
            ui->lineEdit_passwd->setFocus();
        else if (ui->lineEdit_passwd->hasFocus())
            ui->btn_login->click();
    }
}

bool LoginDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_mainWindow && event->type() == QEvent::Close)
    {
        if (QThreadPool::globalInstance()->activeThreadCount())
        {
            MessageDialog::message(tr("Quit Application"),
                                   tr("There are tasks that have not finished!"),
                                   tr("Please wait for the tasks to end before closing!"),
                                   ":/images/warning.svg",
                                   MessageDialog::StandardButton::Ok);
            event->ignore();
            // qApp->quit();
            // event->accept();
            return true;
        }
        event->accept();
    }
    return false;
}

void LoginDialog::initUI()
{
    m_about = new AboutPage(this);
    setResizeable(false);
    setTitle(tr("KylinSec Container System V1(Security Version)"));
    setIcon(QIcon(":/images/logo.png"));
    setButtonHints(TitlebarMinimizeButtonHint | TitlebarCloseButtonHint);
    ui->btn_login->setCursor(QCursor(Qt::PointingHandCursor));
    ui->lab_tips->setStyleSheet("color:#d30000;");
    ui->lab_tips->setAlignment(Qt::AlignHCenter);
    ui->lab_tips->hide();

    //创建标题栏中菜单按钮
    setTitlebarCustomLayoutAlignHCenter(false);
    QHBoxLayout *titleBarLayout = getTitlebarCustomLayout();
    titleBarLayout->setSpacing(10);
    titleBarLayout->setContentsMargins(0, 0, 10, 0);
    QPushButton *btnMenu = new QPushButton(this);
    btnMenu->setObjectName("btnMenu");
    btnMenu->setCursor(QCursor(Qt::PointingHandCursor));
    btnMenu->setFixedSize(QSize(16, 16));

    QFrame *line = new QFrame(this);
    line->setMinimumSize(QSize(1, 16));
    line->setMaximumSize(QSize(1, 16));
    line->setStyleSheet(QStringLiteral("background-color:#393939;"));
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);

    //未激活文本
    m_activate_label = new QLabel(tr("Unactivated"), this);
    m_activate_label->setAlignment(Qt::AlignCenter);
    m_activate_label->setFixedHeight(18);
    m_activate_label->setMinimumWidth(50);
    m_activate_label->setStyleSheet("QLabel{"
                                    "background:rgba(255, 61, 61,255);;"
                                    "font-family: Noto Sans CJK SC regular;"
                                    "font-size:12px;"
                                    "border-radius: 8px}");

    titleBarLayout->addStretch();
    titleBarLayout->addWidget(m_activate_label);
    titleBarLayout->addWidget(btnMenu, Qt::AlignRight);
    titleBarLayout->addWidget(line);

    //创建标题栏中菜单
    QMenu *menu = new QMenu(this);
    QFont font = menu->font();
    font.setPixelSize(12);
    menu->setFont(font);

    menu->addActions(QList<QAction *>() << new QAction(tr("Config server"), menu) << new QAction(tr("Activate"), menu) << new QAction(tr("About"), menu));
    btnMenu->setMenu(menu);
    connect(menu, &QMenu::triggered, this, &LoginDialog::onMenuTrigger);

    //添加输入框布局，设置图标
    QHBoxLayout *userLayout = new QHBoxLayout(ui->lineEdit_username);
    userLayout->setContentsMargins(10, 0, 10, 0);
    userLayout->setSpacing(10);
    QLabel *userIcon = new QLabel(ui->lineEdit_username);
    userIcon->setPixmap(QPixmap(":/images/user_icon.svg"));

    QPushButton *closeBtn = new QPushButton(ui->lineEdit_username);
    closeBtn->setCursor(QCursor(Qt::PointingHandCursor));
    closeBtn->setFixedSize(16, 16);
    closeBtn->setIcon(QIcon(":/images/clear_icon.svg"));
    closeBtn->hide();
    closeBtn->setStyleSheet("border:none;outline:none;");
    connect(closeBtn, &QPushButton::clicked,
            [this] {
                ui->lineEdit_username->clear();
            });

    userLayout->addWidget(userIcon);
    userLayout->addStretch();
    userLayout->addWidget(closeBtn);
    ui->lineEdit_username->setPlaceholderText(tr("Please input user name"));
    ui->lineEdit_username->setTextMargins(30, 0, 30, 0);
    connect(ui->lineEdit_username, &QLineEdit::textEdited,
            [=](QString str) {
                if (str.isEmpty())
                    closeBtn->hide();
                else
                    closeBtn->show();
            });

    QHBoxLayout *pwLayout = new QHBoxLayout(ui->lineEdit_passwd);
    pwLayout->setContentsMargins(10, 0, 10, 0);
    pwLayout->setSpacing(10);
    QLabel *pwIcon = new QLabel(ui->lineEdit_passwd);
    pwIcon->setPixmap(QPixmap(":/images/pw_icon.svg"));

    pwLayout->addWidget(pwIcon);
    pwLayout->addStretch();
    ui->lineEdit_passwd->setEchoMode(QLineEdit::Password);
    ui->lineEdit_passwd->setPlaceholderText(tr("Please input password"));
    ui->lineEdit_passwd->setTextMargins(30, 0, 10, 0);

    connect(ui->btn_login, &QPushButton::clicked, this, &LoginDialog::onLogin);
}

void LoginDialog::initActivation()
{
    //创建激活界面
    m_activateDialog = new LicenseActivation(this);
    m_activateDialog->hide();

    m_licenseProxy = LicenseProxy::getDefault();
    m_activate_label->setVisible(!m_licenseProxy->isActivated());
    connect(m_licenseProxy.data(), &LicenseProxy::licenseChanged, this, &LoginDialog::updateActivation, Qt::UniqueConnection);
}

bool LoginDialog::inspectLoginParam()
{
    if (ui->lineEdit_username->text().isEmpty())
    {
        ui->lab_tips->setText(tr("Please input user name!"));
        ui->lab_tips->show();
        return false;
    }
    if (ui->lineEdit_passwd->text().isEmpty())
    {
        ui->lab_tips->setText(tr("Please input password!"));
        ui->lab_tips->show();
        return false;
    }
    else if (ui->lineEdit_passwd->text().size() < 8)
    {
        ui->lab_tips->setText(tr("Please enter a password with at least 8 digits!"));
        ui->lab_tips->show();
        return false;
    }
    m_server = m_serverCfgDlg->getServerInfo();
    if (m_server.isEmpty())
    {
        ui->lab_tips->setText(tr("Please config a server address!"));
        ui->lab_tips->show();
        m_serverCfgDlg->show();
        return false;
    }
    return true;
}

void LoginDialog::onMenuTrigger(QAction *act)
{
    if (act->text() == tr("Config server"))
    {
        m_serverCfgDlg->show();
        m_serverCfgDlg->setServerInfo();
    }
    else if (act->text() == tr("Activate"))
        showActivatePage();
    else if (act->text() == tr("About"))
        showAboutPage();
}

void LoginDialog::showActivatePage()
{
    int x = this->x() + this->width() / 2 - m_activateDialog->width() / 2;
    int y = this->y() + this->height() / 2 - m_activateDialog->height() / 2;
    m_activateDialog->move(x, y);
    m_activateDialog->show();
}

void LoginDialog::showAboutPage()
{
    int x = this->x() + this->width() / 2 - m_about->width() / 2;
    int y = this->y() + this->height() / 2 - m_about->height() / 2;

    m_about->move(x, y);
    m_about->show();
}

void LoginDialog::updateActivation()
{
    bool isActivate = m_licenseProxy->isActivated();
    if (!isActivate && m_isLogin)
    {
        onLogout();
    }
    //设置激活对话框和激活状态标签是否可见
    // license变化只处理激活后隐藏界面, Fix #34441
    if (isActivate)
    {
        m_activateDialog->setVisible(!isActivate);
        m_activate_label->setVisible(!isActivate);
    }
}

void LoginDialog::onLogin()
{
    if (!m_licenseProxy->isActivated())
    {
        KLOG_DEBUG() << "The application is not activated.";
        //弹出提示
        KiranMessageBox::message(nullptr,
                                 tr("Activation"),
                                 tr("The application is not activated and unable to login. Please activate it."),
                                 KiranMessageBox::Ok);
        //跳转激活界面
        showActivatePage();
        return;
    }

    if (!inspectLoginParam())
        return;

    InfoWorker::getInstance().login(m_objID, ui->lineEdit_username->text().toStdString(), ui->lineEdit_passwd->text().toStdString());
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));  //等待旋转
}

void LoginDialog::onLogout()
{
    InfoWorker::getInstance().logout(m_objID);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));  //等待旋转
}

void LoginDialog::getLoginResult(const QString objID, const QPair<grpc::Status, user::LoginReply> &reply)
{
    if (m_objID != objID)
        return;

    QApplication::restoreOverrideCursor();

    if (reply.first.ok())
    {
        auto userRole = reply.second.user_role().name().data();
        if (0 != QString::compare(userRole, USER_ROLE_SYSADM) &&
            0 != QString::compare(userRole, USER_ROLE_SECADM) &&
            0 != QString::compare(userRole, USER_ROLE_AUDADM))
        {
            auto msg = tr("Login failed: there is no %1 user role").arg(userRole);
            KLOG_WARNING() << msg;
            ui->lab_tips->setText(msg);
            ui->lab_tips->show();
            ui->lineEdit_passwd->clear();
            return;
        }

        m_isLogin = true;
        m_isSessionExpired = false;
        if (!m_mainWindow)
        {
            m_mainWindow = new MainWindow(ui->lineEdit_username->text(), userRole);
            m_mainWindow->showMaximized();
            m_mainWindow->installEventFilter(this);
            connect(m_mainWindow, &MainWindow::sigLogout, this, &LoginDialog::onLogout);
            hide();
        }
        UserConfiguration::getInstance().writeConfig(CONFIG_SETTING_TYPE_LOGIN, ui->lineEdit_username->text(), USERNAME, ui->lineEdit_username->text());
        UserConfiguration::getInstance().writeConfig(CONFIG_SETTING_TYPE_LOGIN, ui->lineEdit_username->text(), PASSWORD, ui->lineEdit_passwd->text());
    }
    else
    {
        KLOG_WARNING() << "Login failed:" << reply.first.error_message().data();
        ui->lab_tips->setText(tr("Login failed %1").arg(reply.first.error_message().data()));
        ui->lab_tips->show();
        ui->lineEdit_passwd->clear();
    }
}

void LoginDialog::getLogoutResult(const QString objID, const QPair<grpc::Status, user::LogoutReply> &reply)
{
    if (m_objID != objID)
        return;

    QApplication::restoreOverrideCursor();

    if (reply.first.ok() || reply.first.error_code() == UNAUTHENTICATED)
    {
        m_isLogin = false;

        if (m_mainWindow)
        {
            delete m_mainWindow;
            m_mainWindow = nullptr;
        }
        ui->lineEdit_passwd->clear();
        if (m_isSessionExpired)
        {
            ui->lab_tips->setText(tr("Session Expired,Please login again!"));
        }
        else
        {
            ui->lab_tips->clear();
        }
        ui->lab_tips->setVisible(!ui->lab_tips->text().isEmpty());
        show();
    }
    else
    {
        MessageDialog::message(tr("Logout"),
                               tr("Logout failed!"),
                               tr("Error: ") + reply.first.error_message().data(),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        KLOG_WARNING() << "Logout failed:" << reply.first.error_message().data();
    }
}

void LoginDialog::sessionExpire()
{
    //注销状态下不处理
    if (!m_isLogin)
        return;

    KLOG_DEBUG() << "get session expired!";

    if (!m_sessionMutex.tryLock())
    {
        KLOG_DEBUG() << "Get lock fail and return";
        return;
    }

    m_isSessionExpired = true;
    onLogout();
    m_sessionMutex.unlock();
}
