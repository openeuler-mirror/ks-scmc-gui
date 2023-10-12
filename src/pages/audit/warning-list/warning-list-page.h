#ifndef WARNING_LIST_PAGE_H
#define WARNING_LIST_PAGE_H

#include "pages/tab-page.h"
#include "warning-list-view.h"

class WaringListView;
class WaringListPage  : public TabPage
{
    Q_OBJECT
public:
    explicit WaringListPage(QWidget *parent = nullptr);
    void updateInfo(QString keyword = "");  //刷新表格
private:
    void initUI();
    void createSubPage(WarningListPageType type);
    void updatePageInfo(int index);

private:
    WaringListView *m_warninglistview_container;
    WaringListView *m_warninglistview_node;
};




#endif // LOGLISTPAGE_H
