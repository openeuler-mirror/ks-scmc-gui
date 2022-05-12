#include "login-dialog.h"
#include <cryptopp/base64.h>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <kiran-log/qt5-log-i.h>
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QMenu>
#include <QPainter>
#include <QSettings>
#include <QTimer>
#include "common/message-dialog.h"
#include "main-window.h"
#include "pages/user/server-config-dialog.h"
#include "ui_login-dialog.h"
#include "user-configuration.h"
#define TIMEOUT 300

using namespace CryptoPP;

//std::string g_server_addr = "10.200.12.181:10060";
//std::string g_server_addr;
LoginDialog::LoginDialog(QWidget *parent) : KiranTitlebarWindow(parent),
                                            ui(new Ui::LoginDialog),
                                            m_mainWindow(nullptr),
                                            m_serverCfgDlg(nullptr)
{
    ui->setupUi(getWindowContentWidget());

    m_serverCfgDlg = new ServerConfigDialog(this);
    m_serverCfgDlg->hide();

    initUI();
    connect(&InfoWorker::getInstance(), &InfoWorker::sessinoExpire, this, &LoginDialog::sessionExpire);
    //loadConfig();
}

LoginDialog::~LoginDialog()
{
    KLOG_INFO() << "*************Deconstruction LoginDialog";
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
}

void LoginDialog::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LoginDialog::initUI()
{
    setTitle(tr("KylinSec security Container magic Cube"));
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

    titleBarLayout->addStretch();
    titleBarLayout->addWidget(btnMenu, Qt::AlignRight);
    titleBarLayout->addWidget(line);

    //创建标题栏中菜单
    QMenu *menu = new QMenu(this);
    menu->addActions(QList<QAction *>() << new QAction(tr("Config server"), menu) << new QAction(tr("About"), menu));
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

    //TODO:just test
    ui->lineEdit_username->setText("yuanxing");
    ui->lineEdit_passwd->setText("12345678");

    connect(ui->btn_login, &QPushButton::clicked, this, &LoginDialog::onLogin);
}

void LoginDialog::loadConfig()
{
    //    readConfig("username", m_usernames);
    //    readConfig("password", m_passwds);
    //    readConfig("server", m_servers);
    //    //readConfig("remember_passwd", m_isRemembera);
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
}

void LoginDialog::onLogin()
{
    if (!inspectLoginParam())
        return;

    connect(&InfoWorker::getInstance(), &InfoWorker::loginFinished, this, &LoginDialog::getLoginResult);
    UserConfiguration::getInstance().setServerAddr(m_server.toStdString());
    InfoWorker::getInstance().login(ui->lineEdit_username->text().toStdString(), ui->lineEdit_passwd->text().toStdString());
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));  //等待旋转
}

void LoginDialog::onLogout()
{
    InfoWorker::getInstance().logout();
    connect(&InfoWorker::getInstance(), &InfoWorker::logoutFinished, this, &LoginDialog::getLogoutResult);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));  //等待旋转
}

void LoginDialog::getLoginResult(const QPair<grpc::Status, user::LoginReply> &reply)
{
    KLOG_INFO() << "getLoginResult";
    QApplication::restoreOverrideCursor();

    if (reply.first.ok())
    {
        if (!m_mainWindow)
        {
            m_mainWindow = new MainWindow();
            m_mainWindow->setUserName(ui->lineEdit_username->text());
            m_mainWindow->showMaximized();
            connect(m_mainWindow, &MainWindow::sigLogout, this, &LoginDialog::onLogout);
            hide();
        }
        UserConfiguration::getInstance().writeConfig(CONFIG_SETTING_TYPE_LOGIN, ui->lineEdit_username->text(), USERNAME, ui->lineEdit_username->text());
        UserConfiguration::getInstance().writeConfig(CONFIG_SETTING_TYPE_LOGIN, ui->lineEdit_username->text(), PASSWORD, ui->lineEdit_passwd->text());
    }
    else
    {
        ui->lab_tips->setText(tr("Login failed %1").arg(reply.first.error_message().data()));
        ui->lab_tips->show();
        ui->lineEdit_passwd->clear();
    }
}

void LoginDialog::getLogoutResult(const QPair<grpc::Status, user::LogoutReply> &reply)
{
    KLOG_INFO() << "getLogoutResult";
    QApplication::restoreOverrideCursor();

    if (reply.first.ok())
    {
        if (m_mainWindow)
        {
            delete m_mainWindow;
            m_mainWindow = nullptr;
        }
        show();
        ui->lineEdit_passwd->clear();
        ui->lab_tips->clear();
        ui->lab_tips->hide();
    }
    else
    {
        MessageDialog::message(tr("Logout"),
                               tr("Logout failed!"),
                               tr("Error: ") + reply.first.error_message().data(),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void LoginDialog::sessionExpire()
{
    KLOG_INFO() << "sessionExpire";
    MessageDialog::message(tr("Login"),
                           tr("session expire!"),
                           tr("back to login page"),
                           ":/images/warning.svg",
                           MessageDialog::StandardButton::Ok);
    if (m_mainWindow)
    {
        delete m_mainWindow;
        m_mainWindow = nullptr;
    }
    show();
    ui->lineEdit_passwd->clear();
    ui->lab_tips->clear();
    ui->lab_tips->hide();
}
