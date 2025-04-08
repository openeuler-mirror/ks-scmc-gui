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

#define TIMEOUT 3000
LogListView::LogListView(QWidget *parent, bool is_open_paging) : TablePage(parent, is_open_paging),
                                                                 m_datePicker(nullptr),
                                                                 m_datePickStart(nullptr),
                                                                 m_datePickEnd(nullptr),
                                                                 m_BtnApply(nullptr),
                                                                 m_searchKey("")
{
    m_timer = new QTimer(this);
    is_openPaging = is_open_paging;
    m_objId = InfoWorker::generateId(this);
    initButtons();
    initTable();
    initLogListConnect();

    connect(this, &TablePage::sigPagingSearch, this, &LogListView::searchClicked);
    connect(this, &TablePage::sigRefreshSearchResult, this,
            [this]()
            {
                m_searchKey = "";
            });
    connect(m_timer, &QTimer::timeout, this, [this]()
            { getLogList(m_type, m_pageOn); });
}

LogListView::~LogListView()
{
    if (m_datePicker)
    {
        delete m_datePicker;
        m_datePicker = nullptr;
    }
    if (m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }
}

void LogListView::updateInfo(QString keyword)
{
    clearText();
    // 刷新搜索结果
    if (keyword.isEmpty())
    {
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
    //    setSearchableCol(2);  //设置"对象"列可供搜索
    setTableDefaultContent("-");
    //    setTableSingleChoose(true);
}

void LogListView::initButtons()
{
    QWidget *btn_widget = new QWidget(this);
    btn_widget->setContentsMargins(0, 0, 0, 0);

    QLabel *label = new QLabel(btn_widget);
    label->setText("-");

    m_datePickStart = new DatePickButton(btn_widget);
    m_datePickEnd = new DatePickButton(btn_widget);
    QDateTime currTime = QDateTime::currentDateTime();  // 获取当前时间

    m_xEnd = currTime;
    m_xStart.setDate(currTime.date().addDays(-7));
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
            [this](QString date)
            {
                m_datePickStart->setText(date);
            });
    connect(m_datePicker, &DatePicker::sigEndDateChange,
            [this](QString date)
            {
                m_datePickEnd->setText(date);
            });

    m_BtnApply = new QPushButton(this);
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

    addSingleOperationWidget(btn_widget);
}

void LogListView::initLogListConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::loggingRuntimeFinished, this, &LogListView::getListRuntime);
    connect(this, &LogListView::sigUpdatePaging, this, &LogListView::updatePagingInfo);
}

void LogListView::getLogList(LogListPageType type, int page_on)
{
    logging::ListRuntimeRequest request;
    request.set_start_time(m_xStart.toSecsSinceEpoch());
    request.set_end_time(m_xEnd.toSecsSinceEpoch());
    KLOG_DEBUG() << "Get log list of range:"
                 << "start:" << m_xStart.toString("yyyy/MM/dd hh:mm:ss")
                 << "end:" << m_xEnd.toString("yyyy/MM/dd hh:mm:ss")
                 << "curr page:" << page_on;

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
    if (m_searchKey != "")
    {
        KLOG_DEBUG() << "Log search key : " << m_searchKey;
        request.mutable_filter()->set_fuzzy(true);

        std::string query = m_searchKey.toStdString();
        request.mutable_filter()->set_query(query);

        std::string property = "target";
        request.mutable_filter()->set_property(property);
    }

    request.set_page_no(page_on);

    InfoWorker::getInstance().listRuntimeLogging(m_objId, request);
}

void LogListView::onDateSelection()
{
}

void LogListView::getListRuntime(const QString objId, const QPair<grpc::Status, logging::ListRuntimeReply> &reply)
{
    if (m_objId != objId)
        return;

    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Get list runtime result failed: " << reply.first.error_message().data();
        setTableDefaultContent("-");
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
        if (grpc::StatusCode::DEADLINE_EXCEEDED == reply.first.error_code())
        {
            setTips(tr("Response timeout!"));
        }
        return;
    }

    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
    clearTable();

    m_totalPages = int(reply.second.total_pages());
    if (is_openPaging)
    {
        if (m_totalPages < 1)
            m_totalPages = 1;
        if (m_pageOn > m_totalPages)
            m_pageOn = m_totalPages;

        setPaging(m_totalPages);
    }

    int size = reply.second.logs_size();
    KLOG_DEBUG() << "Log size:" << size;
    if (size <= 0)
    {
        if (m_searchKey != "")
            setSeachPageNone();
        setTableDefaultContent("-");
        return;
    }

    int row = 0;
    for (auto logging : reply.second.logs())
    {
        QDateTime time = QDateTime::fromSecsSinceEpoch(logging.created_at());
        QString created = time.toString("yyyy/MM/dd hh:mm:ss");

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
        if (!QString::fromStdString(logging.error().data()).isEmpty())
            itemRes->setText(tr("failed"));
        else
            itemRes->setText(tr("success"));
        QStandardItem *itemDetail = new QStandardItem(logging.detail().data());

        QStandardItem *itemCheck = new QStandardItem();
        itemCheck->setCheckable(false);

        setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemUpdateTime << itemObj << itemOpt << itemUser << itemRes << itemDetail);
        row++;
    }
    if (getRowCount() == 0)
    {
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
    m_xStart = m_datePicker->getStartDate();
    m_xEnd = m_datePicker->getEndDate();
    updateInfo();
}

void LogListView::updatePagingInfo(int page_on)
{
    m_pageOn = page_on;
    getLogList(m_type, m_pageOn);
}

void LogListView::searchClicked(QString key)
{
    //    clearText();
    m_searchKey = key;
    getLogList(m_type, m_pageOn);
    //    updateInfo();
}

void LogListView::setLogListPageType(LogListPageType type)
{
    m_type = type;
}
