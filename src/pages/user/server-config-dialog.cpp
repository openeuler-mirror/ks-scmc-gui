#include "server-config-dialog.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <widget-property-helper.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "user-configuration.h"

#define SERVER_GROUP "Server"
#define SERVER_IP "Ip"
#define SERVER_PORT "Port"

ServerConfigDialog::ServerConfigDialog(QWidget *parent) : KiranTitlebarWindow(parent),
                                                          m_ipLineEdit(nullptr),
                                                          m_portLineEdit(nullptr)
{
    initUI();
    setServerInfo();
}

ServerConfigDialog::~ServerConfigDialog()
{
}

QString ServerConfigDialog::getServerInfo()
{
    QString server;
    if (!m_ip.isEmpty() && !m_port.isEmpty())
        server = m_ip + ":" + m_port;
    else
        server = "";
    return server;
}

void ServerConfigDialog::initUI()
{
    setWindowModality(Qt::WindowModal);
    setTitle(tr("Config Server"));
    setIcon(QIcon(":/images/logo.png"));
    setButtonHints(TitlebarButtonHint::TitlebarCloseButtonHint);
    this->setFixedSize(460, 340);
    setResizeable(false);

    QWidget *windowContentWidget = getWindowContentWidget();
    QVBoxLayout *mainVLayout = new QVBoxLayout(windowContentWidget);
    mainVLayout->setSpacing(10);
    mainVLayout->setMargin(0);
    mainVLayout->setContentsMargins(24, 16, 24, 40);

    QWidget *ipWidget = new QWidget(windowContentWidget);
    QVBoxLayout *ipLayout = new QVBoxLayout(ipWidget);
    ipLayout->setMargin(0);
    ipLayout->setSpacing(10);
    QLabel *ipLab = new QLabel(tr("Ip Address"), ipWidget);
    m_ipLineEdit = new QLineEdit(ipWidget);
    m_ipLineEdit->setFixedSize(370, 36);
    m_ipLineEdit->setPlaceholderText(tr("Please input server ip address"));
    m_ipLineEdit->setTextMargins(10, 0, 0, 0);
    ipLayout->addWidget(ipLab);
    ipLayout->addWidget(m_ipLineEdit);

    QWidget *portWidget = new QWidget(windowContentWidget);
    QVBoxLayout *portLayout = new QVBoxLayout(portWidget);
    portLayout->setContentsMargins(0, 0, 0, 10);
    portLayout->setSpacing(10);
    QLabel *portLab = new QLabel(tr("Port"), portWidget);
    m_portLineEdit = new QLineEdit(portWidget);
    m_portLineEdit->setFixedSize(370, 36);
    m_portLineEdit->setPlaceholderText(tr("Please input port"));
    m_portLineEdit->setTextMargins(10, 0, 0, 0);
    portLayout->addWidget(portLab);
    portLayout->addWidget(m_portLineEdit);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(36);
    btnLayout->setMargin(0);
    QPushButton *confirmBtn = new QPushButton(tr("Confirm"));
    confirmBtn->setFixedSize(110, 36);
    Kiran::WidgetPropertyHelper::setButtonType(confirmBtn, Kiran::BUTTON_Default);
    QPushButton *cancleBtn = new QPushButton(tr("Cancle"));
    Kiran::WidgetPropertyHelper::setButtonType(cancleBtn, Kiran::BUTTON_Normal);
    cancleBtn->setFixedSize(110, 36);
    btnLayout->addWidget(confirmBtn);
    btnLayout->addWidget(cancleBtn);

    mainVLayout->addWidget(ipWidget);
    mainVLayout->addWidget(portWidget);
    mainVLayout->addStretch();
    mainVLayout->addLayout(btnLayout);

    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    this->move(screenGeometry.x() + (screenGeometry.width() - this->width()) / 2,
               screenGeometry.y() + (screenGeometry.height() - this->height()) / 2);

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
                UserConfiguration::getInstance().writeConfig(CONFIG_SETTING_TYPE_SERVER, SERVER_GROUP, SERVER_IP, m_ip);
                UserConfiguration::getInstance().writeConfig(CONFIG_SETTING_TYPE_SERVER, SERVER_GROUP, SERVER_PORT, m_port);
                UserConfiguration::getInstance().setServerAddr(QString("%1:%2").arg(m_ip).arg(m_port).toStdString());
                close();
            });
    connect(cancleBtn, &QPushButton::clicked,
            [this] {
                close();
            });
}

void ServerConfigDialog::setServerInfo()
{
    m_ip.clear();
    m_port.clear();

    QString server = QString::fromStdString(UserConfiguration::getInstance().getServerAddr());
    if (!server.isEmpty() && server.contains(":"))
    {
        m_ip = server.split(":").first();
        m_port = server.split(":").last();
        m_ipLineEdit->setText(m_ip);
        m_portLineEdit->setText(m_port);
        return;
    }
    UserConfiguration::getInstance().readConfig(CONFIG_SETTING_TYPE_SERVER, SERVER_GROUP, SERVER_IP, m_ip);
    m_ipLineEdit->setText(m_ip);
    UserConfiguration::getInstance().readConfig(CONFIG_SETTING_TYPE_SERVER, SERVER_GROUP, SERVER_PORT, m_port);
    m_portLineEdit->setText(m_port);
    UserConfiguration::getInstance().setServerAddr(QString("%1:%2").arg(m_ip).arg(m_port).toStdString());
}
