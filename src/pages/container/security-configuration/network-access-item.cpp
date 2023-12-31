/**
 * @file          /ks-scmc-gui/src/pages/container/security-configuration/network-access-item.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "network-access-item.h"
#include <kiran-log/qt5-log-i.h>
#include "ui_network-access-item.h"
NetworkAccessItem::NetworkAccessItem(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::NetworkAccessItem)
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

void NetworkAccessItem::getInfo(QStringList &protocols, QString &addr, int &port)
{
    ui->btn_tcp->isChecked() ? protocols.append("tcp") : protocols.append("");
    ui->btn_udp->isChecked() ? protocols.append("udp") : protocols.append("");
    ui->btn_icmp->isChecked() ? protocols.append("icmp") : protocols.append("");
    protocols.removeAll("");

    addr = ui->lineEdit_address->text();
    port = ui->lineEdit_port->text().toInt();
}

void NetworkAccessItem::setInfo(QStringList protocols, QString addr, int port)
{
    KLOG_INFO() << protocols << addr << port;
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
    ui->btn_add->setCursor(Qt::PointingHandCursor);
    ui->btn_delete->setCursor(Qt::PointingHandCursor);
    ui->lineEdit_address->setPlaceholderText(tr("eg: 192.168.1.10 or 10.0.0.0/8"));
    ui->lineEdit_port->setText(QString::number(0));
    ui->lineEdit_port->setTextMargins(10, 0, 0, 0);
    ui->lineEdit_address->setTextMargins(10, 0, 0, 0);
    ui->btn_tcp->setChecked(true);
    connect(ui->btn_add, &QToolButton::clicked, this, &NetworkAccessItem::onAdd);
    connect(ui->btn_delete, &QToolButton::clicked, this, &NetworkAccessItem::ondelete);
}
