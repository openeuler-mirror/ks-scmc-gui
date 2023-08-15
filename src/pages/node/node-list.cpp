#include "node-list.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QDesktopWidget>
#include "common/message-dialog.h"
#include "node-addition.h"
#include "rpc.h"
#define NODE_ID "node id"
#define ACTION_COL 1
NodeList::NodeList(QWidget *parent) : CommonPage(parent),
                                      m_nodeAddition(nullptr)
{
    m_mapStatus.insert(0, QPair<QString, QString>(tr("Offline"), "red"));
    m_mapStatus.insert(1, QPair<QString, QString>(tr("Unknown"), "black"));
    m_mapStatus.insert(10, QPair<QString, QString>(tr("Online"), "green"));
    initButtons();
    initTable();
}

NodeList::~NodeList()
{
    if (m_nodeAddition)
    {
        delete m_nodeAddition;
        m_nodeAddition = nullptr;
    }
}

void NodeList::updateInfo(QString keyword)
{
    KLOG_INFO() << "NodeList updateInfo";
    clearText();
    InfoWorker::getInstance().disconnect();
    if (keyword.isEmpty())
    {
        initNodeConnect();
        getNodeList();
    }
}

void NodeList::onCreateNode()
{
    if (!m_nodeAddition)
    {
        m_nodeAddition = new NodeAddition();
        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_nodeAddition->move(screenGeometry.x() + (screenGeometry.width() - this->width()) / 2,
                             screenGeometry.y() + (screenGeometry.height() - this->height()) / 2);
        m_nodeAddition->show();

        connect(m_nodeAddition, &NodeAddition::sigSave, this, &NodeList::onSaveSlot);
        connect(m_nodeAddition, &NodeAddition::destroyed,
                [=] {
                    KLOG_INFO() << " m_nodeAdditiong destroy";
                    m_nodeAddition->deleteLater();
                    m_nodeAddition = nullptr;
                });
    }
}

void NodeList::onRemoveNode()
{
    KLOG_INFO() << "onRemoveNode";
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(0);
    std::vector<int64_t> node_ids;
    foreach (auto &idMap, info)
    {
        KLOG_INFO() << idMap.value(NODE_ID).toInt();
        node_ids.push_back(idMap.value(NODE_ID).toInt());
    }

    if (!node_ids.empty())
    {
        MessageDialog::StandardButton ret = MessageDialog::message(tr("Delete Node"),
                                                                   tr("Are you sure you want to delete the node?"),
                                                                   tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                                                   ":/images/warning.svg",
                                                                   MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
        if (ret == MessageDialog::StandardButton::Yes)
        {
            InfoWorker::getInstance().removeNode(node_ids);
        }
        else
            KLOG_INFO() << "cancel";
    }
}

void NodeList::onMonitor(int row)
{
    KLOG_INFO() << row;
}

void NodeList::onSaveSlot(QMap<QString, QString> Info)
{
    KLOG_INFO() << "name" << Info["Node Name"] << "ip" << Info["Node IP"];
    node::CreateRequest request;
    request.set_name(Info["Node Name"].toStdString());
    request.set_address(Info["Node IP"].toStdString());
    InfoWorker::getInstance().createNode(request);
}

void NodeList::getListResult(const QPair<grpc::Status, node::ListReply> &reply)
{
    if (reply.first.ok())
    {
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
        int size = reply.second.nodes_size();
        if (size <= 0)
        {
            setTableDefaultContent("-");
            return;
        }
        clearTable();
        int row = 0;
        QMap<QString, QVariant> idMap;
        for (auto node : reply.second.nodes())
        {
            KLOG_INFO() << "nodeid:" << node.id();
            qint64 nodeId = node.id();
            idMap.insert(NODE_ID, nodeId);

            QStandardItem *itemCheck = new QStandardItem();
            itemCheck->setCheckable(true);

            QStandardItem *itemName = new QStandardItem(node.name().data());
            itemName->setData(QVariant::fromValue(idMap));
            itemName->setTextAlignment(Qt::AlignCenter);

            QStandardItem *itemIp = new QStandardItem(node.address().data());
            itemIp->setTextAlignment(Qt::AlignCenter);

            QPair<QString, QString> status = m_mapStatus[1];
            QString color = status.second;
            QString state = status.first;
            std::string strCntrCnt = "-/-";
            std::string strCpuPct = "-";
            std::string strMemPct = "-";
            if (node.has_status())
            {
                auto tmp = m_mapStatus[node.status().state()];
                state = tmp.first;
                color = tmp.second;

                auto &status = node.status();
                if (status.has_container_stat())
                    strCntrCnt = std::to_string(status.container_stat().running()) + "/" + std::to_string(status.container_stat().total());

                if (status.has_cpu_stat())
                {
                    char str[128]{};
                    sprintf(str, "%0.1f%%", status.cpu_stat().used() * 100);
                    strCpuPct = std::string(str);
                }

                if (status.has_mem_stat())
                {
                    char str[128]{};
                    sprintf(str, "%0.1f%%", status.mem_stat().used_percentage());
                    strMemPct = std::string(str);
                }
            }

            QStandardItem *itemStatus = new QStandardItem(state);
            itemStatus->setForeground(QBrush(QColor(color)));
            itemStatus->setTextAlignment(Qt::AlignCenter);
            QStandardItem *itemCntrCnt = new QStandardItem(strCntrCnt.data());
            itemCntrCnt->setTextAlignment(Qt::AlignCenter);
            QStandardItem *itemCpu = new QStandardItem(strCpuPct.data());
            itemCpu->setTextAlignment(Qt::AlignCenter);
            QStandardItem *itemMem = new QStandardItem(strMemPct.data());
            itemMem->setTextAlignment(Qt::AlignCenter);
            QStandardItem *itemDisk = new QStandardItem("-");
            itemDisk->setTextAlignment(Qt::AlignCenter);

            setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemStatus << itemIp << itemCntrCnt << itemCpu << itemMem << itemDisk);
            row++;
        }
    }
    else
    {
        setTableDefaultContent("-");
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    }
}

