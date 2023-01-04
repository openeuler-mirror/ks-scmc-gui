#include "container-app-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QDesktopWidget>

#include "load-configuration.h"
#include "message-dialog.h"
#include "notification-manager.h"
using namespace grpc;

#define CONTAINER_APP_ID "container app id"
#define CONTAINER_APP_NAME "container app name"
#define CONTAINER_APP_PATH "container app path"
#define CONTAINER_APP_IS_GUI "container app is_gui"
#define STATUS_COL 4

ContainerAppPage::ContainerAppPage(int64_t nodeId, QString nodeAddr, std::string containerId, QString containerName, QWidget *parent) : TablePage(parent),
                                                                                                                                        m_nodeId(nodeId),
                                                                                                                                        m_containerId(containerId),
                                                                                                                                        m_nodeAddr(nodeAddr),
                                                                                                                                        m_containerName(containerName),
                                                                                                                                        m_appOp(nullptr),
                                                                                                                                        m_proc(nullptr)
{
    m_objId = InfoWorker::generateId(this);
    initButtons();
    initTable();
    initConnect();
    setStyleSheet("background-color:#222222;");
}

ContainerAppPage::~ContainerAppPage()
{
    if (m_proc)
    {
        delete m_proc;
        m_proc = nullptr;
    }
}

void ContainerAppPage::updateInfo(QString keyword)
{
    clearText();
    InfoWorker::getInstance().listAppEntry(m_objId, m_nodeId, m_containerId);
}

void ContainerAppPage::onCreate()
{
    if (!m_appOp)
    {
        m_appOp = new ContainerAppOperateDialog(APP_OPERATE_TYPE_CREATE, this);
        m_appOp->setTitle(tr("Create app"));
        connect(m_appOp, &ContainerAppOperateDialog::sigSave, this, &ContainerAppPage::onSaveApp);
    }
    showOperateDlg();
}

void ContainerAppPage::onEdit(int row)
{
    auto item = getItem(row, 1);
    QMap<QString, QVariant> appInfo = item->data().toMap();
    //判断应用是否在运行
    QString name = appInfo.value(CONTAINER_APP_NAME).toString();
    if (!appInfo.value(CONTAIENR_APP_IS_RUNNING).toBool())
    {
        int appId = appInfo.value(CONTAINER_APP_ID).toInt();
        QString path = appInfo.value(CONTAINER_APP_PATH).toString();
        bool isGUI = appInfo.value(CONTAINER_APP_IS_GUI).toBool();

        if (!m_appOp)
        {
            m_appOp = new ContainerAppOperateDialog(appId, name, path, isGUI, APP_OPERATE_TYPE_EDIT, this);
            m_appOp->setTitle(tr("Edit app"));
            connect(m_appOp, &ContainerAppOperateDialog::sigSave, this, &ContainerAppPage::onSaveApp);
        }
        showOperateDlg();
    }
    else
    {
        NotificationManager::sendNotify(tr("Can't edit the app %1").arg(name),
                                        tr("The app is running!"));
    }
}

void ContainerAppPage::onRun(int row)
{
    auto item = getItem(row, 1);
    QMap<QString, QVariant> appInfo = item->data().toMap();

    //判断app类型
    if (appInfo.value(CONTAINER_APP_IS_GUI).toBool())
    {
        //图形app则弹出终端
        //判断该应用程序是否已经在运行
        auto isRunning = appInfo.value(CONTAIENR_APP_IS_RUNNING).toBool();
        if (isRunning)
        {
            NotificationManager::sendNotify(tr("The app(%1) is running!").arg(appInfo.value(CONTAINER_APP_NAME).toString()), "");
            return;
        }

        if (!m_proc)
        {
            m_proc = new QProcess();
            connect(m_proc, &QProcess::stateChanged, this, &ContainerAppPage::guiAppStatus);
        }

        //判断该子进程中是否有应用程序在运行
        if (m_proc->state() == QProcess::ProcessState::Running)
        {
            NotificationManager::sendNotify(tr("There is a app running!"), "");
            return;
        }

        auto cmd = LoadConfiguration::getTerminalConfig("10.20.1.52 -p 222", m_containerName, appInfo.value(CONTAINER_APP_PATH).toString());
        KLOG_INFO() << cmd;

        m_runningAppRow = row;
        m_proc->start(cmd);

        KLOG_INFO() << "pid: " << m_proc->processId();
    }
    else
    {
        //非图形app直接运行
        int appId = appInfo.value(CONTAINER_APP_ID).toInt();
        InfoWorker::getInstance().runAppEntry(m_objId, m_nodeId, m_containerId, appId);
    }
}

