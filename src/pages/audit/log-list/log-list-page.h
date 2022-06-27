/**
 * @file          /ks-scmc-gui/src/pages/audit/log-list/log-list-page.h
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef LOGLISTPAGE_H
#define LOGLISTPAGE_H

#include "pages/tab-page.h"
#include "log-list-view.h"

class LogListView;
class LogListPage  : public TabPage
{
    Q_OBJECT
public:
    explicit LogListPage(QWidget *parent = nullptr);
    void updateInfo(QString keyword = "");  //刷新表格
private:
    void initUI();
    void createSubPage(LogListPageType type);
    void updatePageInfo(int index);

private:
    LogListView *m_loglistview_container;
    LogListView *m_loglistview_system;
    LogListView *m_loglistview_users;
};




#endif // LOGLISTPAGE_H
