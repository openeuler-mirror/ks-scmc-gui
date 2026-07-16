/**
 * @file          /ks-scmc-gui/src/pages/container/template-manager/template-list-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "template-list-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QPushButton>
#include "def.h"
#include "message-dialog.h"

using namespace grpc;

TemplateListPage::TemplateListPage(QWidget *parent) : TablePage(parent), m_createTPSetting(nullptr), m_editTPSetting(nullptr)
{
    m_objId = Container::generateId(this);
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
    clearCheckState();
    clearText();
    if (keyword.isEmpty())
    {
        // gRPC->拿数据->填充内容
        getTemplateInfo();
        getNetworkInfo(-1);  //-1返回所有节点的网卡信息
        getNodeInfo();
        getImageInfo();
    }
}

void TemplateListPage::onEdit(int row)
{
    QStandardItem *item = getItem(row, 1);

    if (!item)
        return;

    if (!m_editTPSetting)
    {
        m_editTPSetting = new ContainerSetting(CONTAINER_SETTING_TYPE_TEMPLATE_EDIT);
        m_editTPSetting->setNodeInfos(m_nodeInfoMap);
        m_editTPSetting->setNetworkInfos(m_networksMap);
        m_editTPSetting->setImageList(m_imageInfos);

        auto idMap = item->data().value<QMap<QString, QVariant>>();
        int templateID = idMap.value(TEMPLATE_ID).toInt();
        m_editTPSetting->getTemplateInspect(templateID);

        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_editTPSetting->move(screenGeometry.x() + (screenGeometry.width() - m_editTPSetting->width()) / 2,
                              screenGeometry.y() + (screenGeometry.height() - m_editTPSetting->height()) / 2);

        m_editTPSetting->show();
        connect(m_editTPSetting, &ContainerSetting::destroyed,
                [=]
                {
                    m_editTPSetting->deleteLater();
                    m_editTPSetting = nullptr;
                });
        connect(m_editTPSetting, &ContainerSetting::sigUpdateTemplate,
                [=]
                {
                    getTemplateInfo();
                });
    }
}

void TemplateListPage::onDelete(int row)
{
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
            Container::getInstance().removeTemplate(m_objId, QList<int64_t>() << id);
        }
    }
}

void TemplateListPage::onCreateTemplate()
{
    if (!m_createTPSetting)
    {
        m_createTPSetting = new ContainerSetting(CONTAINER_SETTING_TYPE_TEMPLATE_CREATE);
        m_createTPSetting->setNodeInfos(m_nodeInfoMap);
        m_createTPSetting->setNetworkInfos(m_networksMap);
        m_createTPSetting->setImageList(m_imageInfos);

        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_createTPSetting->move(screenGeometry.x() + (screenGeometry.width() - m_createTPSetting->width()) / 2,
                                screenGeometry.y() + (screenGeometry.height() - m_createTPSetting->height()) / 2);

        m_createTPSetting->show();
        connect(m_createTPSetting, &ContainerSetting::destroyed,
                [=]
                {
                    m_createTPSetting->deleteLater();
                    m_createTPSetting = nullptr;
                });
        connect(m_createTPSetting, &ContainerSetting::sigUpdateTemplate,
                [=]
                {
                    getTemplateInfo();
                });
    }
}

void TemplateListPage::onRemoveTemplate()
{
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
            Container::getInstance().removeTemplate(m_objId, ids);
        }
    }
}

void TemplateListPage::getNetworkListResult(const QString objId, const QPair<grpc::Status, network::ListReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        m_networksMap.clear();
        for (auto ifs : reply.second.virtual_ifs())
        {
            int nodeId = ifs.node_id();
            auto name = ifs.name();
            auto subnet = ifs.ip_address() + "/" + std::to_string(ifs.ip_mask_len());
            // 后面加空格，是为了防止在3.3-6Nari系统中下拉列表项显示不全
            // TODO:QListView item计算中文长度有误，后续完善
            QString str = QString("%1 (%2:%3)    ")
                              .arg(QString::fromStdString(name))
                              .arg(tr("Subnet"))
                              .arg(QString::fromStdString(subnet));
            KLOG_DEBUG() << "Get network info. "
                         << "node id:" << nodeId << "network info:" << str;
            m_networksMap.insert(nodeId, str);
        }
    }
    else
        KLOG_WARNING() << "Get network list result failed: " << reply.first.error_message().data();
}

void TemplateListPage::getNodeListResult(QString objId, const QPair<Status, node::ListReply> &reply)
{
    if (m_objId != objId)
        return;

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Get node list result failed:" << reply.first.error_message().data();
        return;
    }

    m_nodeInfoMap.clear();
    for (auto n : reply.second.nodes())
    {
        auto nodeId = n.id();
        auto nodeInfo = new NodeInfo;
        nodeInfo->nodeID = nodeId;
        nodeInfo->nodeAddr = QString::fromStdString(n.address().data());
        nodeInfo->totalCPU = n.status().cpu_stat().total();
        nodeInfo->totalMemory = n.status().mem_stat().total();
        m_nodeInfoMap.insert(nodeId, nodeInfo);
    }
}

void TemplateListPage::getListImageFinishedResult(QString objId, const QPair<Status, image::ListReply> &reply)
{
    if (m_objId != objId)
        return;

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Get image list result failed:" << reply.first.error_message().data();
        return;
    }

    m_imageInfos.clear();
    for (auto info : reply.second.images())
    {
        auto image = QString::fromStdString(info.name());
        m_imageInfos.append(image);
    }
}

void TemplateListPage::getListTemplateFinishResult(const QString objId, const QPair<grpc::Status, container::ListTemplateReply> &reply)
{
    if (m_objId != objId)
        return;

    setBusy(false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Get template list failed!" << reply.first.error_message().data();
        if (reply.first.error_code() == PERMISSION_DENIED)
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
        else
        {
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
            if (grpc::StatusCode::DEADLINE_EXCEEDED == reply.first.error_code())
            {
                setTips(tr("Response timeout!"));
            }
        }
        setTableDefaultContent("-");
        setHeaderCheckable(false);
        return;
    }

    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
    clearTable();
    int size = reply.second.data_size();
    KLOG_DEBUG() << "Template size:" << size;
    if (size <= 0)
    {
        setTableDefaultContent("-");
        setHeaderCheckable(false);
        return;
    }
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

        // 基础配置
        // cpu,内存,网卡
        auto cpuNum = cfg.resouce_limit().cpu_limit();
        auto memoryNum = cfg.resouce_limit().memory_limit() / 1024;
        auto networkNum = cfg.networks_size();
        QString baseCfg = tr("CPU: %1 core \n Memory: %2 G \n Network: %3 piece")
                              .arg(cpuNum)
                              .arg(memoryNum)
                              .arg(networkNum);
        QStandardItem *itemBaseCfg = new QStandardItem(baseCfg);
        itemBaseCfg->setTextAlignment(Qt::AlignCenter);

        // 高级配置
        // 环境变量、共享目录、图形化使用、高可用
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

void TemplateListPage::getRemoveTemplateFinishResult(const QString objId, const QPair<grpc::Status, container::RemoveTemplateReply> &reply)
{
    if (m_objId != objId)
        return;

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
    connect(&Container::getInstance(), &Container::listTemplateFinished, this, &TemplateListPage::getListTemplateFinishResult);
    connect(&Network::getInstance(), &Network::listNetworkFinished, this, &TemplateListPage::getNetworkListResult);
    connect(&Container::getInstance(), &Container::removeTemplateFinished, this, &TemplateListPage::getRemoveTemplateFinishResult);
    connect(&Node::getInstance(), &Node::listNodeFinished, this, &TemplateListPage::getNodeListResult);
    connect(&Image::getInstance(), &Image::listImageFinished, this, &TemplateListPage::getListImageFinishedResult);
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
    KLOG_DEBUG() << "Get network info of node:" << node_id;
    InfoWorker::getInstance().listNetwork(m_objId, node_id);
}

void TemplateListPage::getTemplateInfo()
{
    setBusy(true);
    InfoWorker::getInstance().listTemplate(m_objId);
}

void TemplateListPage::getNodeInfo()
{
    InfoWorker::getInstance().listNode(m_objId);
}

void TemplateListPage::getImageInfo()
{
    InfoWorker::getInstance().listImage(m_objId);
}
