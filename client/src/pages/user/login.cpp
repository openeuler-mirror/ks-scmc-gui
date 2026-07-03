#include "login.h"
#include <cryptopp/base64.h>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QPainter>
#include <QSettings>
#include <QTimer>
#include "common/message-dialog.h"
#include "main-window.h"
#include "ui_login.h"

//#define LOGIN_CONFIG "usr/share/ks-scmc-gui/login.ini"
#define LOGIN_CONFIG "login.ini"
#define TIMEOUT 300

using namespace CryptoPP;

//std::string g_server_addr = "10.200.12.181:10060";
std::string g_server_addr;
Login::Login(QWidget *parent) : KiranTitlebarWindow(parent),
                                ui(new Ui::Login),
                                m_mainWindow(nullptr),
                                m_timer(nullptr)
{
    ui->setupUi(getWindowContentWidget());
    QStringList args = qApp->arguments();
    if (args.size() > 1)
    {
        g_server_addr = args[1].toStdString();
    }

    m_loginSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "kylinsec", "ks-scmc-gui");
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
    ui->lineEdit_passwd->setEchoMode(QLineEdit::Password);
    ui->lineEdit_passwd->setPlaceholderText(tr("Please input password"));
    ui->lineEdit_server->setPlaceholderText(tr("Please input server address"));
    ui->lineEdit_username->setPlaceholderText(tr("Please input user name"));

    //    ui->lineEdit_username->setText("test");
    //    ui->lineEdit_passwd->setText("12345678");
    //    ui->lineEdit_server->setText("10.200.12.181:10060");

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

    if (ui->lineEdit_server->text().isEmpty())
    {
        ui->lab_tips->setText(tr("Please enter a server address!"));
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

void Login::onLogin()
{
    if (!inspectLoginParam())
        return;

    connect(&InfoWorker::getInstance(), &InfoWorker::loginFinished, this, &Login::getLoginResult);
    g_server_addr = ui->lineEdit_server->text().toStdString();
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
            writeConfig("server", ui->lineEdit_server->text());
        }
    }
    else
    {
        ui->lab_tips->setText(tr("Login failed! %1").arg(reply.first.error_message().data()));
        ui->lineEdit_passwd->clear();
        ui->lineEdit_server->clear();
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
