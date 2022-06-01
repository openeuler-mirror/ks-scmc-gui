#include "warning-list-view.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QDateTime>
#include <QHBoxLayout>

#define WARN_IDS "warn id"
#define WARN_NODE_ID "warn node id"
#define WARN_CONTAINER_ID "warn container id"
#define WARN_CONTAINER_NAME "warn container name"

WarningListView::WarningListView(QWidget *parent, bool isShowContainerName, bool is_open_paging) :TablePage(parent,is_open_paging)
{
    is_showContainerName = isShowContainerName;
    is_openPaging = is_open_paging;
    initButtons();
    initTable();
    initLogListConnect();
}

WarningListView::~WarningListView()
{
}

void WarningListView::updateInfo(QString keyword)
{
    KLOG_INFO() << "WarningListView updateInfo";
    clearText();
    if (keyword.isEmpty())
    {
        connect(&InfoWorker::getInstance(), &InfoWorker::loggingListWarnFinished, this, &WarningListView::getListWarning);
        //        connect(&InfoWorker::getInstance(), &InfoWorker::loggingReadWarnFinished, this, &WarningListView::getReadWarning);
        getWarningList(m_type,m_pageOn);
        //        getReadWarn();
    }
}

void WarningListView::initTable()
{
    QList<QString> tableHHeaderDate = {
        ""};
    if(is_showContainerName)
        tableHHeaderDate.append(tr("Container Name"));
    tableHHeaderDate.append(QList<QString>() << tr("Current Node")
                                             << tr("Warning Status")
                                             << tr("Warning Content")
                                             << tr("Upadate Times")
                                             << tr("Operation"));

    setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>{{ACTION_BUTTON_TYPE_WARN_READ, QPair<QString, QString>{tr("Readed"), tr("Readed")}/*},
                                                                                                   {ACTION_BUTTON_TYPE_WARN_IGNORE, QPair<QString, QString>{tr("Ignore"), tr("Ignore")}*/}});
    setHeaderSections(tableHHeaderDate);
    //    setHeaderCheckable(false);
    setTableDefaultContent("-");
    //    setTableSingleChoose(true);
}

void WarningListView::initButtons()
{
    QMap<int, QPushButton *> opBtnMap;
    //按钮
    const QMap<int, QString> btnNameMap = {
        {OPERATION_BUTTOM_WARN_READ, tr("Readed")}/*,
        {OPERATION_BUTTOM_WARN_IGNORE, tr("Ignore")}*/};
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
//    connect(opBtnMap[OPERATION_BUTTOM_WARN_IGNORE], &QPushButton::clicked, this, &WarningListView::onBtnIgnore);
    connect(this, &WarningListView::sigWarnRead, this, &WarningListView::onBtnReadLabel);
//    connect(this, &WarningListView::sigWarnIgnore, this, &WarningListView::onBtnIgnoreLabel);

    addBatchOperationButtons(QList<QPushButton *>() << opBtnMap[OPERATION_BUTTOM_WARN_READ]
                                                    /*<< opBtnMap[OPERATION_BUTTOM_WARN_IGNORE]*/);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void WarningListView::initLogListConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::loggingListWarnFinished, this, &WarningListView::getListWarning);
    //     connect(&InfoWorker::getInstance(), &InfoWorker::loggingReadWarnFinished, this, &WarningListView::getReadWarn);
    connect(this,&WarningListView::sigUpdatePaging,this,&WarningListView::updatePagingInfo);
    connect(this,&WarningListView::sigOpenPaging,this,&WarningListView::setPaging);
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

    InfoWorker::getInstance().listWarnLogging(request);
}

void WarningListView::getReadWarn(QList<int64_t> ids)
{
    InfoWorker::getInstance().readWarnLogging(ids);
}

void WarningListView::getListWarning(const QPair<grpc::Status, logging::ListWarnReply> &reply)
{
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
    if (reply.first.ok())
    {
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
        clearTable();

        m_totalPages = int(reply.second.total_pages());
        if(is_openPaging == true)
            emit sigOpenPaging(m_totalPages);

        int size = reply.second.logs_size();
        if (size <= 0)
        {
            setTableDefaultContent("-");
            return;
        }

        int row = 0;
        QMap<QString, QVariant> infoMap;
        for (auto logging : reply.second.logs())
        {
            //            m_idsMap[row] = logging.id();
            qint64 loging_id = logging.id();
            infoMap.insert(WARN_IDS, loging_id);
            infoMap.insert(WARN_NODE_ID, QString(int(logging.node_id())));
            infoMap.insert(WARN_CONTAINER_ID, logging.container_id().data());
            infoMap.insert(WARN_CONTAINER_NAME, logging.container_name().data());

            QStandardItem *itemCheck = new QStandardItem();
            itemCheck->setCheckable(true);

            QStandardItem *item_container = new QStandardItem(logging.container_name().data());
            item_container->setData(QVariant::fromValue(infoMap));

            //            QStandardItem *item_image = new QStandardItem(logging.container_id().data()); // 镜像

            QStandardItem *item_node = new QStandardItem(logging.node_info().data());
            item_node->setData(QVariant::fromValue(infoMap));

            QStandardItem *item_status = new QStandardItem("unknown");
            if (logging.have_read())
                item_status->setText(tr("Readed"));
            else
                item_status->setText(tr("Unread"));

            QStandardItem *item_content = new QStandardItem(logging.detail().data());
            //            switch (logging.event_type()) {
            //            case 1001: //资源忙碌
            //                item_content->setText(tr("Resource usage"));
            //                break;
            //            case 1002: //节点关闭
            //                item_content->setText(tr("Node close"));
            //                break;
            //            case 1003: //非法容器
            //                item_content->setText(tr("illegal container"));
            //                break;
            //            case 1004: //节点异常
            //                item_content->setText(tr("Node abnormal"));
            //                break;
            //            default:
            //                break;
            //            }

            QDateTime time = QDateTime::fromSecsSinceEpoch(logging.updated_at());
            QString update = time.toString("yyyy/MM/dd hh:mm:ss");
            QStandardItem *item_update_time = new QStandardItem(update);
            if(is_showContainerName)
                setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << item_container << item_node << item_status << item_content << item_update_time);
            else
                setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << item_node << item_status << item_content << item_update_time);

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

//void WarningListView::getReadWarning(const QPair<grpc::Status, logging::ReadWarnReply> &reply)
//{
//    if (reply.first.ok())
//    {
////        int size = reply.second;
////        if (size <= 0)
////            ;
//    }
//}

void WarningListView::setLogListPageType(WarningListPageType type)
{
    m_type = type;
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
        getReadWarn(ids);
        updateInfo();
    }
}

void WarningListView::onBtnIgnore()
{
    //    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);
    //    if (!info.isEmpty())
    //    {
    //        int64_t ids = info.at(0).value(WARN_IDS).toInt();
    //        getReadWarn(ids);
    //    }
}

void WarningListView::onBtnReadLabel(int row)
{
    auto infoMap = getItem(row, 1)->data().value<QMap<QString, QVariant>>();
    if (infoMap.isEmpty())
        return;
    qint64 ids = infoMap.value(WARN_IDS).toInt();
    if(ids > 0)
        getReadWarn(QList<int64_t>() << ids);
    KLOG_INFO() << "ids = " << ids;
    updateInfo();
}

void WarningListView::onBtnIgnoreLabel(int row)
{
}

void WarningListView::updatePagingInfo(int page_on)
{
//    getWarningList(m_type,page_on);
    m_pageOn = page_on;
    updateInfo();
}
