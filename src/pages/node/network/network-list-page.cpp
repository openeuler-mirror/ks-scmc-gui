#include "network-list-page.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-titlebar-window.h>
#include <widget-property-helper.h>
#include <QApplication>
#include <QComboBox>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QStandardItem>
#include "message-dialog.h"
#include "notification-manager.h"

#define NAME "name"
#define REAL_IFS "parent"  // 物理网卡
#define SUBNET "subnet"    // 网段
#define GATEWAY "gateway"  // 网关

NetworkListPage::NetworkListPage(NetworkIfsType type, QWidget *parent) : TablePage(parent),
                                                                         m_type(type)
{
    m_objId = InfoWorker::generateId(this);

    if (m_type == NETWORK_IFS_TYPE_VIRT)
    {
        initButtons();
        initVirtIfsTable();
    }
    else if (m_type == NETWORK_IFS_TYPE_REAL)
    {
        initRealIfsTable();
    }

    initConnect();
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
    showOperateDialog(OPERATE_TYPE_CREATE);
}

void NetworkListPage::onRemoveVirtIfs()
{
    auto itemInfo = getCheckedItemInfo(1).at(0);
    QString name = itemInfo[NAME].toString();
    InfoWorker::getInstance().removeNic(m_objId, m_nodeId, name.toStdString());
}

void NetworkListPage::removeVirtIfs(int row)
{
    auto ret = MessageDialog::message(tr("Delete virt interface"),
                                      tr("Are you sure to delete the virt interface?"),
                                      tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                      ":/images/warning.svg",
                                      MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
    if (ret == MessageDialog::StandardButton::Yes)
    {
        auto item = getItem(row, 1);
        auto infoMap = item->data().toMap();
        QString name = infoMap[NAME].toString();
        InfoWorker::getInstance().removeNic(m_objId, m_nodeId, name.toStdString());
    }
}

void NetworkListPage::onEditVirtIfs(int row)
{
    auto item = getItem(row, 1);
    auto infoMap = item->data().toMap();
    QString name = infoMap[NAME].toString();
    QString subnet = infoMap[SUBNET].toString();
    QString realIfs = infoMap[REAL_IFS].toString();

    showOperateDialog(OPERATE_TYPE_EDIT, name, subnet, realIfs);
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
                m_realIfs.clear();
                for (auto ifs : reply.second.real_ifs())
                {
                    m_realIfs.append(QString::fromStdString(ifs.name()));
                }
                QMap<QString, QVariant> infoMap;
                for (auto ifs : reply.second.virtual_ifs())
                {
                    auto name = ifs.name();
                    auto subnet = ifs.ip_address() + "/" + std::to_string(ifs.ip_mask_len());
                    auto realName = ifs.bind_real();
                    auto gateway = ifs.gateway();
                    infoMap.insert(NAME, QString::fromStdString(name));
                    infoMap.insert(REAL_IFS, QString::fromStdString(realName));
                    infoMap.insert(SUBNET, QString::fromStdString(subnet));
                    infoMap.insert(GATEWAY, QString::fromStdString(gateway));

                    QStandardItem *checkItem = new QStandardItem();
                    checkItem->setCheckable(true);
                    QStandardItem *nameItem = new QStandardItem(QString::fromStdString(name));
                    nameItem->setData(QVariant::fromValue(infoMap));
                    QStandardItem *subnetItem = new QStandardItem(QString::fromStdString(subnet));
                    QStandardItem *realNameItem = new QStandardItem(QString::fromStdString(realName));
                    setTableItems(row, 0, QList<QStandardItem *>() << checkItem << nameItem << subnetItem << realNameItem);
                    row++;
                }
            }
        }
        else
        {
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
            KLOG_INFO() << "getNetworkListResult failed";
        }
    }
}

void NetworkListPage::getCreateResult(const QString objId, const QPair<grpc::Status, network::CreateNicReply> &reply)
{
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            NotificationManager::sendNotify(tr("Create virt interface successful!"), "");
            updateInfo();
        }
        else
            NotificationManager::sendNotify(tr("Create virt interface failed!"),
                                            reply.first.error_message().data());
    }
}

void NetworkListPage::getRemoveResult(const QString objId, const QPair<grpc::Status, network::RemoveNicReply> &reply)
{
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            NotificationManager::sendNotify(tr("Remove virt interface successful!"), "");
            updateInfo();
        }
        else
            NotificationManager::sendNotify(tr("Remove virt interface failed!"),
                                            reply.first.error_message().data());
    }
}

void NetworkListPage::getUpdateResult(const QString objId, const QPair<grpc::Status, network::UpdateNicReply> &reply)
{
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            NotificationManager::sendNotify(tr("Update virt interface successful!"), "");
            updateInfo();
        }
        else
            NotificationManager::sendNotify(tr("Update virt interface failed!"),
                                            reply.first.error_message().data());
    }
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
    if (m_type == NETWORK_IFS_TYPE_VIRT)
    {
        connect(&InfoWorker::getInstance(), &InfoWorker::createNicFinished, this, &NetworkListPage::getCreateResult);
        connect(&InfoWorker::getInstance(), &InfoWorker::removeNicFinished, this, &NetworkListPage::getRemoveResult);
        connect(&InfoWorker::getInstance(), &InfoWorker::updateNicFinished, this, &NetworkListPage::getUpdateResult);
    }
    connect(&InfoWorker::getInstance(), &InfoWorker::listNetworkFinished, this, &NetworkListPage::getListResult);
}

