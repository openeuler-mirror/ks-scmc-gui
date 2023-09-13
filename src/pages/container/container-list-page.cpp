#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QComboBox>
#include <QDateTime>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QProcess>
#include <QStandardItem>
#include <QTimer>

#include "common/message-dialog.h"
#include "common/monitor-dialog.h"
#include "container-list-page.h"
#include "container-setting.h"

#define NODE_ID "node id"
#define NODE_ADDRESS "node address"
#define CONTAINER_ID "container id"
#define CONTAINER_NAME "container name"

// TODO read from config file.
const QString TERMINAL_CMD = "mate-terminal -e";
const QString BASHRC_FILE = "/etc/ks-scmc/graphic_rc";

ContainerListPage::ContainerListPage(QWidget *parent)
    : TablePage(parent),
      m_createCTSetting(nullptr),
      m_editCTSetting(nullptr),
      m_monitor(nullptr),
      m_timer(nullptr)
{
    initButtons();
    //初始化表格
    initTable();
    initConnect();

    m_statusMap.insert("running", QPair<QString, QString>(tr("Online"), "#00921b"));
    m_statusMap.insert("exited", QPair<QString, QString>(tr("Offline"), "#d30000"));
    m_statusMap.insert("created", QPair<QString, QString>(tr("Created"), "#00921b"));

    //    m_timer = new QTimer(this);
    //    m_timer->start(10000);
    //    connect(m_timer, &QTimer::timeout,
    //            [this] {
    //                InfoWorker::getInstance().listNode();
    //            });

    connect(this, &ContainerListPage::sigTerminal, this, &ContainerListPage::onTerminal);
}

ContainerListPage::~ContainerListPage()
{
    if (m_createCTSetting)
    {
        delete m_createCTSetting;
        m_createCTSetting = nullptr;
    }
    if (m_editCTSetting)
    {
        delete m_editCTSetting;
        m_editCTSetting = nullptr;
    }
    if (m_monitor)
    {
        delete m_monitor;
        m_monitor = nullptr;
    }
}

void ContainerListPage::onBtnRun()
{
    KLOG_INFO() << "onBtnRun";
    std::map<int64_t, std::vector<std::string>> ids;
    getCheckedItemsId(ids);
    if (!ids.empty())
    {
        setBusy(true);
        InfoWorker::getInstance().startContainer(ids);
    }
}

void ContainerListPage::onBtnRun(QModelIndex index)
{
    KLOG_INFO() << "onBtnRun:" << index.row();
    std::map<int64_t, std::vector<std::string>> ids;
    getItemId(index.row(), ids);
    setBusy(true);
    InfoWorker::getInstance().startContainer(ids);
}

void ContainerListPage::onBtnStop()
{
    KLOG_INFO() << "onBtnStop";
    std::map<int64_t, std::vector<std::string>> ids;
    getCheckedItemsId(ids);
    if (!ids.empty())
    {
        setBusy(true);
        InfoWorker::getInstance().stopContainer(ids);
    }
}

void ContainerListPage::onBtnStop(QModelIndex index)
{
    KLOG_INFO() << "onBtnStop:" << index.row();
    std::map<int64_t, std::vector<std::string>> ids;
    getItemId(index.row(), ids);
    setBusy(true);
    InfoWorker::getInstance().stopContainer(ids);
}

void ContainerListPage::onBtnRestart()
{
    KLOG_INFO() << "onBtnRestart";
    std::map<int64_t, std::vector<std::string>> ids;
    getCheckedItemsId(ids);
    if (ids.empty())
    {
        setBusy(true);
        InfoWorker::getInstance().restartContainer(ids);
    }
}

void ContainerListPage::onBtnRestart(QModelIndex index)
{
    KLOG_INFO() << "onBtnRestart:" << index.row();
    std::map<int64_t, std::vector<std::string>> ids;
    getItemId(index.row(), ids);
    setBusy(true);
    InfoWorker::getInstance().restartContainer(ids);
}

