
/**
 * @file          /ks-scmc-gui/src/pages/container/container-manager/container-list-page.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
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
#include "def.h"

#include "common/message-dialog.h"
#include "common/monitor-dialog.h"
#include "container-list-page.h"
#include "load-configuration.h"

#define CONTAINER_STATUS_RUNNING "running"
#define CONTAINER_STATUS_EXITED "exited"
#define CONTAINRT_STATUS_CREATED "created"

using namespace grpc;

ContainerListPage::ContainerListPage(QWidget *parent)
    : TablePage(parent),
      m_createFromTemplateAct(nullptr),
      m_containerSetting(nullptr),
      m_monitor(nullptr),
      m_timer(nullptr)
{
    m_objId = InfoWorker::generateId(this);
    initButtons();
    //初始化表格
    initTable();
    initConnect();

    //FIXME:以后在多出调用的maskwidget情况下，在updateInfo中调用该函数
    //setMaskParent(this);

    m_statusMap.insert(CONTAINER_STATUS_RUNNING, QPair<QString, QString>(tr("Online"), "#00921b"));
    m_statusMap.insert(CONTAINER_STATUS_EXITED, QPair<QString, QString>(tr("Offline"), "#d30000"));
    m_statusMap.insert(CONTAINRT_STATUS_CREATED, QPair<QString, QString>(tr("Created"), "#00921b"));

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [this] {
        updateInfo();
    });

    connect(this, &ContainerListPage::sigTerminal, this, &ContainerListPage::onTerminal);
}

ContainerListPage::~ContainerListPage()
{
    KLOG_INFO() << "~ContainerListPage";
    if (m_containerSetting)
    {
        delete m_containerSetting;
        m_containerSetting = nullptr;
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
        InfoWorker::getInstance().startContainer(m_objId, ids);
    }
}

void ContainerListPage::onBtnRun(QModelIndex index)
{
    KLOG_INFO() << "onBtnRun:" << index.row();
    std::map<int64_t, std::vector<std::string>> ids;
    getItemId(index.row(), ids);
    setBusy(true);
    InfoWorker::getInstance().startContainer(m_objId, ids);
}

void ContainerListPage::onBtnStop()
{
    KLOG_INFO() << "onBtnStop";
    std::map<int64_t, std::vector<std::string>> ids;
    getCheckedItemsId(ids);
    if (!ids.empty())
    {
        setBusy(true);
        InfoWorker::getInstance().stopContainer(m_objId, ids);
    }
}

void ContainerListPage::onBtnStop(QModelIndex index)
{
    KLOG_INFO() << "onBtnStop:" << index.row();
    std::map<int64_t, std::vector<std::string>> ids;
    getItemId(index.row(), ids);
    setBusy(true);
    InfoWorker::getInstance().stopContainer(m_objId, ids);
}

void ContainerListPage::onBtnRestart()
{
    KLOG_INFO() << "onBtnRestart";
    std::map<int64_t, std::vector<std::string>> ids;
    getCheckedItemsId(ids);
    if (!ids.empty())
    {
        setBusy(true);
        InfoWorker::getInstance().restartContainer(m_objId, ids);
    }
}

void ContainerListPage::onBtnRestart(QModelIndex index)
{
    KLOG_INFO() << "onBtnRestart:" << index.row();
    std::map<int64_t, std::vector<std::string>> ids;
    getItemId(index.row(), ids);
    setBusy(true);
    InfoWorker::getInstance().restartContainer(m_objId, ids);
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
            InfoWorker::getInstance().removeContainer(m_objId, ids);
        }
        else
            KLOG_INFO() << "cancel";
    }
}

void ContainerListPage::onActCreate()
{
    KLOG_INFO() << "onActCreate";
    operateContainer(CONTAINER_SETTING_TYPE_CONTAINER_CREATE);
}

void ContainerListPage::onActCopyConfig()
{
    KLOG_INFO() << "onCopyConfig";
    operateContainer(CONTAINER_SETTING_TYPE_CONTAINER_CREATE_FROM_TEMPLATE);
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
    operateContainer(CONTAINER_SETTING_TYPE_CONTAINER_EDIT, row);
}

void ContainerListPage::onTerminal(int row)
{
    auto infoMap = getItem(row, 1)->data().value<QMap<QString, QVariant>>();
    auto nodeAddr = infoMap.value(NODE_ADDRESS).toString();
    auto containerName = infoMap.value(CONTAINER_NAME).toString();
    //    auto cmd = QString("%1 \"ssh -Xt root@%2 CONTAINER_NAME=%3 bash --rcfile %4\"")
    //                   .arg(TERMINAL_CMD, nodeAddr, containerName, BASHRC_FILE);

    auto cmd = LoadConfiguration::getTerminalConfig(nodeAddr, containerName);

    KLOG_INFO() << cmd;
    QProcess proc;
    proc.startDetached(cmd);
}

void ContainerListPage::onItemClicked(const QModelIndex &index)
{
    KLOG_INFO() << "onItemClicked: " << index.column();
    auto item = getItem(index.row(), index.column());
    if (item)
    {
        if (index.column() == 1 && item->text() != "-")
        {
            auto item = getItem(index.row(), index.column());
            auto infoMap = item->data().value<QMap<QString, QVariant>>();

            emit sigContainerNameClicked(infoMap);
        }
    }
}

void ContainerListPage::onItemEntered(const QModelIndex &index)
{
    auto item = getItem(index.row(), index.column());
    if (item)
    {
        if (index.column() == 1 && item->text() != "-")
            this->setCursor(Qt::PointingHandCursor);
        else
            this->setCursor(Qt::ArrowCursor);
    }
}

void ContainerListPage::getNetworkListResult(const QString objId, const QPair<grpc::Status, network::ListReply> &reply)
{
    KLOG_INFO() << "getNetworkListResult" << m_objId << objId;
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            m_networksMap.clear();
            for (auto ifs : reply.second.virtual_ifs())
            {
                int nodeId = ifs.node_id();
                KLOG_INFO() << nodeId << ifs.name().data() << ifs.ip_address().data() << ifs.ip_mask_len();
                auto name = ifs.name();
                auto subnet = ifs.ip_address() + "/" + std::to_string(ifs.ip_mask_len());
                QString str = QString("%1 (%2:%3)")
                                  .arg(QString::fromStdString(name))
                                  .arg(tr("Subnet"))
                                  .arg(QString::fromStdString(subnet));
                KLOG_INFO() << str;
                m_networksMap.insert(nodeId, str);
            }
            KLOG_INFO() << m_networksMap.keys();
            KLOG_INFO() << m_networksMap.values();
        }
        else
        {
            KLOG_INFO() << "getNetworkListResult failed";
        }
    }
}

void ContainerListPage::getContainerListResult(const QString objId, const QPair<grpc::Status, container::ListReply> &reply)
{
    KLOG_INFO() << "getContainerListResult" << m_objId << objId;
    if (m_objId == objId)
    {
        setBusy(false);
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
        KLOG_INFO() << "getContainerListResult";
        if (reply.first.ok())
        {
            clearTable();
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
            int size = reply.second.containers_size();
            KLOG_INFO() << "container size:" << size;
            if (size <= 0)
            {
                setHeaderCheckable(false);
                return;
            }
            setHeaderCheckable(true);
            int row = 0;
            QMap<QString, QVariant> infoMap;
            for (auto i : reply.second.containers())
            {
                qint64 nodeId = i.node_id();
                infoMap.insert(NODE_ID, nodeId);
                infoMap.insert(CONTAINER_ID, i.info().id().data());
                infoMap.insert(CONTAINER_NAME, i.info().name().data());
                infoMap.insert(CONTAINER_STATUS, i.info().state().data());
                infoMap.insert(NODE_ADDRESS, i.node_address().data());

                QStandardItem *itemCheck = new QStandardItem();
                itemCheck->setCheckable(true);

                QStandardItem *itemName = new QStandardItem(i.info().name().data());
                itemName->setData(QVariant::fromValue(infoMap));
                itemName->setForeground(QBrush(QColor(46, 179, 255)));

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

                QString strOnline = "-";
                if (i.info().started() != 0)
                {
                    auto dt = QDateTime::fromSecsSinceEpoch(i.info().started());
                    strOnline = dt.toString("yyyy/MM/dd hh:mm:ss");
                }

                QStandardItem *itemCpu = new QStandardItem(strCpuPct.data());
                QStandardItem *itemMem = new QStandardItem(strMemPct.data());
                QStandardItem *itemDisk = new QStandardItem(strDiskPct.data());
                QStandardItem *onlineTime = new QStandardItem(strOnline);

                setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemStatus << itemImage << itemNodeAddress << itemCpu << itemMem << itemDisk << onlineTime);

                row++;
            }
        }
        else
        {
            if (PERMISSION_DENIED == reply.first.error_code())
                setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
            else
            {
                setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
                if (DEADLINE_EXCEEDED == reply.first.error_code())
                {
                    setTips(tr("Response timeout!"));
                }
            }
            setHeaderCheckable(false);
            setTableDefaultContent("-");
        }
    }
}

void ContainerListPage::getContainerStartResult(const QString objId, const QPair<grpc::Status, container::StartReply> &reply)
{
    KLOG_INFO() << "getContainerStartResult" << m_objId << objId;
    if (m_objId == objId)
    {
        setBusy(false);
        KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
        if (reply.first.ok())
        {
            getContainerList();
            return;
        }
        else
        {
            MessageDialog::message(tr("Start Container"),
                                   tr("Start container failed!"),
                                   tr("Error: %1").arg(reply.first.error_message().data()),
                                   tr(":/images/error.svg"),
                                   MessageDialog::StandardButton::Ok);
        }
    }
}

void ContainerListPage::getContainerStopResult(const QString objId, const QPair<grpc::Status, container::StopReply> &reply)
{
    KLOG_INFO() << "getContainerStopResult" << m_objId << objId;
    if (m_objId == objId)
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
            MessageDialog::message(tr("Stop Container"),
                                   tr("Stop container failed!"),
                                   tr("Error: %1").arg(reply.first.error_message().data()),
                                   tr(":/images/error.svg"),
                                   MessageDialog::StandardButton::Ok);
        }
    }
}

void ContainerListPage::getContainerRestartResult(const QString objId, const QPair<grpc::Status, container::RestartReply> &reply)
{
    KLOG_INFO() << "getContainerRestartResult" << m_objId << objId;
    if (m_objId == objId)
    {
        setBusy(false);
        KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
        if (reply.first.ok())
        {
            getContainerList();
            return;
        }
        else
        {
            MessageDialog::message(tr("Restart Container"),
                                   tr("Restart container failed!"),
                                   tr("Error: %1").arg(reply.first.error_message().data()),
                                   tr(":/images/error.svg"),
                                   MessageDialog::StandardButton::Ok);
        }
    }
}

void ContainerListPage::getContainerRemoveResult(const QString objId, const QPair<grpc::Status, container::RemoveReply> &reply)
{
    KLOG_INFO() << "getContainerRemoveResult" << m_objId << objId;
    if (m_objId == objId)
    {
        setBusy(false);
        KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
        if (reply.first.ok())
        {
            getContainerList();
            return;
        }
        else
        {
            MessageDialog::message(tr("Remove Container"),
                                   tr("Remove container failed!"),
                                   tr("Error: %1").arg(reply.first.error_message().data()),
                                   tr(":/images/error.svg"),
                                   MessageDialog::StandardButton::Ok);
        }
    }
}

void ContainerListPage::getListTemplateFinishResult(const QString objId, const QPair<grpc::Status, container::ListTemplateReply> &reply)
{
    KLOG_INFO() << "getListTemplateFinishResult";
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            m_templateMap.clear();
            int size = reply.second.data_size();
            if (size <= 0)
            {
                m_createFromTemplateAct->setDisabled(true);
                return;
            }
            for (auto data : reply.second.data())
            {
                auto cfg = data.conf();
                int tempId = data.id();
                int nodeId = data.node_id();
                QString name = QString::fromStdString(cfg.name().data());
                m_templateMap.insert(tempId, QPair<int, QString>{nodeId, name});
            }
            m_createFromTemplateAct->setDisabled(false);
        }
        else
            m_createFromTemplateAct->setDisabled(true);
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
    btnCreate->setStyleSheet("#btnCreate{background-color:#2EB3FF;"
                             "border:none;"
                             "border-radius: 4px;"
                             "font-size:14px;"
                             "color:#ffffff;}"
                             "#btnCreate:hover{background-color:#77ceff;}"
                             "#btnCreate:disabled{color:#919191;background:#393939;}"
                             "#btnCreate:focus{outline:none;}"
                             "#btnCreate::menu-indicator{image: url(:/images/down-arrow.svg);"
                             "subcontrol-position: right center;"
                             "subcontrol-origin: padding;"
                             " left: -5px;}");

    QMenu *btnCreateMenu = new QMenu(btnCreate);
    btnCreateMenu->setObjectName("btnCreateMenu");
    QAction *create = btnCreateMenu->addAction(tr("Create container"));
    m_createFromTemplateAct = btnCreateMenu->addAction(tr("Create container from template"));
    btnCreate->setMenu(btnCreateMenu);
    connect(create, &QAction::triggered, this, &ContainerListPage::onActCreate);
    connect(m_createFromTemplateAct, &QAction::triggered, this, &ContainerListPage::onActCopyConfig);

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
    connect(this, &ContainerListPage::sigHasRunningCtn,
            [this](bool hasRunningCtn) {
                m_batchOpBtnMap[OPERATION_BUTTOM_CONTAINER_LIST_DELETE]->setDisabled(hasRunningCtn);
            });

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
    setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>{{ACTION_BUTTON_TYPE_MONITOR, QPair<QString, QString>{tr("Monitor"), ":/images/monitor.svg"}},
                                                                                                   {ACTION_BUTTON_TYPE_EDIT, QPair<QString, QString>{tr("Edit"), ":/images/edit.svg"}},
                                                                                                   {ACTION_BUTTON_TYPE_TERINAL, QPair<QString, QString>{tr("Terminal"), ":/images/terminal.svg"}},
                                                                                                   {ACTION_BUTTON_TYPE_MENU, QPair<QString, QString>{tr("More"), ":/images/more.svg"}}});

    setTableDefaultContent("-");

    connect(this, &ContainerListPage::sigMonitor, this, &ContainerListPage::onMonitor);
    connect(this, &ContainerListPage::sigEdit, this, &ContainerListPage::onEdit);

    connect(this, &ContainerListPage::sigItemClicked, this, &ContainerListPage::onItemClicked);
    connect(this, &ContainerListPage::sigItemEntered, this, &ContainerListPage::onItemEntered);
}

void ContainerListPage::initConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::listTemplateFinished, this, &ContainerListPage::getListTemplateFinishResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::listNetworkFinished, this, &ContainerListPage::getNetworkListResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::listContainerFinished, this, &ContainerListPage::getContainerListResult, Qt::UniqueConnection);
    connect(&InfoWorker::getInstance(), &InfoWorker::startContainerFinished, this, &ContainerListPage::getContainerStartResult, Qt::UniqueConnection);
    connect(&InfoWorker::getInstance(), &InfoWorker::stopContainerFinished, this, &ContainerListPage::getContainerStopResult, Qt::UniqueConnection);
    connect(&InfoWorker::getInstance(), &InfoWorker::restartContainerFinished, this, &ContainerListPage::getContainerRestartResult, Qt::UniqueConnection);
    connect(&InfoWorker::getInstance(), &InfoWorker::removeContainerFinished, this, &ContainerListPage::getContainerRemoveResult, Qt::UniqueConnection);
}

void ContainerListPage::operateContainer(ContainerSettingType type, int row)
{
    if (!m_containerSetting)
    {
        switch (type)
        {
        case CONTAINER_SETTING_TYPE_CONTAINER_CREATE:
        {
            m_containerSetting = new ContainerSetting(CONTAINER_SETTING_TYPE_CONTAINER_CREATE, m_networksMap);
            break;
        }
        case CONTAINER_SETTING_TYPE_CONTAINER_EDIT:
        {
            auto item = getItem(row, 1);
            m_containerSetting = new ContainerSetting(CONTAINER_SETTING_TYPE_CONTAINER_EDIT,
                                                      m_networksMap,
                                                      item->data().value<QMap<QString, QVariant>>());
            break;
        }
        case CONTAINER_SETTING_TYPE_CONTAINER_CREATE_FROM_TEMPLATE:
        {
            m_containerSetting = new ContainerSetting(CONTAINER_SETTING_TYPE_CONTAINER_CREATE_FROM_TEMPLATE, m_networksMap);
            if (!m_templateMap.isEmpty())
                m_containerSetting->setTemplateList(m_templateMap);
            break;
        }
        default:
            break;
        }

        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_containerSetting->move(screenGeometry.x() + (screenGeometry.width() - m_containerSetting->width()) / 2,
                                 screenGeometry.y() + (screenGeometry.height() - m_containerSetting->height()) / 2);

        m_containerSetting->show();
        connect(m_containerSetting, &ContainerSetting::destroyed,
                [=] {
                    KLOG_INFO() << "container setting destroy";
                    m_containerSetting->deleteLater();
                    m_containerSetting = nullptr;
                });
        connect(m_containerSetting, &ContainerSetting::sigUpdateContainer,
                [=] {
                    getContainerList();
                });
    }
}

void ContainerListPage::getTemplateList()
{
    InfoWorker::getInstance().listTemplate(m_objId);
}

void ContainerListPage::getContainerList(qint64 nodeId)
{
    setBusy(true);
    std::vector<int64_t> vecNodeId;
    if (nodeId < 0)
    {
        InfoWorker::getInstance().listContainer(m_objId, vecNodeId, true);  //获取所有容器
    }
    else
    {
        KLOG_INFO() << "get container list of node " << nodeId;
        vecNodeId.push_back(nodeId);
        InfoWorker::getInstance().listContainer(m_objId, vecNodeId, true);  //获取某节点下的容器
    }
}

void ContainerListPage::getCheckedItemsId(std::map<int64_t, std::vector<std::string>> &ids)
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);
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

void ContainerListPage::timedRefresh(bool start)
{
    KLOG_INFO() << start;
    if (start)
        m_timer->start(60000);
    else
    {
        m_timer->stop();
    }
}

void ContainerListPage::getNetworkInfo(int64_t node_id)
{
    KLOG_INFO() << "getNetworkInfo" << node_id;
    InfoWorker::getInstance().listNetwork(m_objId, node_id);
}

void ContainerListPage::updateInfo(QString keyword)
{
    KLOG_INFO() << "containerList updateInfo, keyword:" << keyword;
    if (keyword == "exitTimedRefresh")
    {
        timedRefresh(false);
        clearCheckState();
        return;
    }

    clearText();
    if (keyword.isEmpty())
    {
        //gRPC->拿数据->填充内容
        getContainerList();
        getTemplateList();
        getNetworkInfo(-1);  //-1返回所有节点的网卡信息
        timedRefresh(true);
    }
}
