#include "node-page-manager.h"
#include "node-info-page.h"
#include "node-list-page.h"
#include "page.h"
#include "table-page.h"
NodePageManager::NodePageManager(QWidget *parent) : StackedPage(parent), m_nodeInfoPage(nullptr)
{
    createStackedPage(NODE_PAGE_TYPE_NODE_LIST, "Node List");
    createStackedPage(NODE_PAGE_TYPE_NODE_INFO, "Node Info");
}

void NodePageManager::createStackedPage(NodePageType type, QString key)
{
    Page *page = nullptr;
    switch (type)
    {
    case NODE_PAGE_TYPE_NODE_LIST:
    {
        NodeListPage *nodeListPage = new NodeListPage(this);
        connect(nodeListPage, &NodeListPage::sigNodeNameClicked, this, &NodePageManager::onNodeNameClicked);
        page = nodeListPage;
        break;
    }
    case NODE_PAGE_TYPE_NODE_INFO:
    {
        m_nodeInfoPage = new NodeInfoPage(this);
        page = m_nodeInfoPage;
    }
    default:
        break;
    }

    addStackedPage(page, key);
}

void NodePageManager::updateInfo(QString keyword)
{
    setCurrentPage(NODE_PAGE_TYPE_NODE_LIST);
    updateCurrentPage();
}

void NodePageManager::onNodeNameClicked(qint64 nodeId)
{
    setCurrentPage(NODE_PAGE_TYPE_NODE_INFO);
    m_nodeInfoPage->setNodeId(nodeId);
    updateCurrentPage();
}
