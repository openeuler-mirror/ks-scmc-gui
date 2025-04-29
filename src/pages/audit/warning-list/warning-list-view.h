/**
 * @file          /ks-scmc-gui/src/pages/audit/warning-list/warning-list-view.h
 * @brief
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef WARNING_LIST_VIEW_H
#define WARNING_LIST_VIEW_H

#include <QTimer>
#include "base/interface/logging.h"
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
    explicit WarningListView(QWidget *parent = nullptr, bool isShowContainerName = false, bool is_open_paging = false);
    ~WarningListView();
    void updateInfo(QString keyword = "");  // 刷新表格
    void setLogListPageType(WarningListPageType);

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    void initTable();
    void initButtons();
    void initLogListConnect();
    void getWarningList(WarningListPageType type = CONTAINER_WARNING, int page_on = 1);
    void readWarn(QList<int64_t> ids);
    void refresh();
    void getCheckedItemsId(QList<qint64> &ids);

private slots:
    void getListWarningResult(const QString objId, const QPair<grpc::Status, logging::ListWarnReply> &);
    void getReadWarningResult(const QString objId, const QPair<grpc::Status, logging::ReadWarnReply> &);
    void onBtnRead();
    void onBtnReadLabel(int row);
    void updatePagingInfo(int page_on);

private:
    QString m_ObjId;
    WarningListPageType m_type;
    QMap<int, int64_t> m_idsMap;
    bool is_showContainerName;
    bool is_openPaging;
    int m_totalPages;
    int64_t m_totalWarnning;
    int m_pageOn = 1;  // 当前页
    QTimer *m_timer;

signals:
    void sigUpdateWaringSums(int64_t);  // 刷新告警总数
};

#endif  // LOGLISTVIEW_H
