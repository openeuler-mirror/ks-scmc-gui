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
#include "base/user-configuration.h"
#include "common/message-dialog.h"
#include "main-window.h"
#include "pages/user/server-config-dialog.h"
#include "ui_login-dialog.h"

#define LOGIN_USER_NAME "username"
#define LOGIN_USER_PASSWORD "password"
#define TIMEOUT 300

using namespace CryptoPP;

//std::string g_server_addr = "10.200.12.181:10060";
//std::string g_server_addr;
LoginDialog::LoginDialog(QWidget *parent) : KiranTitlebarWindow(parent),
                                            ui(new Ui::LoginDialog),
                                            m_mainWindow(nullptr),
                                            m_timer(nullptr),
                                            m_serverCfgDlg(nullptr),
                                            m_userConfig(nullptr)
{
    ui->setupUi(getWindowContentWidget());

    m_serverCfgDlg = new ServerConfigDialog(this);
    m_serverCfgDlg->hide();

    m_userConfig = new UserConfiguration();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &LoginDialog::getPasswd);

    initUI();
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
    if (m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }
    if (m_userConfig)
    {
        delete m_userConfig;
        m_userConfig = nullptr;
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

    //创建标题栏中菜单按钮
    setTitlebarCustomLayoutAlignHCenter(false);
    QHBoxLayout *titleBarLayout = getTitlebarCustomLayout();
    QPushButton *btnMenu = new QPushButton(this);
    btnMenu->setFixedSize(QSize(16, 16));
    titleBarLayout->addStretch();
    titleBarLayout->addWidget(btnMenu, Qt::AlignRight);

    //创建标题栏中菜单
    QMenu *menu = new QMenu(this);
    menu->addActions(QList<QAction *>() << new QAction(tr("Config server"), menu) << new QAction(tr("About"), menu));
    btnMenu->setMenu(menu);
    connect(menu, &QMenu::triggered, this, &LoginDialog::onMenuTrigger);

    ui->lineEdit_passwd->setEchoMode(QLineEdit::Password);
    ui->lineEdit_passwd->setPlaceholderText(tr("Please input password"));
    ui->lineEdit_username->setPlaceholderText(tr("Please input user name"));

    //ui->lineEdit_username->setText("test");
    //ui->lineEdit_passwd->setText("12345678");

    connect(ui->btn_login, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(ui->radio_btn_remember_pw, &QRadioButton::toggled, this, &LoginDialog::onBtnRemember);
    connect(ui->lineEdit_username, &QLineEdit::textEdited,
            [this](QString text) {
                if (!text.isEmpty())
                    m_timer->start(TIMEOUT);
            });
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
        return false;
    }
    if (ui->lineEdit_passwd->text().isEmpty())
    {
        ui->lab_tips->setText(tr("Please input password!"));
        return false;
    }
    else if (ui->lineEdit_passwd->text().size() < 8)
    {
        ui->lab_tips->setText(tr("Please enter a password with at least 8 digits!"));
        return false;
    }
    m_server = m_serverCfgDlg->getServerInfo();
    if (m_server.isEmpty())
    {
        ui->lab_tips->setText(tr("Please config a server address!"));
        m_serverCfgDlg->show();
        return false;
    }
    return true;
}

void LoginDialog::getPasswd()
{
    QString decryptedPw;
    m_userConfig->readConfig(CONFIG_SETTING_TYPE_LOGIN, ui->lineEdit_username->text(), LOGIN_USER_PASSWORD, decryptedPw);
    if (!decryptedPw.isNull())
    {
        ui->lineEdit_passwd->setText(decryptedPw);
    }
    m_timer->stop();
}

void LoginDialog::onBtnRemember(bool checked)
{
    m_isRemember = checked;
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
    m_userConfig->setServerAddr(m_server.toStdString());
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
            m_mainWindow->show();
            connect(m_mainWindow, &MainWindow::sigLogout, this, &LoginDialog::onLogout);
            int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
            QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
            int iTitleBarHeight = m_mainWindow->style()->pixelMetric(QStyle::PM_TitleBarHeight);                               // 获取标题栏高度
            m_mainWindow->setGeometry(0, iTitleBarHeight, screenGeometry.width(), screenGeometry.height() - iTitleBarHeight);  // 设置窗体充满桌面客户区
            hide();
        }
        if (m_isRemember)
        {
            QString encryptedPw = QString::fromStdString(m_userConfig->desEncrypt(ui->lineEdit_passwd->text().toStdString()));
            m_userConfig->writeConfig(CONFIG_SETTING_TYPE_LOGIN, ui->lineEdit_username->text(), LOGIN_USER_NAME, ui->lineEdit_username->text());
            m_userConfig->writeConfig(CONFIG_SETTING_TYPE_LOGIN, ui->lineEdit_username->text(), LOGIN_USER_PASSWORD, encryptedPw);
        }
    }
    else
    {
        ui->lab_tips->setText(tr("Login failed! %1").arg(reply.first.error_message().data()));
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