void ContainerListPage::onBtnDelete()
{
    KLOG_INFO() << "onBtnDelete";
    std::map<int64_t, std::vector<std::string>> ids;
    getCheckedItemsId(ids);
    if (!ids.empty())
    {
        auto ret = MessageDialog::message(tr("Delete Container"),
                                          tr("Are you sure you want to delete the container?"),
                                          tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                          ":/images/warning.svg",
                                          MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
        if (ret == MessageDialog::StandardButton::Yes)
        {
            setBusy(true);
            InfoWorker::getInstance().removeContainer(ids);
        }
        else
            KLOG_INFO() << "cancel";
    }
}

void ContainerListPage::onActCreate()
{
    KLOG_INFO() << "onActCreate";
    if (!m_createCTSetting)
    {
        m_createCTSetting = new ContainerSetting(CONTAINER_SETTING_TYPE_CONTAINER_CREATE);

        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_createCTSetting->move(screenGeometry.x() + (screenGeometry.width() - m_createCTSetting->width()) / 2,
                                screenGeometry.y() + (screenGeometry.height() - m_createCTSetting->height()) / 2);

        m_createCTSetting->show();
        connect(m_createCTSetting, &ContainerSetting::destroyed,
                [=] {
                    KLOG_INFO() << "create container setting destroy";
                    m_createCTSetting->deleteLater();
                    m_createCTSetting = nullptr;
                });
        connect(m_createCTSetting, &ContainerSetting::sigUpdateContainer,
                [=] {
                    updateInfo();
                });
    }
}

void ContainerListPage::onActCopyConfig()
{
    KLOG_INFO() << "onCopyConfig";
}

void ContainerListPage::onActBatchUpdate()
{
    KLOG_INFO() << "onBatchUpdate";
}

void ContainerListPage::onActBatchEdit()
{
    KLOG_INFO() << "onBatchEdit";
}

void ContainerListPage::onActBackup()
{
    KLOG_INFO() << "onBackup";
}

void ContainerListPage::onMonitor(int row)
{
    KLOG_INFO() << "ContainerListPage::onMonitor" << row;
    //setBusy(true);

    auto item = getItem(row, 1);
    QMap<QString, QVariant> idMap = item->data().value<QMap<QString, QVariant>>();

    int nodeId = idMap.value(NODE_ID).toInt();
    std::string containerId = idMap.value(CONTAINER_ID).toString().toStdString();
    KLOG_INFO() << nodeId << containerId.data();

    if (!m_monitor)
    {
        m_monitor = new MonitorDialog(nodeId, containerId);
        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_monitor->resize(QSize(1650, 832));
        KLOG_INFO() << m_monitor->width() << m_monitor->height();
        m_monitor->move(screenGeometry.x() + (screenGeometry.width() - m_monitor->width()) / 2,
                        screenGeometry.y() + (screenGeometry.height() - m_monitor->height()) / 2);

        m_monitor->show();

        connect(m_monitor, &MonitorDialog::destroyed,
                [=] {
                    KLOG_INFO() << " monitor destroy";
                    m_monitor->deleteLater();
                    m_monitor = nullptr;
                });
    }
}

void ContainerListPage::onEdit(int row)
{
    KLOG_INFO() << row;
    if (!m_editCTSetting)
    {
        m_editCTSetting = new ContainerSetting(CONTAINER_SETTING_TYPE_CONTAINER_EDIT);
        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_editCTSetting->move(screenGeometry.x() + (screenGeometry.width() - m_editCTSetting->width()) / 2,
                              screenGeometry.y() + (screenGeometry.height() - m_editCTSetting->height()) / 2);
        m_editCTSetting->show();

        auto item = getItem(row, 1);
        QPair<int64_t, QString> ids = {item->data().toMap().value(NODE_ID).toInt(),
                                       item->data().toMap().value(CONTAINER_ID).toString()};
        m_editCTSetting->setContainerNodeIds(ids);
        getContainerInspect(item->data().toMap());
        connect(m_editCTSetting, &ContainerSetting::destroyed,
                [=] {
                    KLOG_INFO() << " edit container setting destroy";
                    m_editCTSetting->deleteLater();
                    m_editCTSetting = nullptr;
                });
        connect(m_editCTSetting, &ContainerSetting::sigUpdateContainer,
                [=] {
                    updateInfo();
                });
    }
}

void ContainerListPage::onTerminal(int row)
{
    auto infoMap = getItem(row, 1)->data().value<QMap<QString, QVariant>>();
    auto nodeAddr = infoMap.value(NODE_ADDRESS).toString();
    auto containerName = infoMap.value(CONTAINER_NAME).toString();
    auto cmd = QString("%1 \"ssh -Xt root@%2 CONTAINER_NAME=%3 bash --rcfile %4\"")
                   .arg(TERMINAL_CMD, nodeAddr, containerName, BASHRC_FILE);

    KLOG_INFO() << cmd;
    QProcess proc;
    proc.startDetached(cmd);
}

void ContainerListPage::getNodeListResult(const QPair<grpc::Status, node::ListReply> &reply)
{
    KLOG_INFO() << "getNodeListResult";
    setBusy(false);
    if (reply.first.ok())
    {
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
        setHeaderCheckable(true);
        m_vecNodeId.clear();
        for (auto n : reply.second.nodes())
        {
            KLOG_INFO() << n.id();
            m_vecNodeId.push_back(n.id());
        }
        if (!m_vecNodeId.empty())
        {
            setBusy(true);
            InfoWorker::getInstance().listContainer(m_vecNodeId, true);
        }
    }
    else
    {
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
        setTableDefaultContent("-");
        setHeaderCheckable(false);
    }
}

void ContainerListPage::getContainerListResult(const QPair<grpc::Status, container::ListReply> &reply)
{
    setBusy(false);
    KLOG_INFO() << "getContainerListResult";
    if (reply.first.ok())
    {
        int size = reply.second.containers_size();
        KLOG_INFO() << "container size:" << size;
        if (size < 1)
            return;

        clearTable();
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
        setHeaderCheckable(true);
        int row = 0;
        QMap<QString, QVariant> infoMap;
        for (auto i : reply.second.containers())
        {
            qint64 nodeId = i.node_id();
            infoMap.insert(NODE_ID, nodeId);
            infoMap.insert(CONTAINER_ID, i.info().id().data());
            infoMap.insert(CONTAINER_NAME, i.info().name().data());
            infoMap.insert(NODE_ADDRESS, i.node_address().data());

            QStandardItem *itemCheck = new QStandardItem();
            itemCheck->setCheckable(true);

            QStandardItem *itemName = new QStandardItem(i.info().name().data());
            itemName->setData(QVariant::fromValue(infoMap));

            auto status = m_statusMap[i.info().state().data()];
            QStandardItem *itemStatus = new QStandardItem(status.first);
            itemStatus->setForeground(QBrush(QColor(status.second)));

            QStandardItem *itemImage = new QStandardItem(i.info().image().data());

            QStandardItem *itemNodeAddress = new QStandardItem(i.node_address().data());

            std::string strCpuPct = "-";
            std::string strMemPct = "-";
            std::string strDiskPct = "-";
            std::string strOnlineTime = "-";

            if (i.info().has_resource_stat())
            {
                if (i.info().resource_stat().has_cpu_stat())
                {
                    char str[128]{};
                    sprintf(str, "%0.1f%%", i.info().resource_stat().cpu_stat().core_used() * 100);
                    strCpuPct = std::string(str);
                }

                if (i.info().resource_stat().has_mem_stat())
                {
                    char str[128]{};
                    sprintf(str, "%0.1fMB", i.info().resource_stat().mem_stat().used());
                    strMemPct = std::string(str);
                }

                if (i.info().resource_stat().has_disk_stat())
                {
                    char str[128]{};
                    sprintf(str, "%0.0fMB", i.info().resource_stat().disk_stat().used());
                    strDiskPct = std::string(str);
                }
            }

            auto dt = QDateTime::fromSecsSinceEpoch(i.info().created());

            QStandardItem *itemCpu = new QStandardItem(strCpuPct.data());
            QStandardItem *itemMem = new QStandardItem(strMemPct.data());
            QStandardItem *itemDisk = new QStandardItem(strDiskPct.data());
            QStandardItem *onlineTime = new QStandardItem(dt.toString("yyyy/MM/dd hh:mm:ss"));


            setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemStatus << itemImage << itemNodeAddress << itemCpu << itemMem << itemDisk << onlineTime);

            row++;
        }
    }
    else
    {
        setTableDefaultContent("-");
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
        setHeaderCheckable(false);
    }
}

