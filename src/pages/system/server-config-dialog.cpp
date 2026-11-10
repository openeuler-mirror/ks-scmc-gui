/**
 * @file          /ks-scmc-gui/src/pages/user/server-config-dialog.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "server-config-dialog.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <widget-property-helper.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "common/ip-mask-lineedit.h"
#include "ui_server-config-dialog.h"
#include "user-configuration.h"

enum ServerConfigType
{
    SERVER_CONFIG_TYPE_IP = 0,
    SERVER_CONFIG_TYPE_DOMAIN_NAME
};

ServerConfigDialog::ServerConfigDialog(QWidget *parent) : KiranTitlebarWindow(parent),
                                                          ui(new Ui::ServerConfigDialog)
{
    ui->setupUi(getWindowContentWidget());
    initUI();
    setServerInfo();
}

ServerConfigDialog::~ServerConfigDialog()
{
    delete ui;
}

QString ServerConfigDialog::getServerInfo()
{
    return QString::fromStdString(UserConfiguration::getInstance().getServerAddr());
}

void ServerConfigDialog::switchPage()
{
    QRadioButton *rbtn = qobject_cast<QRadioButton *>(sender());
    if (rbtn == ui->rbtn_ip)
    {
        ui->stackedWidget->setCurrentIndex(ServerConfigType::SERVER_CONFIG_TYPE_IP);
    }
    else if (rbtn == ui->rbtn_dn)
    {
        ui->stackedWidget->setCurrentIndex(ServerConfigType::SERVER_CONFIG_TYPE_DOMAIN_NAME);
    }
}

void ServerConfigDialog::initUI()
{
    setWindowModality(Qt::WindowModal);
    setTitle(tr("Config Server"));
    setIcon(QIcon(":/images/logo.png"));
    setButtonHints(TitlebarButtonHint::TitlebarCloseButtonHint);
    this->setFixedSize(520, 340);
    setResizeable(false);

    ui->rbtn_ip->setChecked(true);

    ui->lineEdit_dn->setPlaceholderText(tr("Please input domain name"));
    ui->lineEdit_dn->setTextMargins(10, 0, 0, 0);
    ui->lineEdit_dn->setMaxLength(255);

    ui->lineEdit_port->setPlaceholderText(tr("Please input port"));
    ui->lineEdit_port->setTextMargins(10, 0, 0, 0);
    ui->lineEdit_port->setMaxLength(5);
    ui->lineEdit_port->setValidator(new QIntValidator(0, 65535, this));

    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_confirm, Kiran::BUTTON_Default);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_cancle, Kiran::BUTTON_Normal);

    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    this->move(screenGeometry.x() + (screenGeometry.width() - this->width()) / 2,
               screenGeometry.y() + (screenGeometry.height() - this->height()) / 2);

    connect(ui->rbtn_ip, &QRadioButton::toggled, this, &ServerConfigDialog::switchPage);
    connect(ui->rbtn_dn, &QRadioButton::toggled, this, &ServerConfigDialog::switchPage);
    connect(ui->btn_confirm, &QPushButton::clicked, this, &ServerConfigDialog::confirm);
    connect(ui->btn_cancle, &QPushButton::clicked, this, &ServerConfigDialog::close);
}

bool ServerConfigDialog::handleIpConfig()
{
    auto inputIp = ui->lineEdit_ip->getIpAddress();
    if (inputIp.isEmpty() || ui->lineEdit_port->text().isEmpty())
    {
        KiranMessageBox::message(nullptr,
                                 tr("Failed"),
                                 tr("Please input complete server information!"),
                                 KiranMessageBox::Ok);
        return false;
    }
    else if (!ui->lineEdit_ip->isValidIp(inputIp))
    {
        KiranMessageBox::message(nullptr,
                                 tr("Failed"),
                                 tr("Please input correct IP address!"),
                                 KiranMessageBox::Ok);
        return false;
    }
    KLOG_INFO() << "Get ip and port from ui:" << inputIp << ui->lineEdit_port->text();

    m_ip = inputIp;
    m_port = ui->lineEdit_port->text();
    UserConfiguration::getInstance().writeConfig(CONFIG_SETTING_TYPE_SERVER, SERVER_GROUP, SERVER_IP, m_ip);
    UserConfiguration::getInstance().writeConfig(CONFIG_SETTING_TYPE_SERVER, SERVER_GROUP, SERVER_PORT, m_port);
    UserConfiguration::getInstance().setServerAddr(QString("%1:%2").arg(m_ip).arg(m_port).toStdString());
    return true;
}

bool ServerConfigDialog::handleDomainConfig()
{
    m_domain = ui->lineEdit_dn->text();
    m_port = ui->lineEdit_port->text();
    if (m_domain.isEmpty() || m_port.isEmpty())
    {
        KiranMessageBox::message(nullptr,
                                 tr("Failed"),
                                 tr("Please input complete server information!"),
                                 KiranMessageBox::Ok);
        return false;
    }
    KLOG_INFO() << "Get domain name and port from ui:" << m_domain << ui->lineEdit_port->text();

    UserConfiguration::getInstance().writeConfig(CONFIG_SETTING_TYPE_SERVER, SERVER_GROUP, SERVER_DOMAIN_NAME, m_domain);
    UserConfiguration::getInstance().writeConfig(CONFIG_SETTING_TYPE_SERVER, SERVER_GROUP, SERVER_PORT, m_port);
    UserConfiguration::getInstance().setServerAddr(QString("%1:%2").arg(m_domain).arg(m_port).toStdString());
    return true;
}

void ServerConfigDialog::confirm()
{
    bool isValid = false;
    switch (ui->stackedWidget->currentIndex())
    {
    case SERVER_CONFIG_TYPE_IP:
    {
        isValid = handleIpConfig();
        break;
    }
    case SERVER_CONFIG_TYPE_DOMAIN_NAME:
    {
        isValid = handleDomainConfig();
        break;
    }
    default:
    {
        break;
    }
    }
    if (isValid)
    {
        close();
    }
}

void ServerConfigDialog::setServerInfo()
{
    m_ip.clear();
    m_port.clear();
    m_domain.clear();

    // 设置ip/域名
    QString server = QString::fromStdString(UserConfiguration::getInstance().getServerAddr());
    if (!server.isEmpty() && server.contains(":"))
    {
        auto addr = server.split(":").first();
        // 设置ip/域名
        QRegExp ipRegex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
        if (ipRegex.exactMatch(addr))
        {
            m_ip = addr;
            ui->lineEdit_ip->setIp(m_ip);
            ui->rbtn_ip->setChecked(true);
        }
        else
        {
            m_domain = addr;
            ui->lineEdit_dn->setText(m_domain);
            ui->rbtn_dn->setChecked(true);
        }

        // 设置端口号
        m_port = server.split(":").last();
        ui->lineEdit_port->setText(m_port);
        return;
    }

    UserConfiguration::getInstance().readConfig(CONFIG_SETTING_TYPE_SERVER, SERVER_GROUP, SERVER_IP, m_ip);
    ui->lineEdit_ip->setIp(m_ip);
    UserConfiguration::getInstance().readConfig(CONFIG_SETTING_TYPE_SERVER, SERVER_GROUP, SERVER_DOMAIN_NAME, m_domain);
    ui->lineEdit_dn->setText(m_domain);
    UserConfiguration::getInstance().readConfig(CONFIG_SETTING_TYPE_SERVER, SERVER_GROUP, SERVER_PORT, m_port);
    ui->lineEdit_port->setText(m_port);

    auto addr = m_ip.isEmpty() ? m_domain : m_ip;
    if (!addr.isEmpty())
    {
        UserConfiguration::getInstance().setServerAddr(QString("%1:%2").arg(addr).arg(m_port).toStdString());
    }
}
