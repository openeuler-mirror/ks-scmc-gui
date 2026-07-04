#include "node-list-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include "common/message-dialog.h"
#include "def.h"
#include "node-operate-dialog.h"
#include "rpc.h"

using namespace grpc;

#define ACTION_COL 1
NodeListPage::NodeListPage(QWidget *parent) : TablePage(parent),
                                              m_nodeAddition(nullptr),
                                              m_nodeUpdate(nullptr),
                                              m_timer(nullptr)
{
    m_objId = InfoWorker::generateId(this);
    m_mapStatus.insert(0, QPair<QString, QString>(tr("Offline"), "red"));
    m_mapStatus.insert(1, QPair<QString, QString>(tr("Unknown"), "black"));
    m_mapStatus.insert(10, QPair<QString, QString>(tr("Online"), "green"));
    initButtons();
    initTable();
    initNodeConnect();

    m_timer = new QTimer(this);
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [this] {
        updateInfo();
    });
}

NodeListPage::~NodeListPage()
{
    if (m_nodeAddition)
    {
        delete m_nodeAddition;
        m_nodeAddition = nullptr;
    }
    if (m_nodeUpdate)
    {
        delete m_nodeUpdate;
        m_nodeUpdate = nullptr;
    }
}

void NodeListPage::updateInfo(QString keyword)
{
    KLOG_INFO() << "NodeListPage updateInfo, keyword:" << keyword;
    if (keyword == "exitTimedRefresh")
    {
        timedRefresh(false);
        clearCheckState();
        return;
    }

    clearText();
    if (keyword.isEmpty())
    {
        //initNodeConnect();
        getNodeList();
        timedRefresh(true);
    }
}

void NodeListPage::onCreateNode()
{
    if (!m_nodeAddition)
    {
        m_nodeAddition = new NodeOperateDialog(NODE_OPERATE_TYPE_CREATE);
        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_nodeAddition->move(screenGeometry.x() + (screenGeometry.width() - m_nodeAddition->width()) / 2,
                             screenGeometry.y() + (screenGeometry.height() - m_nodeAddition->height()) / 2);
        m_nodeAddition->show();

        connect(m_nodeAddition, &NodeOperateDialog::sigSave, this, &NodeListPage::onSaveSlot);
        connect(m_nodeAddition, &NodeOperateDialog::destroyed,
                [=] {
                    KLOG_INFO() << " m_nodeAdditiong destroy";
                    m_nodeAddition->deleteLater();
                    m_nodeAddition = nullptr;
                });
    }
}

void NodeListPage::onRemoveNode()
{
    KLOG_INFO() << "onRemoveNode";
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);
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
            InfoWorker::getInstance().removeNode(m_objId, node_ids);
        }
        else
            KLOG_INFO() << "cancel";
    }
}

void NodeListPage::onEdit(int row)
{
    KLOG_INFO() << row;
    auto item = getItem(row, 1);
    QMap<QString, QVariant> nodeInfo = item->data().toMap();

    if (!m_nodeUpdate)
    {
        m_nodeUpdate = new NodeOperateDialog(NODE_OPERATE_TYPE_EDIT);
        m_nodeUpdate->setNodeInfo(nodeInfo.value(NODE_ID).toInt(),
                                  nodeInfo.value(NODE_NAME).toString(),
                                  nodeInfo.value(NODE_ADDRESS).toString(),
                                  nodeInfo.value(NODE_COMMENT).toString());
        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_nodeUpdate->move(screenGeometry.x() + (screenGeometry.width() - m_nodeUpdate->width()) / 2,
                           screenGeometry.y() + (screenGeometry.height() - m_nodeUpdate->height()) / 2);
        m_nodeUpdate->show();

        connect(m_nodeUpdate, &NodeOperateDialog::sigSave, this, &NodeListPage::onSaveSlot);
        connect(m_nodeUpdate, &NodeOperateDialog::destroyed,
                [=] {
                    KLOG_INFO() << " m_nodeUpdate destroy";
                    m_nodeUpdate->deleteLater();
                    m_nodeUpdate = nullptr;
                });
    }
}

