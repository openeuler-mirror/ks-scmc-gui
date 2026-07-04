#include "node-info-page.h"
#include "container/container-list-page.h"
#include "monitor-content.h"
NodeInfoPage::NodeInfoPage(QWidget *parent) : TabPage(parent), m_containerListPage(nullptr), m_monitor(nullptr)
{
    createSubPage(NODE_INFO_SUB_PAGE_TYPE_CONTAINER);
    createSubPage(NODE_INFO_SUB_PAGE_TYPE_MONITOR);
}

void NodeInfoPage::setNodeId(qint64 nodeId)
{
    m_nodeId = nodeId;
}

void NodeInfoPage::updateInfo(QString keyword)
{
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
        m_monitor = new MonitorContent(-1, "", this);
        addTabPage(m_monitor, tr("Monitor"));
        break;
    }
    default:
        break;
    }
}
