/**
 * @file          /ks-scmc-gui/src/pages/audit/warning-list/warning-list-page.h
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef WARNING_LIST_PAGE_H
#define WARNING_LIST_PAGE_H

#include "pages/tab-page.h"
#include "warning-list-view.h"

class WarningListView;
class WarningListPage  : public TabPage
{
    Q_OBJECT
public:
    explicit WarningListPage(QWidget *parent = nullptr);
    void updateInfo(QString keyword = "");  //刷新表格
private:
    void initUI();
    void createSubPage(WarningListPageType type);
    void updatePageInfo(int index);

private:
    WarningListView *m_warninglistview_container;
    WarningListView *m_warninglistview_node;

signals:
    void sigReadedUpdateWaringSums();
};




#endif // LOGLISTPAGE_H
