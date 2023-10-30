#include "container-app-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QDesktopWidget>
#include "notification-manager.h"
using namespace grpc;

#define CONTAINER_APP_ID "container app id"
#define CONTAINER_APP_NAME "container app name"
#define CONTAINER_APP_PATH "container app path"
#define CONTAINER_APP_IS_GUI "container app is_gui"

ContainerAppPage::ContainerAppPage(int nodeId, std::string containerId, QWidget *parent) : TablePage(parent),
                                                                                           m_nodeId(nodeId),
                                                                                           m_containerId(containerId),
                                                                                           m_appOp(nullptr)
{
    m_objId = InfoWorker::generateId(this);
    initButtons();
    initTable();
    initConnect();
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
    //TODO:判断应用是否在运行

    int appId = appInfo.value(CONTAINER_APP_ID).toInt();
    QString name = appInfo.value(CONTAINER_APP_NAME).toString();
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

void ContainerAppPage::onRun()
{
}

void ContainerAppPage::onRun(int row)
{
}

void ContainerAppPage::onStop()
{
}

void ContainerAppPage::onDelete()
{
}

void ContainerAppPage::onDelete(int row)
{
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
                infoMap.insert(NODE_ID, m_nodeId);
                infoMap.insert(CONTAINER_ID, contaienrId.data());
                infoMap.insert(CONTAINER_APP_ID, QVariant::fromValue(app.id()));
                infoMap.insert(CONTAINER_APP_NAME, appName.data());
                infoMap.insert(CONTAINER_APP_PATH, appPath.data());
                infoMap.insert(CONTAINER_APP_IS_GUI, isGUI);

                QStandardItem *itemCheck = new QStandardItem();
                itemCheck->setCheckable(true);

                QStandardItem *itemName = new QStandardItem(appName.data());
                itemName->setData(infoMap);

                QStandardItem *itemType = new QStandardItem(isGUI ? tr("Gui") : tr("Non Gui"));

                setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemType);
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
            m_appOp->close();
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
            m_appOp->close();
            updateInfo();
            NotificationManager::sendNotify(tr("Run container app successful!"), "");
        }
        else
            NotificationManager::sendNotify(tr("Run container app failed!"),
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
    const QMap<int, QString> btnNameMap = {
        {OPERATION_BUTTOM_CONTAINER_LIST_RUN, tr("Run")},
        {OPERATION_BUTTOM_CONTAINER_LIST_STOP, tr("Stop")},
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

    connect(m_batchOpBtnMap[OPERATION_BUTTOM_CONTAINER_LIST_RUN], SIGNAL(clicked()), this, SLOT(onRun()));
    connect(m_batchOpBtnMap[OPERATION_BUTTOM_CONTAINER_LIST_STOP], SIGNAL(clicked()), this, SLOT(onStop()));
    // connect(m_batchOpBtnMap[OPERATION_BUTTOM_CONTAINER_LIST_DELETE], &QPushButton::clicked, this, &ContainerListPage::onBtnDelete);

    addBatchOperationButtons(m_batchOpBtnMap.values());
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void ContainerAppPage::initTable()
{
    QStringList tableHHeaderDate = {
        "",
        QString(tr("APP Name")),
        QString(tr("Type")),
        QString(tr("Quick Actions"))};
    setHeaderSections(tableHHeaderDate);
    setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>{{ACTION_BUTTON_TYPE_APP_RUN, QPair<QString, QString>{tr("Run"), tr("Run")}},
                                                                                                   {ACTION_BUTTON_TYPE_EDIT, QPair<QString, QString>{tr("Edit"), tr("Edit")}},
                                                                                                   {ACTION_BUTTON_TYPE_DELETE, QPair<QString, QString>{tr("Delete"), tr("Delete")}}});

    setTableDefaultContent("-");
    setOperateSearchVisible(false);

    connect(this, &ContainerAppPage::sigEdit, this, &ContainerAppPage::onEdit);
    connect(this, SIGNAL(sigAppRun(int)), this, SLOT(onRun(int)));
    connect(this, SIGNAL(sigDelete(int)), this, SLOT(onDelete(int)));
}

void ContainerAppPage::initConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::listAppEntryFinished, this, &ContainerAppPage::getListAppEntryFinished);
    connect(&InfoWorker::getInstance(), &InfoWorker::addAppEntryFinished, this, &ContainerAppPage::getAddAppEntryFinished);
    connect(&InfoWorker::getInstance(), &InfoWorker::updateAppEntryFinished, this, &ContainerAppPage::getUpdateAppEntryFinished);
    connect(&InfoWorker::getInstance(), &InfoWorker::removeAppEntryFinished, this, &ContainerAppPage::getRemoveAppEntryFinished);
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

ContainerAppDialog::ContainerAppDialog(int64_t nodeId, std::string containerId, QWidget *parent) : KiranTitlebarWindow(parent)
{
    setTitle("Container app");
    setIcon(QIcon(":/images/logo.png"));
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint | KiranTitlebarWindow::TitlebarCloseButtonHint);
    setAttribute(Qt::WA_DeleteOnClose);
    //setResizeable(false);
    QWidget *windowContentWidget = getWindowContentWidget();
    QVBoxLayout *vLayout = new QVBoxLayout(windowContentWidget);
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 20, 0, 20);

    ContainerAppPage *appPage = new ContainerAppPage(nodeId, containerId, windowContentWidget);
    appPage->updateInfo();
    vLayout->addWidget(appPage);
}