void ContainerAppPage::onStop(int row)
{
    auto item = getItem(row, 1);
    QMap<QString, QVariant> appInfo = item->data().toMap();

    //判断app类型
    if (appInfo.value(CONTAINER_APP_IS_GUI).toBool())
    {
        //图形app则关闭终端
        if (!appInfo.value(CONTAIENR_APP_IS_RUNNING).toBool())
        {
            NotificationManager::sendNotify(tr("The app(%1) is not running!").arg(appInfo.value(CONTAINER_APP_NAME).toString()), "");
            return;
        }
        if (m_proc && m_proc->state() == QProcess::ProcessState::Running)
        {
            m_proc->kill();
        }
    }
    else
    {
        //非图形app直接运行
        int appId = appInfo.value(CONTAINER_APP_ID).toInt();
        InfoWorker::getInstance().killAppEntry(m_objId, m_nodeId, m_containerId, appId);
    }
}

void ContainerAppPage::onDelete()
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);
    QList<int> appIds;
    foreach (auto idMap, info)
    {
        KLOG_INFO() << idMap.value(CONTAINER_APP_ID).toInt();
        appIds.append(idMap.value(CONTAINER_APP_ID).toInt());
    }

    if (!appIds.empty())
    {
        auto ret = MessageDialog::message(tr("Delete Container App"),
                                          tr("Are you sure you want to delete the app?"),
                                          tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                          ":/images/warning.svg",
                                          MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
        if (ret == MessageDialog::StandardButton::Yes)
        {
            InfoWorker::getInstance().removeAppEntry(m_objId, m_nodeId, m_containerId, appIds);
        }
        else
            KLOG_INFO() << "cancel";
    }
}

