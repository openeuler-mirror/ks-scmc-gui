#include "network-conf-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QRegExpValidator>
#include "ui_network-conf-tab.h"
NetworkConfTab::NetworkConfTab(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::NetworkConfTab)
{
    ui->setupUi(this);
    initUI();
    getNetworkInfo();
    connect(&InfoWorker::getInstance(), &InfoWorker::listNetworkFinished, this, &NetworkConfTab::getNetworkListResult);
}

NetworkConfTab::~NetworkConfTab()
{
    delete ui;
}

void NetworkConfTab::getNetworkInfo(container::CreateRequest *req)
{
    auto cfg = req->add_network_config();
    QString str = ui->cb_virt_networkcard->currentText();
    QString name = str.split(" ").first();  // 网卡名
    container::NetworkConfig network;

    cfg->set_interface(name.toStdString());
    cfg->set_ip_address(ui->lineEdit_ip->text().toStdString());
    cfg->set_mac_address(ui->lineEdit_mac->text().toStdString());
}

void NetworkConfTab::setNetworkInfo(container::NetworkConfig *networkCfg)
{
    KLOG_INFO() << "setNetworkInfo: " << networkCfg->ip_address().data() << networkCfg->mac_address().data();
    ui->lineEdit_ip->setText(QString::fromStdString(networkCfg->ip_address().data()));
    ui->lineEdit_mac->setText(QString::fromStdString(networkCfg->mac_address().data()));
}

void NetworkConfTab::updateNetworkInfo(int64_t node_id)
{
    KLOG_INFO() << node_id;
    InfoWorker::getInstance().listNetwork(node_id);
}

void NetworkConfTab::getNetworkListResult(const QPair<grpc::Status, network::ListReply> &reply)
{
    KLOG_INFO() << "getNetworkListResult";
    if (reply.first.ok())
    {
        ui->cb_virt_networkcard->clear();
        for (auto ifs : reply.second.virtual_ifs())
        {
            KLOG_INFO() << ifs.name().data() << ifs.ip_address().data() << ifs.ip_mask_len();
            auto name = ifs.name();
            auto subnet = ifs.ip_address() + "/" + std::to_string(ifs.ip_mask_len());
            QString str = QString("%1 (%2:%3)")
                              .arg(QString::fromStdString(name))
                              .arg(tr("Subnet"))
                              .arg(QString::fromStdString(subnet));
            KLOG_INFO() << str;
            ui->cb_virt_networkcard->addItem(str);
        }
        //        QString gatewaykStr;
        //        QString tips;
        //        for (auto bridge : reply.second.bridge_if())
        //        {
        //            auto name = bridge.name();
        //            auto gateway = bridge.gateway();
        //            KLOG_INFO() << QString::fromStdString(name) << QString::fromStdString(gateway);

        //            if (!gateway.empty())
        //            {
        //                gatewaykStr = QString("%1 (%2:%3)")
        //                                  .arg(QString::fromStdString(name))
        //                                  .arg(tr("Gateway"))
        //                                  .arg(QString::fromStdString(gateway));
        //                KLOG_INFO() << "gatewaykStr: " << gatewaykStr;
        //                ui->cb_virt_networkcard->addItem(gatewaykStr);
        //            }
        //            else
        //            {
        //                gatewaykStr = QString("%1 (%2:/)")
        //                                  .arg(QString::fromStdString(name))
        //                                  .arg(tr("Gateway"));
        //                ui->cb_virt_networkcard->addItem(gatewaykStr);
        //            }

        //            if (bridge.has_ip_range())
        //            {  // ip_range优先
        //                auto addr = bridge.ip_range().addr();
        //                auto prefix_len = bridge.ip_range().prefix_len();
        //                tips = QString("Available network segment: %1/%2").arg(QString::fromStdString(addr)).arg(prefix_len);
        //                ui->lineEdit_ip->setToolTip(tips);
        //            }
        //            else if (bridge.has_subnet())
        //            {  // 没有ip_range 使用subnet
        //                auto addr = bridge.subnet().addr();
        //                auto prefix_len = bridge.subnet().prefix_len();
        //                tips = QString("Available network segment: %1/%2").arg(QString::fromStdString(addr)).arg(prefix_len);
        //                ui->lineEdit_ip->setToolTip(tips);
        //            }
        //        }
    }
}

void NetworkConfTab::initUI()
{
    ui->lineEdit_ip->setPlaceholderText(tr("Default auto-assignment when not config"));
    ui->lineEdit_ip->setStyleSheet("QToolTip{"
                                   "background-color: rgb(255,255,255);"
                                   "color:#000000;"
                                   "border-radius: 6px;"
                                   "border:0px solid rgb(0,0,0);"
                                   "outline:none; "
                                   "min-height:30px;"
                                   "}");
    QRegExp rx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    ui->lineEdit_ip->setValidator(new QRegExpValidator(rx));

    QRegExp rxMac("([0-9a-fA-F]{2})((:[0-9a-fA-F]{2}){5})");
    ui->lineEdit_mac->setValidator(new QRegExpValidator(rxMac));
    ui->lineEdit_mac->setPlaceholderText(tr("Default auto-assignment when not config"));
}

void NetworkConfTab::getNetworkInfo()
{
}
