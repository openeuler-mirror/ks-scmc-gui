#include "log-list-view.h"
#include <kiran-log/qt5-log-i.h>
#include <QDateTime>
#include <QHBoxLayout>
#include <widget-property-helper.h>

LogListView::LogListView(QWidget *parent) : TablePage(parent)
  ,m_datePicker(nullptr)
  ,m_datePickStart(nullptr)
  ,m_datePickEnd(nullptr)
  ,m_BtnApply(nullptr)
{
    initButtons();
    initTable();
    initLogListConnect();
}

LogListView::~LogListView()
{

}

void LogListView::updateInfo(QString keyword)
{
    KLOG_INFO() << "LogListView updateInfo";
    clearText();
    if (keyword.isEmpty())
    {
        connect(&InfoWorker::getInstance(), &InfoWorker::loggingRuntimeFinished, this, &LogListView::getListRuntime);
        getLogList();
    }

}

void LogListView::initTable()
{
    QList<QString> tableHHeaderDate = {
        "",
        tr("Operation Time"),
        tr("Object"),
        tr("Operation"),
        tr("Operation User"),
        tr("Operation Result"),
        tr("Operation Details")};
    setHeaderSections(tableHHeaderDate);
    setHeaderCheckable(false);
    setTableDefaultContent("-");
//    setTableSingleChoose(true);
}

void LogListView::initButtons()
{
    QWidget *btn_widget = new QWidget;
    btn_widget->setContentsMargins(0,0,0,0);

    QLabel *label = new QLabel;
    label->setText("-");

    m_datePickStart = new DatePickButton;
    m_datePickEnd = new DatePickButton;
    QDate currDate = QDate::currentDate();
    m_datePickEnd->setText(currDate.toString("yy-MM-dd"));
    m_datePickStart->setText(currDate.addDays(-7).toString("yy-MM-dd"));

    m_datePickEnd->setStyleSheet("DatePickButton {border:1px solid #2eb3ff;"
                                 "background:transparent;"
                                 "border-radius:4px;}");
    m_datePickStart->setStyleSheet("DatePickButton {border:1px solid #2eb3ff;"
                                   "background:transparent;"
                                   "border-radius:4px;}");
    m_datePickEnd->setMinimumSize(120,32);
    m_datePickStart->setMinimumSize(120,32);

    m_datePicker = new DatePicker;
    connect(m_datePicker, &DatePicker::sigStartdateChange,
            [this](QString date) {
                m_datePickStart->setText(date);
            });
    connect(m_datePicker, &DatePicker::sigEndDateChange,
            [this](QString date) {
                m_datePickEnd->setText(date);
            });

    m_BtnApply = new QPushButton;
    Kiran::WidgetPropertyHelper::setButtonType(m_BtnApply, Kiran::BUTTON_Default);
    m_BtnApply->setText(tr("Apply"));

    connect(m_datePickStart, &DatePickButton::clicked, this, &LogListView::popupStartDatePicker);
    connect(m_datePickEnd, &DatePickButton::clicked, this, &LogListView::popupEndDatePicker);
    connect(m_BtnApply, &QPushButton::clicked, this, &LogListView::applyDatePicker);

    QHBoxLayout *hlayout = new QHBoxLayout(btn_widget);
    hlayout->addWidget(m_datePickStart);
    hlayout->addWidget(label);
    hlayout->addWidget(m_datePickEnd);
    hlayout->addWidget(m_BtnApply);
    hlayout->setContentsMargins(0,0,0,0);

    addSingleWidgetButton(btn_widget);
//    addBatchOperationButtons(date_pick);

}

void LogListView::initLogListConnect()
{
     connect(&InfoWorker::getInstance(), &InfoWorker::loggingRuntimeFinished, this, &LogListView::getListRuntime);
}

void LogListView::getLogList()
{
    logging::ListRuntimeRequest request;

    QDateTime *start_time = new QDateTime();
    start_time->setDate(QDate(2022,5,5));
    start_time->setTime(QTime(1,0));
    request.set_start_time(start_time->toSecsSinceEpoch());

    QDateTime time = QDateTime::currentDateTime();
//    QString end = time.toString("yyyy/MM/dd hh:mm:ss");
    request.set_end_time(time.toSecsSinceEpoch());

//    request.set_node_id(1);
    request.set_event_module(2);
//    request.set_username("chendingjian");
//    request.set_page_size(10);
//    request.set_page_no(1);
//    request.set_sort_by("aa");
//    request.set_sort_desc(true);

    InfoWorker::getInstance().listRuntimeLogging(request);
}

void LogListView::onDateSelection()
{

}

void LogListView::getListRuntime(const QPair<grpc::Status,logging::ListRuntimeReply> &reply)
{
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
    if (reply.first.ok())
    {
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
        clearTable();
        int size = reply.second.logs_size();
        if (size <= 0)
        {
            setTableDefaultContent("-");
            return;
        }

        int row = 0;

        for (auto logging : reply.second.logs())
        {
            QDateTime time = QDateTime::fromSecsSinceEpoch(logging.created_at());
            QString created = time.toString("yyyy/MM/dd hh:mm:ss");
            KLOG_INFO() << __func__ << "due time = " << created;
            QStandardItem *itemUpdateTime = new QStandardItem(created);

            QStandardItem *itemObj = new QStandardItem(logging.target().data());

            QStandardItem *itemOpt = new QStandardItem();
            switch (logging.event_type()) {
            case 0:
                itemOpt->setText(tr("Run"));
                break;
            case 1:
                itemOpt->setText(tr("Stop"));
                break;
            case 2:
                itemOpt->setText(tr("Restart"));
                break;
            case 3:
                itemOpt->setText(tr("Delete"));
                break;
            default:
                break;
            }

            QStandardItem *itemUser = new QStandardItem(logging.username().data());
            QStandardItem *itemRes = new QStandardItem(logging.error().data());
            QStandardItem *itemDetail = new QStandardItem(logging.detail().data());

            QStandardItem *itemCheck = new QStandardItem();
            itemCheck->setCheckable(false);

            setTableItems(row, 0, QList<QStandardItem *>() << itemCheck  << itemUpdateTime << itemObj << itemOpt << itemUser << itemRes << itemDetail);
            row++;
        }
    }
    else
    {
        KLOG_INFO() << "get ListDB Result failed: " << reply.first.error_message().data();
        setTableDefaultContent("-");
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    }

}

void LogListView::popupStartDatePicker()
{
    QPoint point = m_datePickStart->mapToGlobal(QPoint(0, 0));
    m_datePicker->move(QPoint(point.x(), point.y() + 35));
    m_datePicker->showDatePicker(CALENDAR_TYPE_START);
    m_datePicker->show();
}

void LogListView::popupEndDatePicker()
{
    QPoint point = m_datePickEnd->mapToGlobal(QPoint(0, 0));
    m_datePicker->move(QPoint(point.x(), point.y() + 35));
    m_datePicker->showDatePicker(CALENDAR_TYPE_END);
    m_datePicker->show();
}

void LogListView::applyDatePicker()
{
    m_xFormat = "dd hh:mm";
    m_xStart = m_datePicker->getStartDate();
    m_xEnd = m_datePicker->getEndDate();
//    m_xInterval = 1 * 60;
//    m_xTitle = tr("Time particle density(%1 hour)").arg(m_xInterval / 60);
//    InfoWorker::getInstance().monitorHistory(m_nodeId, m_xStart.toSecsSinceEpoch(), m_xEnd.toSecsSinceEpoch(), m_xInterval, m_containerId);
}