void ContainerAppPage::onDelete(int row)
{
    auto ret = MessageDialog::message(tr("Delete Container App"),
                                      tr("Are you sure you want to delete the app?"),
                                      tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                      ":/images/warning.svg",
                                      MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
    if (ret == MessageDialog::StandardButton::Yes)
    {
        auto item = getItem(row, 1);
        QMap<QString, QVariant> appInfo = item->data().toMap();

        int appId = appInfo.value(CONTAINER_APP_ID).toInt();

        InfoWorker::getInstance().removeAppEntry(m_objId, m_nodeId, m_containerId, QList<int>() << appId);
    }
    else
        KLOG_INFO() << "cancel";
}

void ContainerAppPage::onSaveApp(const QString name, const QString path, const bool isGui)
{
    if (m_appOp->getType() == APP_OPERATE_TYPE_CREATE)
    {
        container::AddAppEntryRequest req;
        req.set_node_id(m_nodeId);
        auto app = req.mutable_app();
        app->set_container_id(m_containerId);
        app->set_name(name.toStdString());
        app->set_exe_path(path.toStdString());
        app->set_is_gui(isGui);
        InfoWorker::getInstance().addAppEntry(m_objId, req);
    }
    else
    {
        container::UpdateAppEntryRequest req;
        req.set_node_id(m_nodeId);
        auto app = req.mutable_app();
        app->set_id(m_appOp->getAppId());
        app->set_container_id(m_containerId);
        app->set_name(name.toStdString());
        app->set_exe_path(path.toStdString());
        app->set_is_gui(isGui);
        InfoWorker::getInstance().updateAppEntry(m_objId, req);
    }
}

void ContainerAppPage::guiAppStatus(QProcess::ProcessState state)
{
    KLOG_INFO() << "process status:" << state;
    auto item = getItem(m_runningAppRow, STATUS_COL);
    auto dataItem = getItem(m_runningAppRow, 1);
    QMap<QString, QVariant> appInfo = dataItem->data().toMap();
    if (state == QProcess::ProcessState::Running)
    {
        item->setText(tr("Running"));
        item->setForeground(QBrush(QColor("#00921b")));
        appInfo.insert(CONTAIENR_APP_IS_RUNNING, true);
        dataItem->setData(appInfo);
    }
    else if (state == QProcess::ProcessState::NotRunning)
    {
        item->setText(tr("Stop"));
        item->setForeground(QBrush(QColor("#d30000")));
        appInfo.insert(CONTAIENR_APP_IS_RUNNING, false);
        dataItem->setData(appInfo);
    }
}

void ContainerAppPage::getListAppEntryFinished(const QString objId, const QPair<grpc::Status, container::ListAppEntryReply> &reply)
{
    KLOG_INFO() << "getListAppEntryFinished" << m_objId << objId;
    if (m_objId == objId)
    {
        setBusy(false);
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
        if (reply.first.ok())
        {
            clearTable();
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
            int size = reply.second.apps_size();
            KLOG_INFO() << "container app  size:" << size;
            if (size <= 0)
            {
                setHeaderCheckable(false);
                return;
            }
            setHeaderCheckable(true);
            int row = 0;
            QMap<QString, QVariant> infoMap;
            for (auto app : reply.second.apps())
            {
                std::string contaienrId = app.container_id().data();
                std::string appName = app.name().data();
                std::string appPath = app.exe_path().data();
                bool isGUI = app.is_gui();
                bool isRuning = app.is_running();
                infoMap.insert(NODE_ID, m_nodeId);
                infoMap.insert(CONTAINER_ID, contaienrId.data());
                infoMap.insert(CONTAINER_APP_ID, QVariant::fromValue(app.id()));
                infoMap.insert(CONTAINER_APP_NAME, appName.data());
                infoMap.insert(CONTAINER_APP_PATH, appPath.data());
                infoMap.insert(CONTAINER_APP_IS_GUI, isGUI);
                infoMap.insert(CONTAIENR_APP_IS_RUNNING, isRuning);

                QStandardItem *itemCheck = new QStandardItem();
                itemCheck->setCheckable(true);

                QStandardItem *itemName = new QStandardItem(appName.data());
                itemName->setData(infoMap);

                QStandardItem *itemType = new QStandardItem(isGUI ? tr("Gui") : tr("Non Gui"));

                QStandardItem *itemPath = new QStandardItem(app.exe_path().data());

                QStandardItem *itemStatus = new QStandardItem(isRuning ? tr("Running") : tr("Stop"));
                itemStatus->setForeground(QBrush(QColor(isRuning ? "#00921b" : "#d30000")));

                setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemType << itemPath << itemStatus);
                row++;
            }
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

void ContainerAppPage::getAddAppEntryFinished(const QString objId, const QPair<grpc::Status, container::AddAppEntryReply> &reply)
{
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            m_appOp->close();
            updateInfo();
            NotificationManager::sendNotify(tr("Add container app successful!"), "");
        }
        else
            NotificationManager::sendNotify(tr("Add container app failed!"),
                                            reply.first.error_message().data());
    }
}

void ContainerAppPage::getUpdateAppEntryFinished(const QString objId, const QPair<grpc::Status, container::UpdateAppEntryReply> &reply)
{
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            m_appOp->close();
            updateInfo();
            NotificationManager::sendNotify(tr("Update container app successful!"), "");
        }
        else
            NotificationManager::sendNotify(tr("Update container app failed!"),
                                            reply.first.error_message().data());
    }
}

void ContainerAppPage::getRemoveAppEntryFinished(const QString objId, const QPair<grpc::Status, container::RemoveAppEntryReply> &reply)
{
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            updateInfo();
            NotificationManager::sendNotify(tr("Remove container app successful!"), "");
        }
        else
            NotificationManager::sendNotify(tr("Remove container app failed!"),
                                            reply.first.error_message().data());
    }
}

void ContainerAppPage::getRunAppEntryFinished(const QString objId, const QPair<grpc::Status, container::RunAppEntryReply> &reply)
{
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            updateInfo();
            NotificationManager::sendNotify(tr("Run container app successful!"), "");
        }
        else
            NotificationManager::sendNotify(tr("Run container app failed!"),
                                            reply.first.error_message().data());
    }
}

void ContainerAppPage::getKillAppEntryFinished(const QString objId, const QPair<Status, container::KillAppEntryReply> &reply)
{
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            updateInfo();
            NotificationManager::sendNotify(tr("Stop container app successful!"), "");
        }
        else
            NotificationManager::sendNotify(tr("Stop container app failed!"),
                                            reply.first.error_message().data());
    }
}