KiranTitlebarWindow *NetworkListPage::createOperateDialog(NetworkIfsOperateType type, QString name, QString subnet, QString realIfs)
{
    KiranTitlebarWindow *window = new KiranTitlebarWindow(this);
    window->setAttribute(Qt::WA_DeleteOnClose, true);
    window->setWindowModality(Qt::ApplicationModal);
    window->setTitle(tr("Add virt interface"));
    window->setIcon(QIcon(":/images/logo.png"));
    window->setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint | KiranTitlebarWindow::TitlebarCloseButtonHint);
    window->setResizeable(false);

    //创建内容窗口
    QWidget *content = new QWidget(window);
    QVBoxLayout *mainLayout = new QVBoxLayout(content);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setMargin(0);
    gridLayout->setSpacing(10);

    QLabel *labName = new QLabel(QString("%1 <font color=red>*</font>").arg(tr("Name:")), content);
    QLineEdit *lineEditName = new QLineEdit(content);
    lineEditName->setFixedHeight(36);
    if (!name.isEmpty())
        lineEditName->setText(name);

    if (type == OPERATE_TYPE_EDIT)
        lineEditName->setDisabled(true);

    QLabel *labSubnet = new QLabel(tr("Subnet:"), content);
    QLineEdit *lineEditSubnet = new QLineEdit(content);
    lineEditSubnet->setFixedHeight(36);
    lineEditSubnet->setPlaceholderText("xxx.xxx.xxx.xxx/xx");
    if (!subnet.isEmpty())
        lineEditSubnet->setText(subnet);

    QRegExp regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?/{1})([0-9]|[12][0-9]|3[0-2])\\b");
    QRegExpValidator *validator = new QRegExpValidator(regExp, this);
    lineEditSubnet->setValidator(validator);

    QLabel *labBindRealIfs = new QLabel(tr("Bind real interface:"), content);
    QComboBox *cbBindRealIfs = new QComboBox(content);
    cbBindRealIfs->setFixedHeight(36);
    if (!m_realIfs.isEmpty())
    {
        cbBindRealIfs->addItems(m_realIfs);
        if (!realIfs.isEmpty())
        {
            cbBindRealIfs->setCurrentText(realIfs);
        }
    }

    gridLayout->addWidget(labName, 0, 0);
    gridLayout->addWidget(lineEditName, 0, 1);
    gridLayout->addWidget(labSubnet, 1, 0);
    gridLayout->addWidget(lineEditSubnet, 1, 1);
    gridLayout->addWidget(labBindRealIfs, 2, 0);
    gridLayout->addWidget(cbBindRealIfs, 2, 1);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);
    btnLayout->setSpacing(20);

    QPushButton *btnConfirm = new QPushButton(tr("Confirm"), content);
    btnConfirm->setFixedSize(100, 40);
    Kiran::WidgetPropertyHelper::setButtonType(btnConfirm, Kiran::BUTTON_Default);
    connect(btnConfirm, &QPushButton::clicked,
            [=] {
                if (type == OPERATE_TYPE_CREATE)
                {
                    network::CreateNicRequest req;
                    req.set_node_id(m_nodeId);
                    req.set_name(lineEditName->text().toStdString());
                    req.set_parent(cbBindRealIfs->currentText().toStdString());
                    req.set_subnet(lineEditSubnet->text().toStdString());
                    InfoWorker::getInstance().createNic(m_objId, req);
                }
                else
                {
                    network::UpdateNicRequest req;
                    req.set_node_id(m_nodeId);
                    req.set_name(lineEditName->text().toStdString());
                    req.set_parent(cbBindRealIfs->currentText().toStdString());
                    req.set_subnet(lineEditSubnet->text().toStdString());
                    InfoWorker::getInstance().updateNic(m_objId, req);
                }
                window->close();
            });

    QPushButton *btnCancel = new QPushButton(tr("Cancel"), content);
    btnCancel->setFixedSize(100, 40);
    connect(btnCancel, &QPushButton::clicked, window, &KiranTitlebarWindow::close);

    btnLayout->addStretch();
    btnLayout->addWidget(btnConfirm);
    btnLayout->addWidget(btnCancel);

    mainLayout->addLayout(gridLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    window->setWindowContentWidget(content);
    window->resize(500, 400);

    return window;
}

void NetworkListPage::showOperateDialog(NetworkIfsOperateType type, QString name, QString subnet, QString realIfs)
{
    auto window = createOperateDialog(type, name, subnet, realIfs);
    if (window)
    {
        connect(window, &KiranTitlebarWindow::destroyed,
                [=] {
                    KLOG_INFO() << " export backup window destroy";
                    window->deleteLater();
                    //window = nullptr;
                });
    }

    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    window->move(screenGeometry.x() + (screenGeometry.width() - window->width()) / 2,
                 screenGeometry.y() + (screenGeometry.height() - window->height()) / 2);
    window->raise();
    window->show();
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
    connect(this, &NetworkListPage::sigEdit, this, &NetworkListPage::onEditVirtIfs);
    connect(this, &NetworkListPage::sigDelete, this, &NetworkListPage::removeVirtIfs);
}