void ContainerListPage::getContainerStartResult(const QPair<grpc::Status, container::StartReply> &reply)
{
    setBusy(false);
    KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
    if (reply.first.ok())
    {
        getContainerList();
        return;
    }
}

void ContainerListPage::getContainerStopResult(const QPair<grpc::Status, container::StopReply> &reply)
{
    setBusy(false);
    KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
    if (reply.first.ok())
    {
        KLOG_INFO() << "stop surccessful";
        getContainerList();
        return;
    }
    else
    {
        KLOG_INFO() << "stop failed";
    }
}

void ContainerListPage::getContainerRestartResult(const QPair<grpc::Status, container::RestartReply> &reply)
{
    setBusy(false);
    KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
    if (reply.first.ok())
    {
        getContainerList();
        return;
    }
}

void ContainerListPage::getContainerRemoveResult(const QPair<grpc::Status, container::RemoveReply> &reply)
{
    setBusy(false);
    KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
    if (reply.first.ok())
    {
        getContainerList();
        return;
    }
}

void ContainerListPage::initButtons()
{
    //创建按钮及菜单
    QPushButton *btnCreate = new QPushButton(this);
    btnCreate->setText(tr("Create"));
    btnCreate->setObjectName("btnCreate");
    btnCreate->setFixedSize(QSize(78, 32));
    addSingleOperationButton(btnCreate);

    QMenu *btnCreateMenu = new QMenu(btnCreate);
    btnCreateMenu->setObjectName("btnCreateMenu");
    QAction *create = btnCreateMenu->addAction(tr("Create container"));
    QAction *createFromTemplate = btnCreateMenu->addAction(tr("Create container from template"));
    btnCreate->setMenu(btnCreateMenu);
    connect(create, &QAction::triggered, this, &ContainerListPage::onActCreate);
    connect(createFromTemplate, &QAction::triggered, this, &ContainerListPage::onActCopyConfig);

    //其他按钮及菜单
    const QMap<int, QString> btnNameMap = {
        {OPERATION_BUTTOM_CONTAINER_LIST_RUN, tr("Run")},
        {OPERATION_BUTTOM_CONTAINER_LIST_STOP, tr("Stop")},
        {OPERATION_BUTTOM_CONTAINER_LIST_RESTART, tr("Restart")},
        {OPERATION_BUTTOM_CONTAINER_LIST_DELETE, tr("Delete")}};

    for (auto iter = btnNameMap.begin(); iter != btnNameMap.end(); iter++)
    {
        QString name = iter.value();
        QPushButton *btn = new QPushButton(this);
        if (name == tr("Delete"))
        {
            btn->setObjectName("btn");
            btn->setStyleSheet("#btn{background-color:#ff4b4b;"
                               "border:none;"
                               "border-radius: 4px;"
                               "color:#ffffff;}"
                               "#btn:hover{ background-color:#ff6c6c;}"
                               "#btn:focus{outline:none;}"
                               "#btn:disabled{color:#919191;background:#393939;}");
        }
        else
        {
            btn->setObjectName("btn");
            btn->setStyleSheet("#btn{background-color:#2eb3ff;"
                               "border:none;"
                               "border-radius: 4px;"
                               "color:#ffffff;}"
                               "#btn:hover{ background-color:#77ceff;}"
                               "#btn:focus{outline:none;}"
                               "#btn:disabled{color:#919191;background:#393939;}");
        }
        btn->setText(name);
        btn->setFixedSize(QSize(78, 32));
        m_batchOpBtnMap.insert(iter.key(), btn);
    }

    connect(m_batchOpBtnMap[OPERATION_BUTTOM_CONTAINER_LIST_RUN], SIGNAL(clicked()), this, SLOT(onBtnRun()));
    connect(m_batchOpBtnMap[OPERATION_BUTTOM_CONTAINER_LIST_STOP], SIGNAL(clicked()), this, SLOT(onBtnStop()));
    connect(m_batchOpBtnMap[OPERATION_BUTTOM_CONTAINER_LIST_RESTART], SIGNAL(clicked()), this, SLOT(onBtnRestart()));
    connect(m_batchOpBtnMap[OPERATION_BUTTOM_CONTAINER_LIST_DELETE], &QPushButton::clicked, this, &ContainerListPage::onBtnDelete);

    connect(this, SIGNAL(sigRun(QModelIndex)), this, SLOT(onBtnRun(QModelIndex)));
    connect(this, SIGNAL(sigStop(QModelIndex)), this, SLOT(onBtnStop(QModelIndex)));
    connect(this, SIGNAL(sigRestart(QModelIndex)), this, SLOT(onBtnRestart(QModelIndex)));

    addBatchOperationButtons(m_batchOpBtnMap.values());
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void ContainerListPage::initTable()
{
    QStringList tableHHeaderDate = {
        "",
        QString(tr("Container Name")),
        QString(tr("Status")),
        QString(tr("Image")),
        QString(tr("Node")),
        "CPU",
        QString(tr("Memory")),
        QString(tr("Disk")),
        QString(tr("Online Time")),
        QString(tr("Quick Actions"))};
    setHeaderSections(tableHHeaderDate);
    QList<int> sortablCol = {1, 3};
    setSortableCol(sortablCol);
    setTableActions(tableHHeaderDate.size() - 1, QStringList() << ":/images/monitor.svg"
                                                               << ":/images/edit.svg"
                                                               << ":/images/terminal.svg"
                                                               << ":/images/more.svg");

    setTableDefaultContent("-");

    connect(this, &ContainerListPage::sigMonitor, this, &ContainerListPage::onMonitor);
    connect(this, &ContainerListPage::sigEdit, this, &ContainerListPage::onEdit);
}

void ContainerListPage::initConnect()
{
    //connect(&InfoWorker::getInstance(), &InfoWorker::listNodeFinished, this, &ContainerListPage::getNodeListResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::listContainerFinished, this, &ContainerListPage::getContainerListResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::startContainerFinished, this, &ContainerListPage::getContainerStartResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::stopContainerFinished, this, &ContainerListPage::getContainerStopResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::restartContainerFinished, this, &ContainerListPage::getContainerRestartResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::removeContainerFinished, this, &ContainerListPage::getContainerRemoveResult);
}

void ContainerListPage::getContainerList(qint64 nodeId)
{
    if (nodeId < 0)
    {
        setBusy(true);
        InfoWorker::getInstance().listNode();
    }
    else
    {
        std::vector<int64_t> vecNodeId;
        vecNodeId.push_back(nodeId);
        connect(&InfoWorker::getInstance(), &InfoWorker::listContainerFinished, this, &ContainerListPage::getContainerListResult);
        InfoWorker::getInstance().listContainer(vecNodeId, true);
    }
}

void ContainerListPage::getContainerInspect(QMap<QString, QVariant> itemData)
{
    //    setBusy(true);
    InfoWorker::getInstance().containerInspect(itemData.value(NODE_ID).toInt(), itemData.value(CONTAINER_ID).toString().toStdString());
}

void ContainerListPage::getCheckedItemsId(std::map<int64_t, std::vector<std::string>> &ids)
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(0);
    int64_t node_id{};

    foreach (auto idMap, info)
    {
        KLOG_INFO() << idMap.value(NODE_ID).toInt();
        KLOG_INFO() << idMap.value(CONTAINER_ID).toString();

        node_id = idMap.value(NODE_ID).toInt();
        std::map<int64_t, std::vector<std::string>>::iterator iter = ids.find(node_id);
        if (iter == ids.end())
        {
            std::vector<std::string> container_ids;
            container_ids.push_back(idMap.value(CONTAINER_ID).toString().toStdString());
            ids.insert(std::pair<int64_t, std::vector<std::string>>(node_id, container_ids));
        }
        else
        {
            ids[node_id].push_back(idMap.value(CONTAINER_ID).toString().toStdString());
        }
    }
}

