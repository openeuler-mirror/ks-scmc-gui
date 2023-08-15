#include "login.h"
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
#include "ui_login.h"

//#define LOGIN_CONFIG "usr/share/ks-scmc-gui/user.ini"
#define LOGIN_CONFIG "user.ini"
#define TIMEOUT 300

using namespace CryptoPP;

//std::string g_server_addr = "10.200.12.181:10060";
std::string g_server_addr;
Login::Login(QWidget *parent) : KiranTitlebarWindow(parent),
                                ui(new Ui::Login),
                                m_mainWindow(nullptr),
                                m_timer(nullptr),
                                m_serverConfig(nullptr)
{
    ui->setupUi(getWindowContentWidget());
    QStringList args = qApp->arguments();
    if (args.size() > 1)
    {
        g_server_addr = args[1].toStdString();
    }
    m_serverConfig = new ServerConfiguration(this);
    m_serverConfig->hide();

    m_loginSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "kylinsec", "ks-scmc-user");
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Login::getPasswd);

    initUI();
    //loadConfig();
}

Login::~Login()
{
    KLOG_INFO() << "*************Deconstruction Login";
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
    if (m_loginSettings)
    {
        delete m_loginSettings;
        m_loginSettings = nullptr;
    }
    if (m_serverConfig)
    {
        delete m_serverConfig;
        m_serverConfig = nullptr;
    }
}

void Login::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void Login::initUI()
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
    connect(menu, &QMenu::triggered, this, &Login::onMenuTrigger);

    ui->lineEdit_passwd->setEchoMode(QLineEdit::Password);
    ui->lineEdit_passwd->setPlaceholderText(tr("Please input password"));
    ui->lineEdit_username->setPlaceholderText(tr("Please input user name"));

    ui->lineEdit_username->setText("test");
    ui->lineEdit_passwd->setText("12345678");

    connect(ui->btn_login, &QPushButton::clicked, this, &Login::onLogin);
    connect(ui->radio_btn_remember_pw, &QRadioButton::toggled, this, &Login::onBtnRemember);
    connect(ui->lineEdit_username, &QLineEdit::textEdited,
            [this](QString text) {
                if (!text.isEmpty())
                    m_timer->start(TIMEOUT);
            });
}

void Login::loadConfig()
{
    //    readConfig("username", m_usernames);
    //    readConfig("password", m_passwds);
    //    readConfig("server", m_servers);
    //    //readConfig("remember_passwd", m_isRemembera);
}

void Login::readConfig(QString key, QString &value)
{
    value.clear();
    QString group = ui->lineEdit_username->text();
    KLOG_INFO() << group;

    //读取配置信息
    QString encryptedPw = m_loginSettings->value(group + "/" + key).toString();
    if (!encryptedPw.isEmpty())
    {
        std::string decryptedPw = desDecrypt(encryptedPw.toStdString());
        value = QString::fromStdString(decryptedPw);
    }
}

void Login::writeConfig(QString key, QString value)
{
    QString group = ui->lineEdit_username->text();

    QVariant variant;
    variant.setValue(value);
    //将信息写入配置文件
    m_loginSettings->beginGroup(group);
    m_loginSettings->setValue(key, variant);
    m_loginSettings->endGroup();
}

bool Login::inspectLoginParam()
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
    m_server = m_serverConfig->getServerInfo();
    if (m_server.isEmpty())
    {
        ui->lab_tips->setText(tr("Please config a server address!"));
        m_serverConfig->show();
        return false;
    }
    return true;
}

std::string Login::desEncrypt(const std::string &message, const std::string &key)
{
    try
    {
        std::string result;
        ECB_Mode<DES>::Encryption encoder;
        // 这里的key长度必须为8
        encoder.SetKey((const byte *)key.c_str(), key.length());
        StringSource(message, true, new StreamTransformationFilter(encoder, new Base64Encoder(new StringSink(result))));
        return result;
    }
    catch (const CryptoPP::Exception &e)
    {
        KLOG_WARNING("%s.", e.what());
        return std::string();
    }
}

std::string Login::desDecrypt(const std::string &message, const std::string &key)
{
    try
    {
        std::string result;
        ECB_Mode<DES>::Decryption decoder;
        decoder.SetKey((const byte *)key.c_str(), key.length());
        StringSource(message, true, new Base64Decoder(new StreamTransformationFilter(decoder, new StringSink(result))));
        return result;
    }
    catch (const CryptoPP::Exception &e)
    {
        KLOG_WARNING("%s.", e.what());
        return std::string();
    }
}

void Login::getPasswd()
{
    QString decryptedPw;
    readConfig("password", decryptedPw);
    if (!decryptedPw.isNull())
    {
        ui->lineEdit_passwd->setText(decryptedPw);
    }
    m_timer->stop();
}

void Login::onBtnRemember(bool checked)
{
    m_isRemember = checked;
}

void Login::onMenuTrigger(QAction *act)
{
    if (act->text() == tr("Config server"))
    {
        m_serverConfig->show();
        m_serverConfig->setServerInfo();
    }
}

void Login::onLogin()
{
    if (!inspectLoginParam())
        return;

    connect(&InfoWorker::getInstance(), &InfoWorker::loginFinished, this, &Login::getLoginResult);
    g_server_addr = m_server.toStdString();
    InfoWorker::getInstance().login(ui->lineEdit_username->text().toStdString(), ui->lineEdit_passwd->text().toStdString());
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));  //等待旋转
}

void Login::onLogout()
{
    InfoWorker::getInstance().logout();
    connect(&InfoWorker::getInstance(), &InfoWorker::logoutFinished, this, &Login::getLogoutResult);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));  //等待旋转
}

