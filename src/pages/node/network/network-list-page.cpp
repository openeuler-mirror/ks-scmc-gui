#include "network-list-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QStandardItem>
NetworkListPage::NetworkListPage(NetworkIfsType type, QWidget *parent) : TablePage(parent),
                                                                         m_type(type)
{
    m_objId = InfoWorker::generateId(this);

    if (m_type == NETWORK_IFS_TYPE_VIRT)
    {
        initButtons();
        initVirtIfsTable();
        connect(&InfoWorker::getInstance(), &InfoWorker::createNicFinished, this, &NetworkListPage::getCreateResult);
        connect(&InfoWorker::getInstance(), &InfoWorker::removeNicFinished, this, &NetworkListPage::getRemoveResult);
        connect(&InfoWorker::getInstance(), &InfoWorker::updateNicFinished, this, &NetworkListPage::getUpdateResult);
    }
    else if (m_type == NETWORK_IFS_TYPE_REAL)
    {
        initRealIfsTable();
    }
    connect(&InfoWorker::getInstance(), &InfoWorker::listNetworkFinished, this, &NetworkListPage::getListResult);
}

void NetworkListPage::updateInfo(QString keyword)
{
    clearText();
    if (keyword.isEmpty())
    {
        InfoWorker::getInstance().listNetwork(m_objId, m_nodeId);
    }
}

void NetworkListPage::setNodeId(int64_t nodeId)
{
    m_nodeId = nodeId;
}

void NetworkListPage::onCreateVirtIfs()
{
}

void NetworkListPage::onRemoveVirtIfs()
{
}

void NetworkListPage::getListResult(const QString objId, const QPair<grpc::Status, network::ListReply> &reply)
{
    if (m_objId == objId)
    {
        KLOG_INFO() << "get network list result";
        if (reply.first.ok())
        {
            clearTable();
            int row = 0;
            if (m_type == NETWORK_IFS_TYPE_REAL)
            {
                if (reply.second.real_ifs().size() <= 0)
                {
                    setTableDefaultContent("-");
                    return;
                }
                for (auto ifs : reply.second.real_ifs())
                {
                    auto name = ifs.name();
                    auto ip = ifs.ip_address();
                    auto mask = ifs.ip_mask();
                    auto gateway = ifs.gateway();
                    auto mac = ifs.mac_address();
                    auto status = ifs.is_up();

                    QStandardItem *checkItem = new QStandardItem();
                    QStandardItem *nameItem = new QStandardItem(QString::fromStdString(name));
                    QStandardItem *ipItem = new QStandardItem(QString::fromStdString(ip));
                    QStandardItem *maskItem = new QStandardItem(QString::fromStdString(mask));
                    QStandardItem *gatewayItem = new QStandardItem(QString::fromStdString(gateway));
                    QStandardItem *macItem = new QStandardItem(QString::fromStdString(mac));
                    QStandardItem *statusItem = new QStandardItem(status == true ? tr("Up") : tr("Down"));

                    setTableItems(row, 0, QList<QStandardItem *>() << checkItem << nameItem << ipItem << maskItem << gatewayItem << macItem << statusItem);
                    row++;
                }
            }
            else if (m_type == NETWORK_IFS_TYPE_VIRT)
            {
                setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
                if (reply.second.virtual_ifs().size() <= 0)
                {
                    setTableDefaultContent("-");
                    return;
                }
                for (auto ifs : reply.second.virtual_ifs())
                {
                    auto name = ifs.name();
                    auto subnet = ifs.ip_address() + "/" + ifs.ip_mask() + "/" + std::to_string(ifs.ip_mask_len());
                    auto realName = ifs.bind_real();

                    QStandardItem *checkItem = new QStandardItem();
                    checkItem->setCheckable(true);
                    QStandardItem *nameItem = new QStandardItem(QString::fromStdString(name));
                    QStandardItem *subnetItem = new QStandardItem(QString::fromStdString(subnet));
                    QStandardItem *realNameItem = new QStandardItem(QString::fromStdString(realName));
                    setTableItems(row, 0, QList<QStandardItem *>() << checkItem << nameItem << subnetItem << realNameItem);
                    row++;
                }
            }
        }
        else
        {
            KLOG_INFO() << "getNetworkListResult failed";
        }
    }
}

void NetworkListPage::getCreateResult(const QString objId, const QPair<grpc::Status, network::CreateNicReply> &reply)
{
}

void NetworkListPage::getRemoveResult(const QString objId, const QPair<grpc::Status, network::RemoveNicReply> &reply)
{
}

void NetworkListPage::getUpdateResult(const QString objId, const QPair<grpc::Status, network::UpdateNicReply> &reply)
{
}

void NetworkListPage::initButtons()
{
    QPushButton *btnCreate = new QPushButton(this);
    btnCreate->setText(tr("Create"));
    btnCreate->setObjectName("btnCreate");
    btnCreate->setFixedSize(QSize(78, 32));
    addSingleOperationButton(btnCreate);
    connect(btnCreate, &QPushButton::clicked, this, &NetworkListPage::onCreateVirtIfs);

    QPushButton *btnRemove = new QPushButton(this);
    btnRemove->setText(tr("Remove"));
    btnRemove->setObjectName("btnRemove");
    btnRemove->setFixedSize(QSize(78, 32));
    connect(btnRemove, &QPushButton::clicked, this, &NetworkListPage::onRemoveVirtIfs);

    addBatchOperationButtons(QList<QPushButton *>() << btnRemove);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void NetworkListPage::initConnect()
{
}

void NetworkListPage::initRealIfsTable()
{
    QStringList tableHHeaderDate = {
        "",
        QString(tr("Network interface")),
        "IP",
        QString(tr("Mask")),
        QString(tr("Gateway")),
        "Mac",
        QString(tr("Status"))};
    setHeaderSections(tableHHeaderDate);
    setHeaderCheckable(false);
    setTableDefaultContent("-");
}

void NetworkListPage::initVirtIfsTable()
{
    QStringList tableHHeaderDate = {
        "",
        QString(tr("Network interface")),
        QString(tr("Subnet")),
        QString(tr("Real network interface")),
        QString(tr("Quick Actions"))};
    setHeaderSections(tableHHeaderDate);
    setHeaderCheckable(false);
    setTableDefaultContent("-");
    setTableSingleChoose(true);
    setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>{
                                                     {ACTION_BUTTON_TYPE_EDIT, QPair<QString, QString>{tr("Edit"), ":/images/edit.svg"}},
                                                     {ACTION_BUTTON_TYPE_DELETE, QPair<QString, QString>{tr("Delete"), ":/images/btn-delete-normal.png"}}});
    //    connect(this, &NodeListPage::sigEdit, this, &NodeListPage::onEdit);
    //    connect(this, &NodeListPage::sigItemClicked, this, &NodeListPage::onItemClicked);
    //    connect(this, &NodeListPage::sigItemEntered, this, &NodeListPage::onItemEntered);
}
