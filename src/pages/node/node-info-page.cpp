/**
 * @file          /ks-scmc-gui/src/pages/node/node-info-page.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "node-info-page.h"
#include <kiran-log/qt5-log-i.h>
#include "container/container-manager/container-list-page.h"
#include "monitor-content.h"
NodeInfoPage::NodeInfoPage(QWidget *parent) : TabPage(parent), m_containerListPage(nullptr), m_monitor(nullptr)
{
    createSubPage(NODE_INFO_SUB_PAGE_TYPE_CONTAINER);
    createSubPage(NODE_INFO_SUB_PAGE_TYPE_MONITOR);
    connect(this, &NodeInfoPage::sigTabBarClicked, this, &NodeInfoPage::updatePageInfo);
}

void NodeInfoPage::setNodeId(qint64 nodeId)
{
    m_nodeId = nodeId;
}

void NodeInfoPage::updateInfo(QString keyword)
{
    KLOG_INFO() << "NodeInfoPage UpdateInfo" << m_nodeId;
    m_monitor->updateMonitorInfo(m_nodeId);
    m_containerListPage->getContainerList(m_nodeId);
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
    default:
        break;
    }
}

void NodeInfoPage::updatePageInfo(int index)
{
    if (index == NODE_INFO_SUB_PAGE_TYPE_CONTAINER)
    {
        m_containerListPage->getContainerList(m_nodeId);
    }
    else if (index == NODE_INFO_SUB_PAGE_TYPE_MONITOR)
        m_monitor->updateMonitorInfo(m_nodeId);
}
