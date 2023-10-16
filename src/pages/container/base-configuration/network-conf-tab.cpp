/**
 * @file          /ks-scmc-gui/src/pages/container/base-configuration/network-conf-tab.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "network-conf-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QRegExpValidator>
#include "ui_network-conf-tab.h"
NetworkConfTab::NetworkConfTab(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::NetworkConfTab),
                                                  m_virtNetwork("")
{
    ui->setupUi(this);
    initUI();
}

NetworkConfTab::~NetworkConfTab()
{
    delete ui;
}
//创建容器、模板使用
void NetworkConfTab::getNetworkInfo(container::ContainerConfigs *cntrCfg)
{
    auto cfg = cntrCfg->add_networks();
    QString str = ui->cb_virt_networkcard->currentText();
    const QString networkSplitStr = " (" + tr("Subnet");
    QString name = str.split(networkSplitStr).first();  // 网卡名

    KLOG_DEBUG() << "Network interface ip:" << name << ui->lineEdit_ip->text();
    cfg->set_interface(name.toStdString());
    cfg->set_ip_address(ui->lineEdit_ip->text().toStdString());
}

//更新容器、模板使用
void NetworkConfTab::getNetworkInfo(container::UpdateRequest *req)
{
    auto cfg = req->add_networks();
    QString str = ui->cb_virt_networkcard->currentText();
    const QString networkSplitStr = " (" + tr("Subnet");
    QString name = str.split(networkSplitStr).first();  // 网卡名

    KLOG_DEBUG() << "Network interface ip:" << name << ui->lineEdit_ip->text();
    cfg->set_interface(name.toStdString());
    cfg->set_ip_address(ui->lineEdit_ip->text().toStdString());
}

//更新容器,更新模板 时设置界面初始值
void NetworkConfTab::setNetworkInfo(container::NetworkConfig *networkCfg, QList<QString> networkList)
{
    KLOG_DEBUG() << "Set network info:"
                 << "name:" << networkCfg->interface().data()
                 << "ip:" << networkCfg->ip_address().data()
                 << "mac:" << networkCfg->mac_address().data();
    KLOG_DEBUG() << "All network list:" << networkList;
    auto name = networkCfg->interface().data();

    ui->cb_virt_networkcard->clear();
    ui->cb_virt_networkcard->addItems(QStringList(networkList));
    setVirtNetwork(name);

    ui->lineEdit_ip->setText(QString::fromStdString(networkCfg->ip_address().data()));
}

void NetworkConfTab::setVirtNetwork(QString virtNetwork)
{
    if (!virtNetwork.isEmpty())
    {
        int count = ui->cb_virt_networkcard->count();
        const QString networkSplitStr = " (" + tr("Subnet");
        for (int i = 0; i < count; ++i)
        {
            QString text = ui->cb_virt_networkcard->itemText(i);
            // 网卡名可以以" ("结尾
            if (text.split(networkSplitStr).first() == QString(virtNetwork))
            {
                ui->cb_virt_networkcard->setCurrentIndex(i);
                KLOG_DEBUG() << "Found virt network:" << virtNetwork;
                break;
            }
        }
    }
    else
        ui->cb_virt_networkcard->setCurrentIndex(0);
}

//初始化网卡页面信息
void NetworkConfTab::initVirtNetworkInfo(QList<QString> networks)
{
    ui->cb_virt_networkcard->clear();
    ui->cb_virt_networkcard->addItems(QStringList(networks));
    ui->lineEdit_ip->clear();
}

void NetworkConfTab::initUI()
{
    ui->lineEdit_ip->setTextMargins(10, 0, 0, 0);
    ui->lineEdit_ip->setPlaceholderText(tr("Default auto-assignment when not config"));
    ui->lineEdit_ip->setStyleSheet("QToolTip{"
                                   "background-color: rgb(255,255,255);"
                                   "color:#000000;"
                                   "border-radius: 6px;"
                                   "border:0px solid rgb(0,0,0);"
                                   "outline:none; "
                                   "min-height:30px;"
                                   "}");
    QRegExp rx("^[0-9.]+$");
    ui->lineEdit_ip->setValidator(new QRegExpValidator(rx, this));
}
