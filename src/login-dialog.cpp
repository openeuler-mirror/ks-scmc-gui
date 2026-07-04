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
                                            m_serverCfgDlg(nullptr),
                                            m_activate_page(nullptr),
                                            m_dbusutil(nullptr)
{
    ui->setupUi(getWindowContentWidget());

    initMessageBox();
    m_license = new License;
    m_serverCfgDlg = new ServerConfigDialog(this);
    m_serverCfgDlg->hide();
    m_activate_page = new ActivatePage(this);
    m_activate_page->hide();
    m_dbusutil = new DBusUtils(this);

    connect(m_dbusutil, SIGNAL(LicenseChanged(bool)), this, SLOT(updateLicense(bool)));
    connect(m_dbusutil, SIGNAL(callDbusFailed()), this, SLOT(showErrorBox()));

    QString licence_str = m_dbusutil->callInterface(GET_LICENSE);
    getLicense(licence_str);

    initUI();
    connect(&InfoWorker::getInstance(), &InfoWorker::sessinoExpire, this, &LoginDialog::sessionExpire);
    m_activate_page->setText(m_license->machine_code,m_license->activation_code,m_license->activation_time,m_license->expired_time);
    connect(m_activate_page,&ActivatePage::activate_app,this,&LoginDialog::activation);
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
    if(m_activate_page)
    {
        delete m_activate_page;
        m_activate_page = nullptr;
    }
    if(m_dbusutil)
    {
        delete m_dbusutil;
        m_dbusutil = nullptr;
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

    //未激活文本
    activate_label = new QLabel(this);
    activate_label->setAlignment(Qt::AlignHCenter);
    activate_label->setFixedHeight(18);
    activate_label->setMinimumWidth(50);
    if(m_license->activation_status != LicenseActivationStatus::LAS_ACTIVATED)
    {
        activate_label->setStyleSheet("QLabel{"
                                     "background:rgba(255, 61, 61,255);;"
                                     "font-family: Noto Sans CJK SC regular;"
                                     "font-size:12px;"
                                     "border-radius: 8px}");
        activate_label->setText(tr("Unactivated"));
    }

    titleBarLayout->addStretch();
    titleBarLayout->addWidget(activate_label);
    titleBarLayout->addWidget(btnMenu, Qt::AlignRight);
    titleBarLayout->addWidget(line);

    //创建标题栏中菜单
    QMenu *menu = new QMenu(this);
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

    //TODO:just test
    ui->lineEdit_username->setText("yuanxing");
    ui->lineEdit_passwd->setText("12345678");

    connect(ui->btn_login, &QPushButton::clicked, this, &LoginDialog::onLogin);
//    connect(m_activate_page,SIGNAL(activate(QString)),this,SLOT(activation(QSting)));
}

void LoginDialog::initMessageBox()
{
    m_errorMessageBox = new ErrorMessageBox();
    m_errorMessageBox->hide();

    m_activate_page_box = new KiranMessageBox(this);
    QPushButton *activeButton = new QPushButton(tr("Active"));
    activeButton->setStyleSheet("QPushButton{"
                          "color:black;"
                          "font:NotoSansCJKsc-Regular;"
                          "font-size:14px;"
                          "border-radius:4px;"
                          "background:rgba(255,255,255,255);}");
    activeButton->setFocusPolicy(Qt::NoFocus);
    m_activate_page_box->addButton(activeButton, QDialogButtonBox::AcceptRole);
    m_activate_page_box->setButtonSize(QSize(80, 30));
    m_activate_page_box->setText(tr("The software is not activated and the reinforcement function cannot be used. Please activate it."));
    connect(activeButton, SIGNAL(clicked(bool)), this, SLOT(showActivatePage()));

    m_dbusErrorBox = new KiranMessageBox(this);
    m_dbusErrorBox->setWindowFlag(Qt::WindowStaysOnTopHint);
    QPushButton *okButton = new QPushButton(tr("OK"));
    okButton->setStyleSheet("QPushButton{"
                          "color:black;"
                          "font:NotoSansCJKsc-Regular;"
                          "font-size:14px;"
                          "border-radius:4px;"
                          "background:rgba(255,255,255,255);}");
    okButton->setFocusPolicy(Qt::NoFocus);
    m_dbusErrorBox->addButton(okButton, QDialogButtonBox::AcceptRole);
    m_dbusErrorBox->setButtonSize(QSize(80, 30));
    m_dbusErrorBox->setText(tr("Failed to get data, please check the backend service status."));
    connect(okButton, SIGNAL(clicked(bool)), this, SLOT(close()));
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

void LoginDialog::getLicense(QString license_str)
{
    QJsonParseError jsonerror;
    QJsonDocument doc = QJsonDocument::fromJson(license_str.toLatin1(), &jsonerror);
    if (!doc.isNull() && jsonerror.error == QJsonParseError::NoError)
    {
        if (doc.isObject())
        {
            QJsonObject object = doc.object();
            //objectParsing(&object);
            QJsonObject::iterator it = object.begin();
            while (it != object.end())
            {
                switch (it.value().type())
                {
                    case QJsonValue::String:
                    {
                        QString jsonKey = it.key();
                        QString jsonString = it.value().toString();
                        if (jsonKey == "activation_code")
                        {
                            m_license->activation_code = jsonString;
                        }
                        else if (jsonKey == "machine_code")
                        {
                            m_license->machine_code = jsonString;
                        }
                        break;
                    }
                    case QJsonValue::Double:
                    {
                        QString jsonKey = it.key();
                        if (jsonKey == "activation_status")
                        {
                            m_license->activation_status = it.value().toDouble();
                        }
                        else if (jsonKey == "activation_time")
                        {
                            m_license->activation_time = it.value().toDouble();
                        }
                        else if (jsonKey == "expired_time")
                        {
                            m_license->expired_time = it.value().toDouble();
                        }
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                it++;
            }
        }
    }
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
}

void LoginDialog::showActivatePage()
{
    int x = this->x() / 2 + m_activate_page->width() / 2;
    int y = this->y() / 2 + m_activate_page->height() / 2;
    m_activate_page->move(x,y);
    m_activate_page->show();
}

void LoginDialog::showErrorBox()
{
    int x = this->x() + this->width() / 2 + m_dbusErrorBox->width() / 4;
    int y = this->y() + this->height() / 2 + m_dbusErrorBox->height() / 4;
    m_dbusErrorBox->move(x, y);
    m_dbusErrorBox->show();
    //this->close();
}

void LoginDialog::activation(QString activation_code)
{
    if(activation_code.isNull())
    {
        KLOG_DEBUG() << "Null activation_code";
        return;
    }

    m_license->activation_code = activation_code;
    if(!m_dbusutil->callInterface(ACTIVATE_BYACTIVATIONCODE, m_license->activation_code))
    {
        m_activate_page->showErrorBox();
    }
}

void LoginDialog::onLogin()
{
    if(m_license->activation_status != LicenseActivationStatus::LAS_ACTIVATED)
    {
        //弹出提示
        //跳转激活界面
        m_activate_page_box->show();
        KLOG_DEBUG() << "Not activated.";
        return;
    }

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

void LoginDialog::updateLicense(bool ret)
{
    if(ret)
    {
        KLOG_DEBUG() << "license changed!";
        QString license_str = m_dbusutil->callInterface(GET_LICENSE);
        getLicense(license_str);

        if(m_license->activation_status == LicenseActivationStatus::LAS_ACTIVATED)
        {
            activate_label->hide();
        }

        m_activate_page->setText(m_license->machine_code, m_license->activation_code, m_license->activation_time, m_license->expired_time);
        m_activate_page->showAcitvedBox();
    }
}