void NodeList::getCreateResult(const QPair<grpc::Status, node::CreateReply> &reply)
{
    KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
    if (reply.first.ok())
    {
        getNodeList();
        return;
    }
    else
    {
        KLOG_DEBUG() << QString::fromStdString(reply.first.error_message());
        MessageDialog::message(tr("Create Node"),
                               tr("Create Node failed!"),
                               tr("Error: ") + reply.first.error_message().data(),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void NodeList::getRemoveResult(const QPair<grpc::Status, node::RemoveReply> &reply)
{
    KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
    if (reply.first.ok())
    {
        getNodeList();
    }
}

void NodeList::onItemClicked(const QModelIndex &index)
{
    if (index.column() == 1)
    {
        KLOG_INFO() << "onItemClicked: " << index.column();
    }
}

void NodeList::initUI()
{
}

void NodeList::initButtons()
{
    QPushButton *btnCreate = new QPushButton(this);
    btnCreate->setText(tr("Create"));
    btnCreate->setObjectName("btnCreate");
    btnCreate->setFixedSize(QSize(78, 32));
    addSingleOperationButton(btnCreate);
    connect(btnCreate, &QPushButton::clicked, this, &NodeList::onCreateNode);

    QPushButton *btnRemove = new QPushButton(this);
    btnRemove->setText(tr("Remove"));
    btnRemove->setObjectName("btnRemove");
    btnRemove->setFixedSize(QSize(78, 32));
    connect(btnRemove, &QPushButton::clicked, this, &NodeList::onRemoveNode);

    addBatchOperationButtons(QList<QPushButton *>() << btnRemove);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void NodeList::initTable()
{
    QStringList tableHHeaderDate = {
        "",
        QString(tr("Node Name")),
        QString(tr("Status")),
        QString(tr("IP")),
        QString(tr("Container Number")),
        "CPU",
        QString(tr("Memory")),
        QString(tr("Disk")),
        QString(tr("Quick Actions")),
    };
    setHeaderSections(tableHHeaderDate);
    setTableColNum(tableHHeaderDate.size());
    QList<int> sortablCol = {1, 3};
    setSortableCol(sortablCol);
    setTableActions(tableHHeaderDate.size() - 1, QStringList() << ":/images/monitor.svg");
    setTableDefaultContent("-");

    connect(this, &NodeList::sigMonitor, this, &NodeList::onMonitor);
    connect(this, &NodeList::sigItemClicked, this, &NodeList::onItemClicked);
}

void NodeList::initNodeConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::listNodeFinished, this, &NodeList::getListResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::createNodeFinished, this, &NodeList::getCreateResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::removeNodeFinished, this, &NodeList::getRemoveResult);
}

void NodeList::getNodeList()
{
    InfoWorker::getInstance().listNode();
}