void ContainerListPage::getItemId(int row, std::map<int64_t, std::vector<std::string>> &ids)
{
    auto item = getItem(row, 1);
    QMap<QString, QVariant> idMap = item->data().value<QMap<QString, QVariant>>();

    std::vector<std::string> container_ids;
    container_ids.push_back(idMap.value(CONTAINER_ID).toString().toStdString());
    auto nodeId = idMap.value(NODE_ID).toInt();
    ids.insert(std::pair<int64_t, std::vector<std::string>>(nodeId, container_ids));
}

void ContainerListPage::updateInfo(QString keyword)
{
    KLOG_INFO() << "containerList updateInfo";
    clearText();
    //InfoWorker::getInstance().disconnect();
    disconnect(&InfoWorker::getInstance(), &InfoWorker::listNodeFinished, 0, 0);
    disconnect(&InfoWorker::getInstance(), &InfoWorker::listContainerFinished, 0, 0);
    if (keyword.isEmpty())
    {
        //initConnect();
        //gRPC->拿数据->填充内容
        connect(&InfoWorker::getInstance(), &InfoWorker::listNodeFinished, this, &ContainerListPage::getNodeListResult);
        connect(&InfoWorker::getInstance(), &InfoWorker::listContainerFinished, this, &ContainerListPage::getContainerListResult);

        getContainerList();
    }
}

void ContainerListPage::operateAreaVisible(bool visible)
{
}
