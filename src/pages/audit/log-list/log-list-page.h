#ifndef LOGLISTPAGE_H
#define LOGLISTPAGE_H

#include "pages/tab-page.h"
#include "log-list-view.h"

enum LogListPageType
{
    CONTAINER_LOGS,
    SYSTEM_LOGS,
    USERS_REFUSED
};

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
    LogListView *m_loglistview;
};




#endif // LOGLISTPAGE_H