void NodeListPage::onSaveSlot(QMap<QString, QString> Info)
{
    KLOG_INFO() << "name" << Info[NODE_NAME] << "ip" << Info[NODE_ADDRESS] << Info[NODE_COMMENT];
    NodeOperateDialog *dialog = qobject_cast<NodeOperateDialog *>(sender());
    auto type = dialog->getType();
    auto nodeId = dialog->getNodeId();
    if (type == NODE_OPERATE_TYPE_CREATE)
    {
        node::CreateRequest request;
        request.set_name(Info[NODE_NAME].toStdString());
        request.set_address(Info[NODE_ADDRESS].toStdString());
        request.set_comment(Info[NODE_COMMENT].toStdString());
        InfoWorker::getInstance().createNode(m_objId, request);
    }
    else if (type == NODE_OPERATE_TYPE_EDIT)
    {
        node::UpdateRequest request;
        request.set_node_id(nodeId);
        request.set_name(Info[NODE_NAME].toStdString());
        request.set_comment(Info[NODE_COMMENT].toStdString());
        InfoWorker::getInstance().updateNode(m_objId, request);
    }
}

void NodeListPage::getListResult(const QString objId, const QPair<grpc::Status, node::ListReply> &reply)
{
    KLOG_INFO() << "getNodeListResult" << m_objId << objId;
    if (m_objId == objId)
    {
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
        if (reply.first.ok())
        {
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
            clearTable();
            int size = reply.second.nodes_size();
            if (size <= 0)
            {
                setTableDefaultContent("-");
                setHeaderCheckable(false);
                return;
            }
            setHeaderCheckable(true);
            int row = 0;
            QMap<QString, QVariant> idMap;
            for (auto node : reply.second.nodes())
            {
                KLOG_INFO() << "nodeid:" << node.id();
                qint64 nodeId = node.id();
                idMap.insert(NODE_ID, nodeId);
                idMap.insert(NODE_NAME, node.name().data());
                idMap.insert(NODE_COMMENT, node.comment().data());
                idMap.insert(NODE_ADDRESS, node.address().data());

                QStandardItem *itemCheck = new QStandardItem();
                itemCheck->setCheckable(true);

                QStandardItem *itemName = new QStandardItem(node.name().data());
                itemName->setData(QVariant::fromValue(idMap));
                itemName->setTextAlignment(Qt::AlignCenter);
                itemName->setForeground(QBrush(QColor(46, 179, 255)));

                QStandardItem *itemIp = new QStandardItem(node.address().data());
                itemIp->setTextAlignment(Qt::AlignCenter);

                QPair<QString, QString> status = m_mapStatus[1];
                QString color = status.second;
                QString state = status.first;
                std::string strCntrCnt = "-/-";
                QString strCpuPct = "-";
                std::string strMemPct = "-";
                std::string strDiskSize = "-";
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
                        strCpuPct = QString("%1 (%2 %3)").arg(str).arg(status.cpu_stat().total()).arg(tr("core"));
                    }

                    if (status.has_mem_stat())
                    {
                        char str[128]{};
                        sprintf(str, "%0.1f%%", status.mem_stat().used_percentage());
                        strMemPct = std::string(str);
                    }

                    if (status.has_disk_stat())
                    {
                        char str[128]{};
                        sprintf(str, "%0.1f%%", status.disk_stat().used_percentage());
                        strDiskSize = std::string(str);
                    }
                }
                QStandardItem *itemStatus = new QStandardItem(state);
                itemStatus->setForeground(QBrush(QColor(color)));
                itemStatus->setTextAlignment(Qt::AlignCenter);
                QStandardItem *itemCntrCnt = new QStandardItem(strCntrCnt.data());
                itemCntrCnt->setTextAlignment(Qt::AlignCenter);
                QStandardItem *itemCpu = new QStandardItem(strCpuPct);
                itemCpu->setTextAlignment(Qt::AlignCenter);
                QStandardItem *itemMem = new QStandardItem(strMemPct.data());
                itemMem->setTextAlignment(Qt::AlignCenter);
                QStandardItem *itemDisk = new QStandardItem(strDiskSize.data());
                itemDisk->setTextAlignment(Qt::AlignCenter);
                QStandardItem *itemWarning = new QStandardItem(QString::number(node.unread_warn()));
                itemWarning->setTextAlignment(Qt::AlignCenter);
                QStandardItem *itemComment = new QStandardItem(node.comment().data());
                itemComment->setTextAlignment(Qt::AlignCenter);

                setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemStatus << itemIp << itemCntrCnt << itemCpu << itemMem << itemDisk << itemWarning << itemComment);
                row++;
            }
        }
        else
        {
            if (reply.first.error_code() == PERMISSION_DENIED)
                setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
            else
                setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
            setTableDefaultContent("-");
            setHeaderCheckable(false);
        }
    }
}

