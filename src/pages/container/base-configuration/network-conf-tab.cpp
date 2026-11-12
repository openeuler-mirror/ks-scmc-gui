/**
 * @file          /ks-scmc-gui/src/pages/container/base-configuration/network-conf-tab.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "network-conf-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QRegExpValidator>
#include <QDoubleValidator>
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
// 创建容器、模板使用
void NetworkConfTab::getNetworkInfo(container::ContainerConfigs *cntrCfg)
{
    auto cfg = cntrCfg->add_networks();
    QString str = ui->cb_virt_networkcard->currentText();
    const QString networkSplitStr = " (" + tr("Subnet");
    QString name = str.split(networkSplitStr).first();  // 网卡名
    auto inputIp = ui->lineEdit_ip->getIpAddress();

    cfg->set_interface(name.toStdString());

    std::string ip = ui->lineEdit_ip->isValidIp(inputIp) ? inputIp.toStdString() : "";
    cfg->set_ip_address(ip);

    // 获取下行流量限制（下载速度）
    QString downloadSpeed = ui->download_speed_edit->text().trimmed();
    bool ok = false;
    double netRbw = downloadSpeed.toDouble(&ok);
    if (ok && netRbw > 0)
    {
        cfg->set_net_rbw(netRbw);
    } else {
        cfg->set_net_rbw(0);
    }

    // 获取上行流量限制（上传速度）
    QString uploadSpeed = ui->upload_speed_edit->text().trimmed();
    ok = false;
    double netWbw = uploadSpeed.toDouble(&ok);
    if (ok && netWbw > 0)
    {
        cfg->set_net_wbw(netWbw);
    } else {
        cfg->set_net_wbw(0);
    }

    KLOG_INFO() << "Set network interface to backed:" << name << ip.data() 
                << "net_rbw:" << netRbw << "net_wbw:" << netWbw;
}

// 更新容器、模板使用
void NetworkConfTab::getNetworkInfo(container::UpdateRequest *req)
{
    auto cfg = req->add_networks();
    QString str = ui->cb_virt_networkcard->currentText();
    const QString networkSplitStr = " (" + tr("Subnet");
    QString name = str.split(networkSplitStr).first();  // 网卡名
    auto inputIp = ui->lineEdit_ip->getIpAddress();

    cfg->set_interface(name.toStdString());

    std::string ip = ui->lineEdit_ip->isValidIp(inputIp) ? inputIp.toStdString() : "";
    cfg->set_ip_address(ip);

    KLOG_INFO() << "Set network interface to backed:" << name << ip.data();
}

// 更新容器,更新模板 时设置界面初始值
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

    KLOG_INFO() << "Set ip to ui:" << QString::fromStdString(networkCfg->ip_address().data());
    ui->lineEdit_ip->setIp(QString::fromStdString(networkCfg->ip_address().data()));
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

// 初始化网卡页面信息
void NetworkConfTab::initVirtNetworkInfo(QList<QString> networks)
{
    ui->cb_virt_networkcard->clear();
    ui->cb_virt_networkcard->addItems(QStringList(networks));
    ui->lineEdit_ip->clear();
}

void NetworkConfTab::initUI()
{
    ui->lineEdit_ip->setStyleSheet("QToolTip{"
                                   "background-color: rgb(255,255,255);"
                                   "color:#000000;"
                                   "border-radius: 6px;"
                                   "border:0px solid rgb(0,0,0);"
                                   "outline:none; "
                                   "min-height:30px;"
                                   "}");
}
