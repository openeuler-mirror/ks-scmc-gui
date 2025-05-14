/**
 * @file          src/pages/node/security-configuration/network-access-ctl-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "network-config-page.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QButtonGroup>
#include "common/message-dialog.h"
#include "common/notification-manager.h"
#include "common/security-widgets/network-access-list.h"
#include "common/security-widgets/security-list.h"
#include "ui_network-config-page.h"
NetworkConfigPage::NetworkConfigPage(QWidget* parent) : Page(parent),
                                                        ui(new Ui::NetworkConfigPage),
                                                        m_nodeID(-1),
                                                        m_accessCtrlEnabled(false),
                                                        m_whiteListEnabled(false),
                                                        m_accessList(nullptr),
                                                        m_processList(nullptr)
{
    ui->setupUi(this);
    m_objId = Node::generateId(this);
    initUI();
    initConnect();
}

void NetworkConfigPage::updateInfo(QString keyword)
{
    m_accessList->clearItems();
    m_processList->clearItems();
    Node::getInstance().getNetworkRule(m_objId, m_nodeID);
}

void NetworkConfigPage::setNodeId(qint64 nodeID)
{
    m_nodeID = nodeID;
    getDefaultPorts();
}

void NetworkConfigPage::setConfigEnabled(QAbstractButton* btn)
{
    auto radioBtn = qobject_cast<QRadioButton*>(btn);
    if (!radioBtn)
        return;

    struct ButtonAction
    {
        QRadioButton* button;
        bool& enabledFlag;
        bool isEnabled;
    };

    static const std::vector<ButtonAction> actions = {
        {ui->btn_access_close, m_accessCtrlEnabled, false},
        {ui->btn_access_open, m_accessCtrlEnabled, true},
        {ui->btn_whitelist_close, m_whiteListEnabled, false},
        {ui->btn_whitelist_open, m_whiteListEnabled, true}};

    for (const auto& action : actions)
    {
        if (radioBtn != action.button)
            continue;

        if (radioBtn == ui->btn_access_close || radioBtn == ui->btn_access_open)
        {
            m_accessList->setItemsEnable(action.isEnabled);
        }
        else if (radioBtn == ui->btn_whitelist_close || radioBtn == ui->btn_whitelist_open)
        {
            m_processList->setItemsEnable(action.isEnabled);
        }
        action.enabledFlag = action.isEnabled;
        break;
    }
}

void NetworkConfigPage::getNetworkRuleFinished(const QString objId, const QPair<grpc::Status, node::GetNetworkRuleReply>& reply)
{
    if (m_objId != objId)
        return;

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Failed to get node:" << m_nodeID << "Network rule list:" << reply.first.error_message().data();
        return;
    }

    if (!reply.second.has_network_rule())
    {
        KLOG_WARNING() << "Failed to get node:" << m_nodeID << "Network rule list:"
                       << "network_rule not found";
        return;
    }

    auto networkRule = reply.second.network_rule();
    bool isOn = networkRule.is_on();
    auto btn = isOn ? ui->btn_access_open : ui->btn_access_close;
    btn->setChecked(true);

    QList<NetworkAccessList::NetworkAccessInfo> infos;
    for (auto rule : networkRule.rules())
    {
        NetworkAccessList::NetworkAccessInfo info;

        QStringList protocols;
        for (std::string protocol : rule.protocols())
        {
            QString strProtocol = QString::fromStdString(protocol);
            if (0 == strProtocol.compare("tcp", Qt::CaseInsensitive) ||
                0 == strProtocol.compare("udp", Qt::CaseInsensitive) ||
                0 == strProtocol.compare("icmp", Qt::CaseInsensitive))

            {
                protocols.append(strProtocol);
            }
        }
        QString addr = QString::fromStdString(rule.addr().data());
        int port = rule.port();

        info.protocols = protocols;
        info.addr = addr;
        info.port = port;
        info.defaultPort = m_defaultPorts.contains(port);

        infos.append(info);
        KLOG_DEBUG() << "Get node:" << m_nodeID << "network rule from backend: "
                     << "enable:" << isOn << "rule:" << protocols << addr << port;
    }
    m_accessList->setNeteworkInfos(infos);
    setConfigEnabled(btn);
}

void NetworkConfigPage::updateNetworkRuleFinished(const QString objId, const QPair<grpc::Status, node::UpdateNetworkRuleReply>& reply)
{
    if (m_objId != objId)
        return;

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Failed to update node:" << m_nodeID << "network rule:" << reply.first.error_message().data();
        return;
    }

    NotificationManager::sendNotify(tr("Successful to update node network rule!"), "");
}

void NetworkConfigPage::save()
{
    // 保存网络访问控制信息
    node::UpdateNetworkRuleRequest req;
    req.set_node_id(m_nodeID);

    auto securityCfg = req.mutable_security_config();

    auto networkRule = securityCfg->mutable_network_rule();
    networkRule->set_is_on(m_accessCtrlEnabled);

    auto infos = m_accessList->getNeteworkInfos();
    for (auto info : infos)
    {
        auto rules = networkRule->add_rules();
        foreach (auto protocol, info.protocols)
        {
            rules->add_protocols(protocol.toStdString());
        }
        rules->set_addr(info.addr.toStdString());

        if (m_defaultPorts.contains(info.port))
        {
            auto ret = MessageDialog::message(tr("Update Node Network Rules"),
                                              tr("Failed to update node network rules"),
                                              tr("The port %1 is in use, please input again!").arg(info.port),
                                              ":/images/error.svg",
                                              MessageDialog::StandardButton::Yes);
            return;
        }
        else
        {
            rules->set_port(info.port);
        }

        KLOG_DEBUG() << "Set node:" << m_nodeID << "network rule to backend: "
                     << "enable:" << m_accessCtrlEnabled << "rule:" << info.protocols << info.addr << info.port;
    }

    Node::getInstance().updateNetworkRule(m_objId, req);
}

void NetworkConfigPage::initUI()
{
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_save, Kiran::BUTTON_Default);

    m_accessList = new NetworkAccessList(this);
    ui->layout_access_control->addWidget(m_accessList);

    m_processList = new SecurityList(tr("Network process"), this);
    ui->layout_process->addWidget(m_processList);

    auto accessBtnGroup = new QButtonGroup(this);
    accessBtnGroup->addButton(ui->btn_access_close);
    accessBtnGroup->addButton(ui->btn_access_open);

    auto whiteListBtnGroup = new QButtonGroup(this);
    whiteListBtnGroup->addButton(ui->btn_whitelist_open);
    whiteListBtnGroup->addButton(ui->btn_whitelist_close);

    connect(ui->btn_save, &QPushButton::clicked, this, &NetworkConfigPage::save);
    connect(accessBtnGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this, &NetworkConfigPage::setConfigEnabled);
    connect(whiteListBtnGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this, &NetworkConfigPage::setConfigEnabled);

    ui->btn_access_close->click();
    ui->btn_whitelist_close->click();
}

void NetworkConfigPage::initConnect()
{
    connect(&Node::getInstance(), &Node::getNetworkRuleFinished, this, &NetworkConfigPage::getNetworkRuleFinished);
    connect(&Node::getInstance(), &Node::updateNetworkRuleFinished, this, &NetworkConfigPage::updateNetworkRuleFinished);
}

void NetworkConfigPage::getDefaultPorts()
{
    auto reply = Node::getInstance().getNodeDefaultPort(m_nodeID);
    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Failed to get node:" << m_nodeID << "default ports:" << reply.first.error_message().data();
        return;
    }

    auto blockedPorts = reply.second.blocked_ports();
    for (auto port : blockedPorts)
    {
        m_defaultPorts.append(port.port());
    }
    KLOG_DEBUG() << "Get node:" << m_nodeID << "default port from backend: " << m_defaultPorts;
}
