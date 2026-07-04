#ifndef WARNING_LIST_VIEW_H
#define WARNING_LIST_VIEW_H

#include "info-worker.h"
#include "pages/container/container-manager/container-list-page.h"
#include "pages/table-page.h"

enum WarningListPageType
{
    CONTAINER_WARNING,
    NODE_WARNING
};

class WarningListView : public TablePage
{
    Q_OBJECT
public:
    explicit WarningListView(QWidget *parent = nullptr);
    ~WarningListView();
    void updateInfo(QString keyword = "");  //刷新表格
    void setLogListPageType(WarningListPageType);

private:
    void initTable();
    void initButtons();
    void initLogListConnect();
    void getWarningList(WarningListPageType);
    void getReadWarn(int64_t ids);

private slots:
    void getListWarning(const QPair<grpc::Status, logging::ListWarnReply> &);
//    void getReadWarning(const QPair<grpc::Status, logging::ReadWarnReply> &);
    void onBtnRead();
    void onBtnIgnore();
    void onBtnReadLabel(int row);
    void onBtnIgnoreLabel(int row);

private:
    WarningListPageType m_type;
    QMap<int, int64_t> m_idsMap;
};

#endif  // LOGLISTVIEW_H
