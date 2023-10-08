#ifndef LOGLISTVIEW_H
#define LOGLISTVIEW_H

#include "pages/table-page.h"
#include "info-worker.h"
#include "pages/container/container-list-page.h"
#include "common/date-picker/date-picker.h"
#include "common/date-picker/date-pick-button.h"
#include "common/monitor-content.h"

enum LogListPageType
{
    CONTAINER_LOGS,
    SYSTEM_LOGS,
    USERS_LOGS
};

class LogListView : public TablePage
{
    Q_OBJECT
public:
    explicit LogListView(QWidget *parent = nullptr);
    ~LogListView();
    void updateInfo(QString keyword = "");  //刷新表格
    void setLogListPageType(LogListPageType);

private:
    void initTable();
    void initButtons();
    void initLogListConnect();
    void getLogList(LogListPageType);

private slots:
    void onDateSelection();
    void getListRuntime(const QPair<grpc::Status,logging::ListRuntimeReply> &);
    void popupStartDatePicker();
    void popupEndDatePicker();
    void applyDatePicker();

private:
    DatePicker *m_datePicker;
    DatePickButton *m_datePickStart;
    DatePickButton *m_datePickEnd;
    QPushButton *m_BtnApply;
    QDateTime m_xStart;
    QDateTime m_xEnd;
    LogListPageType m_type;
};

#endif // LOGLISTVIEW_H
