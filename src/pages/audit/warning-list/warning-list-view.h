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
    explicit WarningListView(QWidget *parent = nullptr,bool isShowContainerName = false,bool is_open_paging = false);
    ~WarningListView();
    void updateInfo(QString keyword = "");  //刷新表格
    void setLogListPageType(WarningListPageType);

private:
    void initTable();
    void initButtons();
    void initLogListConnect();
    void getWarningList(WarningListPageType type = CONTAINER_WARNING,int page_on = 1);
    void getReadWarn(QList<int64_t> ids);

private slots:
    void getListWarning(const QString objId, const QPair<grpc::Status, logging::ListWarnReply> &);
    //    void getReadWarning(const QPair<grpc::Status, logging::ReadWarnReply> &);
    void onBtnRead();
    void onBtnIgnore();
    void onBtnReadLabel(int row);
    void onBtnIgnoreLabel(int row);
    void updatePagingInfo(int page_on);

private:
    QString m_ObjId;
    WarningListPageType m_type;
    QMap<int, int64_t> m_idsMap;
    bool is_showContainerName;
    bool is_openPaging;
    int m_totalPages;
    int m_pageOn = 1;//当前页

signals:
    void sigOpenPaging(int);
    void sigUpdateWaringSums();//刷新告警总数
};

#endif  // LOGLISTVIEW_H
