/**
 * @file          /ks-scmc-gui/src/pages/container/security-configuration/network-access-item.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "network-access-item.h"
#include <kiran-log/qt5-log-i.h>
#include <QRegExpValidator>
#include "common/ip-mask-lineedit.h"
#include "ui_network-access-item.h"

NetworkAccessItem::NetworkAccessItem(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::NetworkAccessItem),
                                                        m_ipEditor(nullptr),
                                                        m_defaultPort(false)
{
    ui->setupUi(this);
    initUI();
}

NetworkAccessItem::~NetworkAccessItem()
{
    delete ui;
}

void NetworkAccessItem::setDeleteBtnVisible(bool visible)
{
    ui->btn_delete->setVisible(visible);
}

void NetworkAccessItem::setDefaultPort(bool defaultPort)
{
    m_defaultPort = defaultPort;
}

bool NetworkAccessItem::isDefaultPort()
{
    return m_defaultPort;
}

void NetworkAccessItem::getInfo(QStringList &protocols, QString &addr, int &port)
{
    ui->btn_tcp->isChecked() ? protocols.append("tcp") : protocols.append("");
    ui->btn_udp->isChecked() ? protocols.append("udp") : protocols.append("");
    ui->btn_icmp->isChecked() ? protocols.append("icmp") : protocols.append("");
    protocols.removeAll("");

    auto inputIpMask = m_ipEditor->getIpMask();
    // 处理最后的"/",若用户没有输入掩码，则去除“/”。
    if (inputIpMask.endsWith("/"))
    {
        inputIpMask = inputIpMask.remove("/");
        addr = m_ipEditor->isValidIp(inputIpMask) ? inputIpMask : "";
    }
    else
    {
        addr = m_ipEditor->isValidIpMask(inputIpMask) ? inputIpMask : "";
    }

    port = ui->lineEdit_port->text().toInt();

    KLOG_INFO() << "Get info from ui."
                << "ip mask:" << addr
                << "port: " << port;
}

void NetworkAccessItem::setInfo(QStringList protocols, QString addr, int port)
{
    KLOG_DEBUG() << "Set network access item info:" << protocols << addr << port;
    protocols.contains("tcp", Qt::CaseInsensitive) ? ui->btn_tcp->setChecked(true) : ui->btn_tcp->setChecked(false);
    protocols.contains("udp", Qt::CaseInsensitive) ? ui->btn_udp->setChecked(true) : ui->btn_udp->setChecked(false);
    protocols.contains("icmp", Qt::CaseInsensitive) ? ui->btn_icmp->setChecked(true) : ui->btn_icmp->setChecked(false);

    ui->lineEdit_address->setText(addr);
    ui->lineEdit_port->setText(QString::number(port));
}

void NetworkAccessItem::ondelete()
{
    emit sigDelete();
}

void NetworkAccessItem::onAdd()
{
    emit sigAdd();
}

void NetworkAccessItem::initUI()
{
    ui->btn_add->setIcon(QIcon(":/images/icon-add.svg"));
    ui->btn_delete->setIcon(QIcon(":/images/icon-delete.svg"));
    ui->btn_add->setCursor(Qt::PointingHandCursor);
    ui->btn_delete->setCursor(Qt::PointingHandCursor);
    ui->lineEdit_address->setPlaceholderText(tr("eg: 192.168.1.10 or 10.0.0.0/8"));
    ui->lineEdit_address->setTextMargins(10, 0, 0, 0);
    //  匹配 IP 地址和可选的子网掩码
    QRegExp ipMaskRegex(
        "^"
        "(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
        "(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
        "(?:(?:/(?:3[0-2]|[12][0-9]|[0-9]))?)"
        "$");
    ui->lineEdit_address->setValidator(new QRegExpValidator(ipMaskRegex, this));
    ui->lineEdit_address->setMaxLength(20);

    ui->lineEdit_port->setText(QString::number(0));
    ui->lineEdit_port->setTextMargins(10, 0, 0, 0);
    ui->lineEdit_port->setMaxLength(5);
    ui->lineEdit_port->setValidator(new QIntValidator(0, 65535, this));

    connect(ui->btn_add, &QToolButton::clicked, this, &NetworkAccessItem::onAdd);
    connect(ui->btn_delete, &QToolButton::clicked, this, &NetworkAccessItem::ondelete);
}
