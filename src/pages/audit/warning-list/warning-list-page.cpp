/**
 * @file          /ks-scmc-gui/src/pages/audit/warning-list/warning-list-page.cpp
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "warning-list-page.h"
#include <kiran-log/qt5-log-i.h>

WarningListPage::WarningListPage(QWidget *parent) : TabPage(parent)
  ,m_warninglistview_container(nullptr)
  ,m_warninglistview_node(nullptr)
{
    createSubPage(CONTAINER_WARNING);
    createSubPage(NODE_WARNING);
    connect(this, &WarningListPage::sigTabBarClicked, this, &WarningListPage::updatePageInfo);
}

void WarningListPage::updateInfo(QString keyword)
{
    setCurrentPage(CONTAINER_WARNING);
    m_warninglistview_container->updateInfo();
//    m_loglistview_system->updateInfo();
}

void WarningListPage::createSubPage(WarningListPageType type)
{
    switch (type)
    {
    case CONTAINER_WARNING:
    {
        m_warninglistview_container = new WarningListView(this,true,true);
        m_warninglistview_container->setLogListPageType(CONTAINER_WARNING);
        connect(m_warninglistview_container,&WarningListView::sigUpdateWaringSums,this,&WarningListPage::sigReadedUpdateWaringSums);
        addTabPage(m_warninglistview_container,tr("container warning"));
        break;
    }
    case NODE_WARNING:
    {
        m_warninglistview_node = new WarningListView(this,false,true);
        m_warninglistview_node->setLogListPageType(NODE_WARNING);
        connect(m_warninglistview_node,&WarningListView::sigUpdateWaringSums,this,&WarningListPage::sigReadedUpdateWaringSums);
        addTabPage(m_warninglistview_node,tr("node warning"));
        break;
    }
    default:
        break;
    }
}

void WarningListPage::updatePageInfo(int index)
{
    if (index == CONTAINER_WARNING)
    {
        m_warninglistview_container->updateInfo();
    }
    else if (index == NODE_WARNING)
    {
        m_warninglistview_node->updateInfo();
    }
}
