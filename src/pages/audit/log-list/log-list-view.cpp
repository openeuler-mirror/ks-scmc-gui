/**
 * @file          /ks-scmc-gui/src/pages/audit/log-list/log-list-view.cpp
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "log-list-view.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QDateTime>
#include <QHBoxLayout>

LogListView::LogListView(QWidget *parent, bool is_open_paging) : TablePage(parent, is_open_paging), m_datePicker(nullptr), m_datePickStart(nullptr), m_datePickEnd(nullptr), m_BtnApply(nullptr)
{
    is_openPaging = is_open_paging;
    m_objId = InfoWorker::generateId(this);
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
        getLogList(m_type, m_pageOn);
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
    btn_widget->setContentsMargins(0, 0, 0, 0);

    QLabel *label = new QLabel;
    label->setText("-");

    m_datePickStart = new DatePickButton;
    m_datePickEnd = new DatePickButton;
    QDate currDate = QDate::currentDate();

    m_xEnd.setDate(currDate);
    m_xStart.setDate(currDate.addDays(-7));
    m_datePickEnd->setText(m_xEnd.date().toString("yy-MM-dd"));
    m_datePickStart->setText(m_xStart.date().toString("yy-MM-dd"));

    m_datePickEnd->setStyleSheet("DatePickButton {border:1px solid #2eb3ff;"
                                 "background:transparent;"
                                 "border-radius:4px;}");
    m_datePickStart->setStyleSheet("DatePickButton {border:1px solid #2eb3ff;"
                                   "background:transparent;"
                                   "border-radius:4px;}");
    m_datePickEnd->setMinimumSize(120, 32);
    m_datePickStart->setMinimumSize(120, 32);

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
    hlayout->setContentsMargins(0, 0, 0, 0);

    addSingleWidgetButton(btn_widget);
    //    addBatchOperationButtons(date_pick);
}

void LogListView::initLogListConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::loggingRuntimeFinished, this, &LogListView::getListRuntime);
    connect(this, &LogListView::sigUpdatePaging, this, &LogListView::updatePagingInfo);
    connect(this, &LogListView::sigOpenPaging, this, &LogListView::setPaging);
}

void LogListView::getLogList(LogListPageType type, int page_on)
{
    logging::ListRuntimeRequest request;

    //    QDateTime *start_time = new QDateTime();
    //    start_time->setDate(QDate(2022,5,13));
    //    start_time->setTime(QTime(1,0));
    request.set_start_time(m_xStart.toSecsSinceEpoch());

    QDateTime time = QDateTime::currentDateTime();
    //    QString end = time.toString("yyyy/MM/dd hh:mm:ss");
    // 结束时间加一天，到结束当天的23:59:59时，否则当天的日志无法获取
    request.set_end_time(m_xEnd.toSecsSinceEpoch() + 86399);

    //    request.set_node_id(1);
    switch (type)
    {
    case CONTAINER_LOGS:
    {
        request.set_event_module(2);
        break;
    }
    case SYSTEM_LOGS:
    {
        request.set_event_module(3);
        break;
    }
    case USERS_LOGS:
    {
        request.set_event_module(4);
        break;
    }
    default:
        break;
    }

    request.set_page_no(page_on);
    //    request.set_username("chendingjian");
    //    request.set_page_size(10);
    //    request.set_page_no(1);
    //    request.set_sort_by("aa");
    //    request.set_sort_desc(true);
    InfoWorker::getInstance().listRuntimeLogging(m_objId, request);
}

void LogListView::onDateSelection()
{
}

void LogListView::getListRuntime(const QString objId, const QPair<grpc::Status, logging::ListRuntimeReply> &reply)
{
    KLOG_INFO() << "getListRuntime" << m_objId << objId;
    if (m_objId == objId)
    {
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
        if (reply.first.ok())
        {
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
            clearTable();

            m_totalPages = int(reply.second.total_pages());
            if (is_openPaging)
                emit sigOpenPaging(m_totalPages);

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

                //            if(m_xStart.date().toJulianDay() > time.date().toJulianDay() || m_xEnd.date().toJulianDay() < time.date().toJulianDay())
                //                continue;

                QStandardItem *itemUpdateTime = new QStandardItem(created);

                QStandardItem *itemObj = new QStandardItem(logging.target().data());

                QStandardItem *itemOpt = new QStandardItem();
                switch (logging.event_type())
                {
                case 101:
                    itemOpt->setText(tr("Create"));
                    break;
                case 102:
                    itemOpt->setText(tr("Update"));
                    break;
                case 103:
                    itemOpt->setText(tr("Remove"));
                    break;
                case 201:
                    itemOpt->setText(tr("Create"));
                    break;
                case 202:
                    itemOpt->setText(tr("Start"));
                    break;
                case 203:
                    itemOpt->setText(tr("Stop"));
                    break;
                case 204:
                    itemOpt->setText(tr("Remove"));
                    break;
                case 205:
                    itemOpt->setText(tr("Restart"));
                    break;
                case 301:
                    itemOpt->setText(tr("Upload"));
                    break;
                case 302:
                    itemOpt->setText(tr("Download"));
                    break;
                case 303:
                    itemOpt->setText(tr("Approve"));
                    break;
                case 304:
                    itemOpt->setText(tr("Update"));
                    break;
                case 305:
                    itemOpt->setText(tr("Remove"));
                    break;
                case 401:
                    itemOpt->setText(tr("Login"));
                    break;
                case 402:
                    itemOpt->setText(tr("Logout"));
                    break;
                case 403:
                    itemOpt->setText(tr("Create"));
                    break;
                case 404:
                    itemOpt->setText(tr("Update"));
                    break;
                case 405:
                    itemOpt->setText(tr("Remove"));
                    break;
                case 406:
                    itemOpt->setText(tr("Create"));
                    break;
                case 407:
                    itemOpt->setText(tr("Update"));
                    break;
                case 408:
                    itemOpt->setText(tr("Remove"));
                    break;
                case 409:
                    itemOpt->setText(tr("Update Password"));
                    break;
                default:
                    break;
                }

                QStandardItem *itemUser = new QStandardItem(logging.username().data());
                QStandardItem *itemRes = new QStandardItem();
                if (logging.error().data())
                    itemRes->setText("success");
                else
                    itemRes->setText("failed");
                QStandardItem *itemDetail = new QStandardItem(logging.detail().data());

                QStandardItem *itemCheck = new QStandardItem();
                itemCheck->setCheckable(false);

                setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemUpdateTime << itemObj << itemOpt << itemUser << itemRes << itemDetail);
                row++;
            }
            if (getTableRowCount() == 0)
            {
                setTableDefaultContent("-");
                setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
            }
        }
        else
        {
            KLOG_INFO() << "get ListDB Result failed: " << reply.first.error_message().data();
            setTableDefaultContent("-");
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
        }
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
    m_xStart = m_datePicker->getStartDate();
    m_xEnd = m_datePicker->getEndDate();
    updateInfo();
}

void LogListView::updatePagingInfo(int page_on)
{
    m_pageOn = page_on;
    updateInfo();
}

void LogListView::setLogListPageType(LogListPageType type)
{
    m_type = type;
}