void ContainerAppPage::initButtons()
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

    connect(btnCreate, &QPushButton::clicked, this, &ContainerAppPage::onCreate);
    //其他按钮及菜单
    QPushButton *btnDelete = new QPushButton(tr("Delete"), this);
    btnDelete->setObjectName("btnDelete");
    btnDelete->setStyleSheet("#btnDelete{background-color:#ff4b4b;"
                             "border:none;"
                             "border-radius: 4px;"
                             "color:#ffffff;}"
                             "#btnDelete:hover{ background-color:#ff6c6c;}"
                             "#btnDelete:focus{outline:none;}"
                             "#btnDelete:disabled{color:#919191;background:#393939;}");

    btnDelete->setFixedSize(QSize(78, 32));

    connect(btnDelete, SIGNAL(clicked()), this, SLOT(onDelete()));
    connect(this, &ContainerAppPage::sigHasRunningCtn,
            [=](bool hasRunningCtn) {
                btnDelete->setDisabled(hasRunningCtn);
            });

    addBatchOperationButtons(QList<QPushButton *>() << btnDelete);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void ContainerAppPage::initTable()
{
    QStringList tableHHeaderDate = {
        "",
        tr("APP Name"),
        tr("Type"),
        tr("Exec Path"),
        tr("Status"),
        tr("Quick Actions")};
    setHeaderSections(tableHHeaderDate);
    setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>{{ACTION_BUTTON_TYPE_APP_RUN, QPair<QString, QString>{tr("Run"), tr("Run")}},
                                                                                                   {ACTION_BUTTON_TYPE_APP_STOP, QPair<QString, QString>{tr("Stop"), tr("Stop")}},
                                                                                                   {ACTION_BUTTON_TYPE_EDIT, QPair<QString, QString>{tr("Edit"), tr("Edit")}},
                                                                                                   {ACTION_BUTTON_TYPE_DELETE, QPair<QString, QString>{tr("Delete"), tr("Delete")}}});

    setTableDefaultContent("-");
    setOperateSearchVisible(false);

    connect(this, &ContainerAppPage::sigEdit, this, &ContainerAppPage::onEdit);
    connect(this, SIGNAL(sigAppRun(int)), this, SLOT(onRun(int)));
    connect(this, SIGNAL(sigAppStop(int)), this, SLOT(onStop(int)));
    connect(this, SIGNAL(sigDelete(int)), this, SLOT(onDelete(int)));
}

void ContainerAppPage::initConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::listAppEntryFinished, this, &ContainerAppPage::getListAppEntryFinished);
    connect(&InfoWorker::getInstance(), &InfoWorker::addAppEntryFinished, this, &ContainerAppPage::getAddAppEntryFinished);
    connect(&InfoWorker::getInstance(), &InfoWorker::updateAppEntryFinished, this, &ContainerAppPage::getUpdateAppEntryFinished);
    connect(&InfoWorker::getInstance(), &InfoWorker::removeAppEntryFinished, this, &ContainerAppPage::getRemoveAppEntryFinished);
    connect(&InfoWorker::getInstance(), &InfoWorker::runAppEntryFinished, this, &ContainerAppPage::getRunAppEntryFinished);
    connect(&InfoWorker::getInstance(), &InfoWorker::killAppEntryFinished, this, &ContainerAppPage::getKillAppEntryFinished);
}

void ContainerAppPage::showOperateDlg()
{
    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    m_appOp->move(screenGeometry.x() + (screenGeometry.width() - m_appOp->width()) / 2,
                  screenGeometry.y() + (screenGeometry.height() - m_appOp->height()) / 2);

    m_appOp->show();
    connect(m_appOp, &ContainerAppOperateDialog::destroyed,
            [=] {
                KLOG_INFO() << "destroy  ContainerAppOperateDialog";
                m_appOp->deleteLater();
                m_appOp = nullptr;
            });
}

ContainerAppDialog::ContainerAppDialog(int64_t nodeId, QString nodeAddr, std::string containerId, QString containerName, QWidget *parent) : KiranTitlebarWindow(parent)
{
    setTitle(tr("Container app"));
    setIcon(QIcon(":/images/logo.png"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    QWidget *windowContentWidget = getWindowContentWidget();
    QVBoxLayout *vLayout = new QVBoxLayout(windowContentWidget);
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 20, 0, 20);

    ContainerAppPage *appPage = new ContainerAppPage(nodeId, nodeAddr, containerId, containerName, windowContentWidget);
    appPage->updateInfo();
    vLayout->addWidget(appPage);
}
