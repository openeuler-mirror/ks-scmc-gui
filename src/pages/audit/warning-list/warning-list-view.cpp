/**
 * @file          /ks-scmc-gui/src/pages/audit/warning-list/warning-list-view.cpp
 * @brief
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "warning-list-view.h"
#include <kiran-log/qt5-log-i.h>
#include <notification-manager.h>
#include <widget-property-helper.h>
#include <QDateTime>
#include <QHBoxLayout>

#define WARN_IDS "warn id"
#define WARN_NODE_ID "warn node id"
#define WARN_CONTAINER_ID "warn container id"
#define WARN_CONTAINER_NAME "warn container name"

#define TIMEOUT 3000
WarningListView::WarningListView(QWidget *parent, bool isShowContainerName, bool is_open_paging) : TablePage(parent, is_open_paging),
                                                                                                   m_totalPages(0),
                                                                                                   m_totalWarnning(0)
{
    m_timer = new QTimer(this);

    m_ObjId = Logging::generateId(this);
    is_showContainerName = isShowContainerName;
    is_openPaging = is_open_paging;
    initButtons();
    initTable();
    initLogListConnect();
}

WarningListView::~WarningListView()
{
    if (m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }
}

void WarningListView::updateInfo(QString keyword)
{
    clearText();
    clearCheckState();
    if (keyword.isEmpty())
    {
        getWarningList(m_type, m_pageOn);
    }
}

void WarningListView::initTable()
{
    QList<QString> tableHHeaderDate = {
        ""};
    if (is_showContainerName)
        tableHHeaderDate.append(tr("Container Name"));
    tableHHeaderDate.append(QList<QString>() << tr("Current Node")
                                             << tr("Warning Status")
                                             << tr("Warning Content")
                                             << tr("Upadate Times")
                                             << tr("Operation"));

    setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>{{
                                                     ACTION_BUTTON_TYPE_WARN_READ, QPair<QString, QString>{tr("Readed"), tr("Readed")} /*},
                                                                                                   {ACTION_BUTTON_TYPE_WARN_IGNORE, QPair<QString, QString>{tr("Ignore"), tr("Ignore")}*/
                                                 }});
    setHeaderSections(tableHHeaderDate);
    //    setHeaderCheckable(false);
    setTableDefaultContent("-");
    //    setTableSingleChoose(true);
}

void WarningListView::initButtons()
{
    QMap<int, QPushButton *> opBtnMap;
    // 按钮
    const QMap<int, QString> btnNameMap = {
        {OPERATION_BUTTOM_WARN_READ, tr("Readed")}};
    for (auto iter = btnNameMap.begin(); iter != btnNameMap.end(); iter++)
    {
        QString name = iter.value();
        QPushButton *btn = new QPushButton(this);
        btn->setObjectName("btn");

        if (name == tr("Ignore"))
        {
            btn->setStyleSheet("#btn{background-color:#ff4b4b;"
                               "border:none;"
                               "border-radius: 4px;"
                               "color:#ffffff;}"
                               "#btn:hover{ background-color:#ff6c6c;}"
                               "#btn:focus{outline:none;}"
                               "#btn:disabled{color:#919191;background:#393939;}");
        }
        else
            btn->setStyleSheet("#btn{background-color:#2eb3ff;"
                               "border:none;"
                               "border-radius: 4px;"
                               "color:#ffffff;}"
                               "#btn:hover{ background-color:#77ceff;}"
                               "#btn:focus{outline:none;}"
                               "#btn:disabled{color:#919191;background:#393939;}");
        btn->setText(name);
        btn->setFixedSize(QSize(78, 32));
        opBtnMap.insert(iter.key(), btn);
    }
    connect(opBtnMap[OPERATION_BUTTOM_WARN_READ], &QPushButton::clicked, this, &WarningListView::onBtnRead);
    connect(this, &WarningListView::sigWarnRead, this, &WarningListView::onBtnReadLabel);

    addBatchOperationButtons(QList<QPushButton *>() << opBtnMap[OPERATION_BUTTOM_WARN_READ]);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void WarningListView::initLogListConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::loggingListWarnFinished, this, &WarningListView::getListWarningResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::loggingReadWarnFinished, this, &WarningListView::getReadWarningResult);
    connect(this, &WarningListView::sigUpdatePaging, this, &WarningListView::updatePagingInfo);
    connect(m_timer, &QTimer::timeout, this, [this]()
            { refresh(); });
}

void WarningListView::getWarningList(WarningListPageType type, int page_on)
{
    logging::ListWarnRequest request;

    switch (type)
    {
    case CONTAINER_WARNING:
    {
        request.set_event_module(2);
        break;
    }
    case NODE_WARNING:
    {
        request.set_event_module(1);
        break;
    }
    default:
        break;
    }

    request.set_page_no(page_on);
    InfoWorker::getInstance().listWarnLogging(m_ObjId, request);
}

