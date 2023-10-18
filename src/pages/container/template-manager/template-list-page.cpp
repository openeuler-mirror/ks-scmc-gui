#include "template-list-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QPushButton>
#include "container/container-setting.h"
#include "def.h"
#include "message-dialog.h"

using namespace grpc;

TemplateListPage::TemplateListPage(QWidget *parent) : TablePage(parent), m_createTPSetting(nullptr), m_editTPSetting(nullptr)
{
    m_objId = InfoWorker::generateId(this);
    initTable();
    initButtons();
    initTemplateConnect();
}

TemplateListPage::~TemplateListPage()
{
    if (m_createTPSetting)
    {
        delete m_createTPSetting;
        m_createTPSetting = nullptr;
    }
    if (m_editTPSetting)
    {
        delete m_editTPSetting;
        m_editTPSetting = nullptr;
    }
}

void TemplateListPage::updateInfo(QString keyword)
{
    KLOG_INFO() << "TemplateListPage updateInfo";
    clearCheckState();
    clearText();
    if (keyword.isEmpty())
    {
        //initConnect();
        //gRPC->拿数据->填充内容
        getTemplateInfo();
        getNetworkInfo(-1);  //-1返回所有节点的网卡信息
    }
}

void TemplateListPage::onEdit(int row)
{
    KLOG_INFO() << "TemplateListPage edit:" << row;
    QStandardItem *item = getItem(row, 1);
    if (item)
    {
        if (!m_editTPSetting)
        {
            m_editTPSetting = new ContainerSetting(CONTAINER_SETTING_TYPE_TEMPLATE_EDIT,
                                                   m_networksMap,
                                                   item->data().value<QMap<QString, QVariant>>());

            int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
            QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
            m_editTPSetting->move(screenGeometry.x() + (screenGeometry.width() - m_editTPSetting->width()) / 2,
                                  screenGeometry.y() + (screenGeometry.height() - m_editTPSetting->height()) / 2);

            m_editTPSetting->show();
            connect(m_editTPSetting, &ContainerSetting::destroyed,
                    [=] {
                        KLOG_INFO() << "update template setting destroy";
                        m_editTPSetting->deleteLater();
                        m_editTPSetting = nullptr;
                    });
            connect(m_editTPSetting, &ContainerSetting::sigUpdateTemplate,
                    [=] {
                        getTemplateInfo();
                    });
        }
    }
}

void TemplateListPage::onDelete(int row)
{
    KLOG_INFO() << "TemplateListPage delete:" << row;
    int64_t id = -1;
    getItemId(row, id);
    if (id > 0)
    {
        auto ret = MessageDialog::message(tr("Remove Template"),
                                          tr("Are you sure you want to Remove the Template?"),
                                          tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                          ":/images/warning.svg",
                                          MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
        if (ret == MessageDialog::StandardButton::Yes)
        {
            //setBusy(true);
            InfoWorker::getInstance().removeTemplate(m_objId, QList<int64_t>() << id);
        }
        else
            KLOG_INFO() << "cancel";
    }
}

void TemplateListPage::onCreateTemplate()
{
    KLOG_INFO() << "onCreateTemplate";
    if (!m_createTPSetting)
    {
        m_createTPSetting = new ContainerSetting(CONTAINER_SETTING_TYPE_TEMPLATE_CREATE, m_networksMap);

        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_createTPSetting->move(screenGeometry.x() + (screenGeometry.width() - m_createTPSetting->width()) / 2,
                                screenGeometry.y() + (screenGeometry.height() - m_createTPSetting->height()) / 2);

        m_createTPSetting->show();
        connect(m_createTPSetting, &ContainerSetting::destroyed,
                [=] {
                    KLOG_INFO() << "create template setting destroy";
                    m_createTPSetting->deleteLater();
                    m_createTPSetting = nullptr;
                });
        connect(m_createTPSetting, &ContainerSetting::sigUpdateTemplate,
                [=] {
                    getTemplateInfo();
                });
    }
}

void TemplateListPage::onRemoveTemplate()
{
    KLOG_INFO() << "onRemoveTemplate";
    QList<int64_t> ids;
    getCheckedItemsId(ids);
    if (!ids.empty())
    {
        auto ret = MessageDialog::message(tr("Remove Template"),
                                          tr("Are you sure you want to Remove the Template?"),
                                          tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                          ":/images/warning.svg",
                                          MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
        if (ret == MessageDialog::StandardButton::Yes)
        {
            //setBusy(true);
            InfoWorker::getInstance().removeTemplate(m_objId, ids);
        }
        else
            KLOG_INFO() << "cancel";
    }
}

void TemplateListPage::getNetworkListResult(const QString objId, const QPair<grpc::Status, network::ListReply> &reply)
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
            KLOG_INFO() << "getNetworkListResult failed";
    }
}