void Login::getLoginResult(const QPair<grpc::Status, user::LoginReply> &reply)
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
            connect(m_mainWindow, &MainWindow::sigLogout, this, &Login::onLogout);
            int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
            QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
            int iTitleBarHeight = m_mainWindow->style()->pixelMetric(QStyle::PM_TitleBarHeight);                               // 获取标题栏高度
            m_mainWindow->setGeometry(0, iTitleBarHeight, screenGeometry.width(), screenGeometry.height() - iTitleBarHeight);  // 设置窗体充满桌面客户区
            hide();
        }
        if (m_isRemember)
        {
            QString encryptedPw = QString::fromStdString(desEncrypt(ui->lineEdit_passwd->text().toStdString()));
            writeConfig("username", ui->lineEdit_username->text());
            writeConfig("password", encryptedPw);
        }
    }
    else
    {
        ui->lab_tips->setText(tr("Login failed! %1").arg(reply.first.error_message().data()));
        ui->lineEdit_passwd->clear();
    }
}

void Login::getLogoutResult(const QPair<grpc::Status, user::LogoutReply> &reply)
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

ServerConfiguration::ServerConfiguration(QWidget *parent) : KiranTitlebarWindow(parent),
                                                            m_ipLineEdit(nullptr),
                                                            m_portLineEdit(nullptr),
                                                            m_serverSetting(nullptr)
{
    m_serverSetting = new QSettings(QSettings::IniFormat, QSettings::UserScope, "kylinsec", "ks-scmc-server");
    initUI();
    setServerInfo();
}

ServerConfiguration::~ServerConfiguration()
{
    if (m_serverSetting)
    {
        delete m_serverSetting;
        m_serverSetting = nullptr;
    }
}

QString ServerConfiguration::getServerInfo()
{
    //10.200.12.181:10060
    QString server;
    if (!m_ip.isEmpty() && !m_port.isEmpty())
        server = m_ip + ":" + m_port;
    else
        server = "";
    return server;
}

void ServerConfiguration::ServerConfiguration::initUI()
{
    setWindowModality(Qt::WindowModal);
    setTitle(tr("Config Server"));
    setButtonHints(TitlebarButtonHint::TitlebarCloseButtonHint);
    this->setFixedSize(400, 300);

    QWidget *windowContentWidget = getWindowContentWidget();
    QVBoxLayout *mainVLayout = new QVBoxLayout(windowContentWidget);
    mainVLayout->setSpacing(10);
    this->setLayout(mainVLayout);

    QWidget *ipWidget = new QWidget(windowContentWidget);
    QVBoxLayout *ipLayout = new QVBoxLayout(ipWidget);
    ipLayout->setMargin(0);
    ipLayout->setSpacing(10);
    QLabel *ipLab = new QLabel(tr("Ip Address"), ipWidget);
    m_ipLineEdit = new QLineEdit(ipWidget);
    m_ipLineEdit->setFixedHeight(32);
    m_ipLineEdit->setPlaceholderText(tr("Please input server ip address"));
    ipLayout->addWidget(ipLab);
    ipLayout->addWidget(m_ipLineEdit);

    QWidget *portWidget = new QWidget(windowContentWidget);
    QVBoxLayout *portLayout = new QVBoxLayout(portWidget);
    portLayout->setMargin(0);
    portLayout->setSpacing(10);
    QLabel *portLab = new QLabel(tr("Port"), portWidget);
    m_portLineEdit = new QLineEdit(portWidget);
    m_portLineEdit->setFixedHeight(32);
    m_portLineEdit->setPlaceholderText(tr("Please input port"));
    portLayout->addWidget(portLab);
    portLayout->addWidget(m_portLineEdit);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *confirmBtn = new QPushButton(tr("Confirm"));
    QPushButton *cancleBtn = new QPushButton(tr("Cancle"));
    btnLayout->addWidget(confirmBtn);
    btnLayout->addWidget(cancleBtn);

    mainVLayout->addWidget(ipWidget);
    mainVLayout->addWidget(portWidget);
    mainVLayout->addStretch();
    mainVLayout->addLayout(btnLayout);

    connect(confirmBtn, &QPushButton::clicked,
            [this] {
                if (m_ipLineEdit->text().isEmpty() || m_portLineEdit->text().isEmpty())
                {
                    KiranMessageBox::message(nullptr,
                                             tr("Failed"),
                                             tr("Please input complete server information!"),
                                             KiranMessageBox::Ok);
                    return;
                }
                m_ip = m_ipLineEdit->text();
                m_port = m_portLineEdit->text();
                writeConfig("Ip", m_ip);
                writeConfig("Port", m_port);
                close();
            });
    connect(cancleBtn, &QPushButton::clicked,
            [this] {
                close();
            });
}

void ServerConfiguration::readConfig(QString key, QString &value)
{
    value.clear();
    QString group = "Server";
    KLOG_INFO() << group;

    //读取配置信息
    value = m_serverSetting->value(group + "/" + key).toString();
}

void ServerConfiguration::writeConfig(QString key, QString value)
{
    QString group = "Server";

    QVariant variant;
    variant.setValue(value);
    //将信息写入配置文件

    m_serverSetting->beginGroup(group);
    m_serverSetting->setValue(key, variant);
    m_serverSetting->endGroup();
}

void ServerConfiguration::setServerInfo()
{
    m_ip.clear();
    m_port.clear();
    readConfig("Ip", m_ip);
    m_ipLineEdit->setText(m_ip);
    readConfig("Port", m_port);
    m_portLineEdit->setText(m_port);
}
