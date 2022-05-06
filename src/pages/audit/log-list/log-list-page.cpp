#include "log-list-page.h"
#include <kiran-log/qt5-log-i.h>

LogListPage::LogListPage(QWidget *parent) : TabPage(parent) ,m_loglistview(nullptr)
{
    createSubPage(CONTAINER_LOGS);
    createSubPage(SYSTEM_LOGS);
    createSubPage(USERS_REFUSED);
    connect(this, &LogListPage::sigTabBarClicked, this, &LogListPage::updatePageInfo);
}

void LogListPage::updateInfo(QString keyword)
{
    m_loglistview->updateInfo();
}

void LogListPage::createSubPage(LogListPageType type)
{
    switch (type)
    {
    case CONTAINER_LOGS:
    {
        m_loglistview = new LogListView;
        addTabPage(m_loglistview,tr("container logs"));
        break;
    }
    case SYSTEM_LOGS:
    {
        m_loglistview = new LogListView;
        addTabPage(m_loglistview,tr("system logs"));
        break;
    }
    case USERS_REFUSED:
    {
        m_loglistview = new LogListView;
        addTabPage(m_loglistview,tr("users logs"));
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
        m_loglistview->updateInfo();
    }
    else if (index == SYSTEM_LOGS)
    {

    }
    else if (index == USERS_REFUSED)
    {

    }
}
