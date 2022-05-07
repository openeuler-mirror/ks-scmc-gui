#include "log-list-page.h"
#include <kiran-log/qt5-log-i.h>

LogListPage::LogListPage(QWidget *parent) : TabPage(parent)
  ,m_loglistview_container(nullptr)
  ,m_loglistview_system(nullptr)
  ,m_loglistview_users(nullptr)
{
    createSubPage(CONTAINER_LOGS);
    createSubPage(SYSTEM_LOGS);
    createSubPage(USERS_LOGS);
    connect(this, &LogListPage::sigTabBarClicked, this, &LogListPage::updatePageInfo);
}

void LogListPage::updateInfo(QString keyword)
{
    m_loglistview_container->updateInfo();
//    m_loglistview_system->updateInfo();
}

void LogListPage::createSubPage(LogListPageType type)
{
    switch (type)
    {
    case CONTAINER_LOGS:
    {
        m_loglistview_container = new LogListView;
        m_loglistview_container->setLogListPageType(CONTAINER_LOGS);
        addTabPage(m_loglistview_container,tr("container logs"));
        break;
    }
    case SYSTEM_LOGS:
    {
        m_loglistview_system = new LogListView;
        m_loglistview_system->setLogListPageType(SYSTEM_LOGS);
        addTabPage(m_loglistview_system,tr("system logs"));
        break;
    }
    case USERS_LOGS:
    {
        m_loglistview_users = new LogListView;
        m_loglistview_users->setLogListPageType(USERS_LOGS);
        addTabPage(m_loglistview_users,tr("users logs"));
        break;
    }
    default:
        break;
    }
}

void LogListPage::updatePageInfo(int index)
{
    if (index == CONTAINER_LOGS)
    {
        m_loglistview_container->updateInfo();
    }
    else if (index == SYSTEM_LOGS)
    {
        m_loglistview_system->updateInfo();
    }
    else if (index == USERS_LOGS)
    {
        m_loglistview_users->updateInfo();
    }
}