void NodeListPage::getCreateResult(const QString objId, const QPair<grpc::Status, node::CreateReply> &reply)
{
    KLOG_INFO() << "getCreateResult" << m_objId << objId;
    if (m_objId == objId)
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
                                   tr("Create node failed!"),
                                   tr("Error: ") + reply.first.error_message().data(),
                                   ":/images/error.svg",
                                   MessageDialog::StandardButton::Ok);
        }
    }
}

void NodeListPage::getRemoveResult(const QString objId, const QPair<grpc::Status, node::RemoveReply> &reply)
{
    KLOG_INFO() << "getRemoveResult" << m_objId << objId;
    if (m_objId == objId)
    {
        KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
        if (reply.first.ok())
        {
            getNodeList();
        }
        else
        {
            MessageDialog::message(tr("Remove Node"),
                                   tr("Remove node failed!"),
                                   tr("Error: %1").arg(reply.first.error_message().data()),
                                   tr(":/images/error.svg"),
                                   MessageDialog::StandardButton::Ok);
        }
    }
}

void NodeListPage::getUpdateResult(const QString objId, const QPair<grpc::Status, node::UpdateReply> &reply)
{
    KLOG_INFO() << "getUpdateResult" << m_objId << objId;
    if (m_objId == objId)
    {
        KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
        if (reply.first.ok())
        {
            getNodeList();
        }
        else
        {
            MessageDialog::message(tr("Update Node"),
                                   tr("Update node failed!"),
                                   tr("Error: %1").arg(reply.first.error_message().data()),
                                   tr(":/images/error.svg"),
                                   MessageDialog::StandardButton::Ok);
        }
    }
}

void NodeListPage::onItemClicked(const QModelIndex &index)
{
    KLOG_INFO() << "onItemClicked: " << index.column();
    if (index.column() == 1)
    {
        auto item = getItem(index.row(), index.column());
        auto itemInfo = item->data().toMap();
        qint64 nodeId = itemInfo[NODE_ID].toInt();
        KLOG_INFO() << "node " << nodeId << "clicked";

        emit sigNodeNameClicked(nodeId);
    }
}

void NodeListPage::onItemEntered(const QModelIndex &index)
{
    if (index.column() == 1)
        this->setCursor(Qt::PointingHandCursor);
    else
        this->setCursor(Qt::ArrowCursor);
}

void NodeListPage::initButtons()
{
    QPushButton *btnCreate = new QPushButton(this);
    btnCreate->setText(tr("Create"));
    btnCreate->setObjectName("btnCreate");
    btnCreate->setFixedSize(QSize(78, 32));
    addSingleOperationButton(btnCreate);
    connect(btnCreate, &QPushButton::clicked, this, &NodeListPage::onCreateNode);

    QPushButton *btnRemove = new QPushButton(this);
    btnRemove->setText(tr("Remove"));
    btnRemove->setObjectName("btnRemove");
    btnRemove->setFixedSize(QSize(78, 32));
    connect(btnRemove, &QPushButton::clicked, this, &NodeListPage::onRemoveNode);

    addBatchOperationButtons(QList<QPushButton *>() << btnRemove);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void NodeListPage::initTable()
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
        QString(tr("Warning Number")),
        QString(tr("Comment")),
        QString(tr("Quick Actions"))};
    setHeaderSections(tableHHeaderDate);
    QList<int> sortablCol = {1, 2};
    setSortableCol(sortablCol);
    setTableDefaultContent("-");

    setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>{
                                                     {ACTION_BUTTON_TYPE_EDIT, QPair<QString, QString>{tr("Edit"), ":/images/edit.svg"}}});

    connect(this, &NodeListPage::sigEdit, this, &NodeListPage::onEdit);
    connect(this, &NodeListPage::sigItemClicked, this, &NodeListPage::onItemClicked);
    connect(this, &NodeListPage::sigItemEntered, this, &NodeListPage::onItemEntered);
}

void NodeListPage::initNodeConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::listNodeFinished, this, &NodeListPage::getListResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::createNodeFinished, this, &NodeListPage::getCreateResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::removeNodeFinished, this, &NodeListPage::getRemoveResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::updateNodeFinished, this, &NodeListPage::getUpdateResult);
}

void NodeListPage::getNodeList()
{
    InfoWorker::getInstance().listNode(m_objId);
}

void NodeListPage::timedRefresh(bool start)
{
    KLOG_INFO() << start;
    if (start)
        m_timer->start(60000);
    else
    {
        m_timer->stop();
    }
}