void TemplateListPage::getListTemplateFinishResult(const QString objId, const QPair<grpc::Status, container::ListTemplateReply> &reply)
{
    KLOG_INFO() << "getListTemplateFinishResult" << m_objId << objId;
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
            int size = reply.second.data_size();
            if (size <= 0)
            {
                setTableDefaultContent("-");
                setHeaderCheckable(false);
                return;
            }
            clearTable();
            setHeaderCheckable(true);
            int row = 0;
            QMap<QString, QVariant> idMap;
            for (auto data : reply.second.data())
            {
                auto cfg = data.conf();
                qint64 tempId = data.id();
                qint64 nodeId = data.node_id();
                idMap.insert(TEMPLATE_ID, tempId);
                idMap.insert(NODE_ID, nodeId);

                QStandardItem *itemCheck = new QStandardItem();
                itemCheck->setCheckable(true);

                QStandardItem *itemName = new QStandardItem(cfg.name().data());
                itemName->setData(QVariant::fromValue(idMap));
                itemName->setTextAlignment(Qt::AlignCenter);

                QStandardItem *itemDesc = new QStandardItem(cfg.desc().data());
                itemDesc->setTextAlignment(Qt::AlignCenter);

                //基础配置
                //cpu,内存,网卡
                auto cpuNum = cfg.resouce_limit().cpu_limit();
                auto memoryNum = cfg.resouce_limit().memory_limit() / 1024;
                auto networkNum = cfg.networks_size();
                QString baseCfg = tr("CPU: %1 core \n Memory: %2 G \n Network: %3 piece")
                                      .arg(cpuNum)
                                      .arg(memoryNum)
                                      .arg(networkNum);
                QStandardItem *itemBaseCfg = new QStandardItem(baseCfg);
                itemBaseCfg->setTextAlignment(Qt::AlignCenter);

                //高级配置
                //环境变量、共享目录、图形化使用、高可用
                auto envNum = cfg.envs().size();
                auto mountNum = cfg.mounts_size();
                QString enableGraphic = cfg.enable_graphic() ? tr("Open") : tr("Close");
                auto restartPolicy = cfg.restart_policy().name().data();
                QMap<QString, QString> policyMap = {{"no", tr("no")},
                                                    {"always", tr("always")},
                                                    {"on-failure", tr("on-failure")},
                                                    {"unless-stopped", tr("unless-stopped")}};

                QString advanceCfg = tr("Env: %1  Mount: %2 \n Graphic: %3 \n RestartPolicy: %4")
                                         .arg(envNum)
                                         .arg(mountNum)
                                         .arg(enableGraphic)
                                         .arg(policyMap.value(restartPolicy));
                QStandardItem *itemAdvanceCfg = new QStandardItem(advanceCfg);
                itemAdvanceCfg->setTextAlignment(Qt::AlignCenter);

                setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemDesc << itemBaseCfg << itemAdvanceCfg);
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

void TemplateListPage::getRemoveTemplateFinishResult(const QString objId, const QPair<grpc::Status, container::RemoveTemplateReply> &reply)
{
    KLOG_INFO() << "getRemoveTemplateFinishResult" << m_objId << objId;
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            getTemplateInfo();
        }
        else
        {
            MessageDialog::message(tr("Remove template"),
                                   tr("Remove template failed!"),
                                   tr("Error: ") + reply.first.error_message().data(),
                                   ":/images/error.svg",
                                   MessageDialog::StandardButton::Ok);
        }
    }
}

void TemplateListPage::initTable()
{
    QStringList tableHHeaderDate = {
        "",
        QString(tr("Template Name")),
        QString(tr("description")),
        QString(tr("Basic configuration")),
        QString(tr("Advanced configuration")),
        QString(tr("Quick Actions"))};
    setHeaderSections(tableHHeaderDate);
    setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>{
                                                     {ACTION_BUTTON_TYPE_EDIT, QPair<QString, QString>{tr("Edit"), ":/images/edit.svg"}},
                                                     {ACTION_BUTTON_TYPE_DELETE, QPair<QString, QString>{tr("Delete"), ":/images/btn-delete-normal.png"}}});

    setTableDefaultContent("-");

    connect(this, &TemplateListPage::sigEdit, this, &TemplateListPage::onEdit);
    connect(this, &TemplateListPage::sigDelete, this, &TemplateListPage::onDelete);
}

void TemplateListPage::initButtons()
{
    QPushButton *btnCreate = new QPushButton(this);
    btnCreate->setText(tr("Create"));
    btnCreate->setObjectName("btnCreate");
    btnCreate->setFixedSize(QSize(78, 32));
    connect(btnCreate, &QPushButton::clicked, this, &TemplateListPage::onCreateTemplate);

    QPushButton *btnRemove = new QPushButton(this);
    btnRemove->setText(tr("Remove"));
    btnRemove->setObjectName("btnRemove");
    btnRemove->setFixedSize(QSize(78, 32));
    connect(btnRemove, &QPushButton::clicked, this, &TemplateListPage::onRemoveTemplate);

    addSingleOperationButton(btnCreate);
    addBatchOperationButtons(QList<QPushButton *>() << btnRemove);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void TemplateListPage::initTemplateConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::listTemplateFinished, this, &TemplateListPage::getListTemplateFinishResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::listNetworkFinished, this, &TemplateListPage::getNetworkListResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::removeTemplateFinished, this, &TemplateListPage::getRemoveTemplateFinishResult);
}

void TemplateListPage::getCheckedItemsId(QList<int64_t> &ids)
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);
    foreach (auto idInfo, info)
    {
        ids.append(idInfo.value(TEMPLATE_ID).toInt());
    }
}

void TemplateListPage::getItemId(int row, int64_t &id)
{
    auto item = getItem(row, 1);
    QMap<QString, QVariant> idMap = item->data().value<QMap<QString, QVariant>>();
    id = idMap.value(TEMPLATE_ID).toInt();
}

void TemplateListPage::getNetworkInfo(int64_t node_id)
{
    KLOG_INFO() << "getNetworkInfo" << node_id;
    InfoWorker::getInstance().listNetwork(m_objId, node_id);
}

void TemplateListPage::getTemplateInfo()
{
    InfoWorker::getInstance().listTemplate(m_objId);
}