void WarningListView::readWarn(QList<int64_t> ids)
{
    InfoWorker::getInstance().readWarnLogging(m_ObjId, ids);
}

void WarningListView::refresh()
{
    clearText();
    getWarningList(m_type, m_pageOn);
}

void WarningListView::getListWarningResult(const QString objId, const QPair<grpc::Status, logging::ListWarnReply> &reply)
{
    if (m_ObjId != objId)
        return;

    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Get warn list result failed: " << reply.first.error_message().data();
        setTableDefaultContent("-");
        if (grpc::StatusCode::DEADLINE_EXCEEDED == reply.first.error_code())
        {
            setTips(tr("Response timeout!"));
        }
        return;
    }
    QList<qint64> ids;
    getCheckedItemsId(ids);

    clearTable();

    // 更新总页数
    m_totalPages = int(reply.second.total_pages());
    if (is_openPaging == true)
    {
        if (m_totalPages < 1)
            m_totalPages = 1;
        if (m_pageOn > m_totalPages)
            m_pageOn = m_totalPages;
        setPaging(m_totalPages);
    }

    // 更新告警总数
    auto totalWarnning = reply.second.total_warnnings();
    if (totalWarnning != m_totalWarnning)
    {
        m_totalWarnning = totalWarnning;
        emit sigUpdateWaringSums(m_totalWarnning);
    }

    int size = reply.second.logs_size();
    if (size <= 0)
    {
        setHeaderCheckable(false);
        setTableDefaultContent("-");
        return;
    }
    setHeaderCheckable(true);

    int row = 0;
    QMap<QString, QVariant> infoMap;
    for (auto logging : reply.second.logs())
    {
        qint64 loging_id = logging.id();
        infoMap.insert(WARN_IDS, loging_id);
        infoMap.insert(WARN_NODE_ID, QString(int(logging.node_id())));
        infoMap.insert(WARN_CONTAINER_ID, logging.container_id().data());
        infoMap.insert(WARN_CONTAINER_NAME, logging.container_name().data());

        QStandardItem *itemCheck = new QStandardItem();
        itemCheck->setCheckable(true);

        QStandardItem *item_container = new QStandardItem(logging.container_name().data());
        item_container->setData(QVariant::fromValue(infoMap));

        QStandardItem *item_node = new QStandardItem(logging.node_info().data());
        item_node->setData(QVariant::fromValue(infoMap));

        QStandardItem *item_status = new QStandardItem("unknown");
        if (logging.have_read())
            item_status->setText(tr("Readed"));
        else
            item_status->setText(tr("Unread"));

        QStandardItem *item_content = new QStandardItem(logging.detail().data());

        QDateTime time = QDateTime::fromSecsSinceEpoch(logging.updated_at());
        QString update = time.toString("yyyy/MM/dd hh:mm:ss");
        QStandardItem *item_update_time = new QStandardItem(update);
        if (is_showContainerName)
            setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << item_container << item_node << item_status << item_content << item_update_time);
        else
            setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << item_node << item_status << item_content << item_update_time);

        // 恢复选中状态
        if (-1 != ids.indexOf(loging_id))
        {
            itemCheck->setCheckState(Qt::Checked);
        }

        row++;
    }
}

void WarningListView::getReadWarningResult(const QString objId, const QPair<grpc::Status, logging::ReadWarnReply> &reply)
{
    if (m_ObjId != objId)
        return;

    if (reply.first.ok())
    {
        updateInfo();
    }
    else
    {
        NotificationManager::sendNotify(tr("Read warning faild"), reply.first.error_message().data());
        KLOG_DEBUG() << "Read warning failed:" << reply.first.error_message().data();
    }
}

void WarningListView::setLogListPageType(WarningListPageType type)
{
    m_type = type;
}

void WarningListView::showEvent(QShowEvent *event)
{
    m_timer->start(TIMEOUT);
    TablePage::showEvent(event);
}

void WarningListView::hideEvent(QHideEvent *event)
{
    m_timer->stop();
    TablePage::hideEvent(event);
}

void WarningListView::onBtnRead()
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);
    if (!info.isEmpty())
    {
        QList<int64_t> ids;
        foreach (auto idInfo, info)
        {
            ids.append(idInfo.value(WARN_IDS).toInt());
        }
        readWarn(ids);
    }
}

void WarningListView::onBtnReadLabel(int row)
{
    auto infoMap = getItem(row, 1)->data().value<QMap<QString, QVariant>>();
    if (infoMap.isEmpty())
        return;
    qint64 ids = infoMap.value(WARN_IDS).toInt();
    if (ids > 0)
        readWarn(QList<int64_t>() << ids);
}

void WarningListView::updatePagingInfo(int page_on)
{
    m_pageOn = page_on;
    updateInfo();
}
void WarningListView::getCheckedItemsId(QList<qint64> &ids)
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);

    foreach (auto idMap, info)
    {
        ids.append(idMap.value(WARN_IDS).toInt());
    }
}