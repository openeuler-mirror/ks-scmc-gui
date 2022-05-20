#include "warning-list-view.h"
#include <kiran-log/qt5-log-i.h>
#include <QDateTime>
#include <QHBoxLayout>
#include <widget-property-helper.h>

WaringListView::WaringListView(QWidget *parent) : TablePage(parent)
{
    initButtons();
    initTable();
    initLogListConnect();
}

WaringListView::~WaringListView()
{

}

void WaringListView::updateInfo(QString keyword)
{
    KLOG_INFO() << "WaringListView updateInfo";
    clearText();
    if (keyword.isEmpty())
    {
        connect(&InfoWorker::getInstance(), &InfoWorker::loggingListWarnFinished, this, &WaringListView::getListWarning);
        connect(&InfoWorker::getInstance(), &InfoWorker::loggingReadWarnFinished, this, &WaringListView::getReadWarning);
        getWarningList(m_type);
        getReadWarn();
    }
}

void WaringListView::initTable()
{
    QList<QString> tableHHeaderDate = {
        "",
        tr("Container Name"),
        tr("Image"),
        tr("Current Node"),
        tr("Warning Status"),
        tr("Warning Content"),
        tr("Upadate Times"),
        tr("Operation")};

    setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QString>{{ACTION_BUTTON_TYPE_WARN_READ,  tr("Readed")},
                                                                                   {ACTION_BUTTON_TYPE_WARN_IGNORE, tr("Ignore")}});
    setHeaderSections(tableHHeaderDate);
//    setHeaderCheckable(false);
    setTableDefaultContent("-");
//    setTableSingleChoose(true);
}

void WaringListView::initButtons()
{
    QMap<int, QPushButton *> opBtnMap;
    //按钮
    const QMap<int, QString> btnNameMap = {
        {OPERATION_BUTTOM_WARN_READ, tr("Readed")},
        {OPERATION_BUTTOM_WARN_IGNORE, tr("Ignore")}};
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
    connect(opBtnMap[OPERATION_BUTTOM_WARN_READ], &QPushButton::clicked, this, &WaringListView::onBtnRead);
    connect(opBtnMap[OPERATION_BUTTOM_WARN_IGNORE], &QPushButton::clicked, this, &WaringListView::onBtnIgnore);
    connect(this,&WaringListView::sigImagePass,this,&WaringListView::onBtnReadLabel);
    connect(this,&WaringListView::sigImageRefuse,this,&WaringListView::onBtnIgnoreLabel);

    addBatchOperationButtons(QList<QPushButton *>() << opBtnMap[OPERATION_BUTTOM_WARN_READ]
                                                    << opBtnMap[OPERATION_BUTTOM_WARN_IGNORE]);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void WaringListView::initLogListConnect()
{
     connect(&InfoWorker::getInstance(), &InfoWorker::loggingListWarnFinished, this, &WaringListView::getListWarning);
     connect(&InfoWorker::getInstance(), &InfoWorker::loggingReadWarnFinished, this, &WaringListView::getReadWarn);
}

void WaringListView::getWarningList(WarningListPageType type)
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

    InfoWorker::getInstance().listWarnLogging(request);
}

void WaringListView::getReadWarn()
{
    InfoWorker::getInstance().readWarnLogging();
}

void WaringListView::getListWarning(const QPair<grpc::Status, logging::ListWarnReply> &reply)
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
            QStandardItem *itemCheck = new QStandardItem();
            itemCheck->setCheckable(true);

            QStandardItem *item_container = new QStandardItem(logging.container_name().data());

            QStandardItem *item_image = new QStandardItem(logging.container_id().data()); // 镜像

            QStandardItem *item_node = new QStandardItem(logging.node_info().data());

            QStandardItem *item_status = new QStandardItem("unknown");

            QStandardItem *item_content = new QStandardItem("unknown");
            switch (logging.event_type()) {
            case 1001: //资源忙碌
                item_content->setText(tr("Resource usage"));
                break;
            case 1002: //节点关闭
                item_content->setText(tr("Node close"));
                break;
            case 1003: //非法容器
                item_content->setText(tr("illegal container"));
                break;
            case 1004: //节点异常
                item_content->setText(tr("Node abnormal"));
                break;
            default:
                break;
            }

            QDateTime time = QDateTime::fromSecsSinceEpoch(logging.updated_at());
            QString update = time.toString("yyyy/MM/dd hh:mm:ss");
            QStandardItem *item_update_time = new QStandardItem(update);

            setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << item_container  << item_image << item_node << item_status << item_content << item_update_time);
            row++;
        }
        if(getTableRowCount() == 0)
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

void WaringListView::getReadWarning(const QPair<grpc::Status, logging::ReadWarnReply> &reply)
{
    if (reply.first.ok())
    {
//        int size = reply.second;
//        if (size <= 0)
//            ;
    }
}

void WaringListView::setLogListPageType(WarningListPageType type)
{
    m_type = type;
}

void WaringListView::onBtnRead()
{
}

void WaringListView::onBtnIgnore()
{

}

void WaringListView::onBtnReadLabel(int row)
{

}

void WaringListView::onBtnIgnoreLabel(int row)
{

}

