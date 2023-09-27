#ifndef LOGLISTVIEW_H
#define LOGLISTVIEW_H

#include "pages/table-page.h"
#include "info-worker.h"
#include "pages/container/container-list-page.h"
#include "common/date-picker/date-picker.h"
#include "common/date-picker/date-pick-button.h"
#include "common/monitor-content.h"

class LogListView : public TablePage
{
    Q_OBJECT
public:
    explicit LogListView(QWidget *parent = nullptr);
    ~LogListView();
    void updateInfo(QString keyword = "");  //刷新表格

private:
    void initTable();
    void initButtons();
    void initLogListConnect();
    void getLogList();

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
    QString m_xFormat;
};

#endif // LOGLISTVIEW_H
