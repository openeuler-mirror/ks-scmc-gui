/**
 * @file          /ks-scmc-gui/src/pages/node/node-info-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "node-info-page.h"
#include <kiran-log/qt5-log-i.h>
#include "IPC/ipc-page.h"
#include "config/node-config-page.h"
#include "container/container-manager/container-list-page.h"
#include "monitor-content.h"
#include "network/network-page-manager.h"
#include "security-configuration/file-protection-page.h"
#include "security-configuration/network-config-page.h"
NodeInfoPage::NodeInfoPage(QWidget *parent) : TabPage(parent), m_containerListPage(nullptr), m_monitor(nullptr)
{
    createSubPage(NODE_INFO_SUB_PAGE_TYPE_CONTAINER);
    createSubPage(NODE_INFO_SUB_PAGE_TYPE_MONITOR);
    createSubPage(NODE_INFO_SUB_PAGE_TYPE_NETWORK);
    createSubPage(NODE_INFO_SUB_PAGE_TYPE_CONFIG);
    createSubPage(NODE_INFO_SUB_PAGE_TYPE_NETWORK_CONFIG);
    createSubPage(NODE_INFO_SUB_PAGE_TYPE_FILE_PROTECT);
    connect(this, &NodeInfoPage::sigTabBarClicked, this, &NodeInfoPage::updatePageInfo);
}

void NodeInfoPage::setNodeId(qint64 nodeId)
{
    m_nodeId = nodeId;
    m_containerListPage->setNodeID(m_nodeId);
    m_networkPage->setNodeId(m_nodeId);
    m_configPage->setNodeId(m_nodeId);
}

void NodeInfoPage::updateInfo(QString keyword)
{
    setCurrentPage(NODE_INFO_SUB_PAGE_TYPE_CONTAINER);
    m_containerListPage->updateInfo();
}

void NodeInfoPage::createSubPage(NodeInfoSubPageType type)
{
    switch (type)
    {
    case NODE_INFO_SUB_PAGE_TYPE_CONTAINER:
    {
        m_containerListPage = new ContainerListPage(this);
        m_containerListPage->setOperateAreaVisible(false);
        addTabPage(m_containerListPage, tr("Container"));
        break;
    }
    case NODE_INFO_SUB_PAGE_TYPE_MONITOR:
    {
        m_monitor = new MonitorContent(this);
        addTabPage(m_monitor, tr("Monitor"));
        break;
    }
    case NODE_INFO_SUB_PAGE_TYPE_NETWORK:
    {
        m_networkPage = new NetworkPageManager(this);
        addTabPage(m_networkPage, tr("Network"));
        break;
    }
    case NODE_INFO_SUB_PAGE_TYPE_CONFIG:
    {
        m_configPage = new NodeConfigPage(this);
        addTabPage(m_configPage, tr("Config"));
        break;
    }
    case NODE_INFO_SUB_PAGE_TYPE_NETWORK_CONFIG:
    {
        m_networkConfigPage = new NetworkConfigPage(this);
        addTabPage(m_networkConfigPage, tr("Network Config"));
        break;
    }
    case NODE_INFO_SUB_PAGE_TYPE_FILE_PROTECT:
    {
        m_fileProtectionPage = new FileProtectionPage(this);
        addTabPage(m_fileProtectionPage, tr("File Protection"));
        break;
    }
    default:
        break;
    }
}

void NodeInfoPage::updatePageInfo(int index)
{
    if (index == NODE_INFO_SUB_PAGE_TYPE_CONTAINER)
    {
        m_containerListPage->updateInfo();
    }
    else if (index == NODE_INFO_SUB_PAGE_TYPE_MONITOR)
    {
        m_monitor->updateMonitorInfo(m_nodeId);
    }
    else if (index == NODE_INFO_SUB_PAGE_TYPE_NETWORK)
    {
        m_networkPage->updateInfo();
    }
    else if (index == NODE_INFO_SUB_PAGE_TYPE_CONFIG)
    {
        m_configPage->updateInfo();
    }
    else if (index == NODE_INFO_SUB_PAGE_TYPE_NETWORK_CONFIG)
    {
        m_networkConfigPage->updateInfo();
    }
    else if (index == NODE_INFO_SUB_PAGE_TYPE_FILE_PROTECT)
    {
        m_fileProtectionPage->updateInfo();
    }
}
