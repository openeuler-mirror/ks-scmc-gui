/**
 * @file          /ks-scmc-gui/src/pages/container/template-manager/template-list-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "template-list-page.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFormLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>
#include "authorization-dialog.h"
#include "def.h"
#include "message-dialog.h"

using namespace grpc;

TemplateImportDialog::TemplateImportDialog(QWidget *parent) : KiranTitlebarWindow(parent, Qt::Dialog)
{
    setTitle(tr("Import Template"));
    setFixedSize(500, 250);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint | KiranTitlebarWindow::TitlebarCloseButtonHint);

    // 创建控件
    auto nameLabel = new QLabel(tr("Template Name:"));
    m_templateNameEdit = new QLineEdit();
    m_templateNameEdit->setPlaceholderText(tr("Please enter template name"));
    m_templateNameEdit->setFixedHeight(32);
    m_templateNameEdit->setMaxLength(50);

    auto nameTipBtn = new QToolButton();
    nameTipBtn->setFixedSize(16, 16);
    nameTipBtn->setIcon(QIcon(":/images/tips.svg"));
    nameTipBtn->setToolTip(tr("Only letter, digit or ._- three special characters;\nThe first characters must be letter or digit"));

    auto fileLabel = new QLabel(tr("Template Config File:"));
    m_templateFileEdit = new QLineEdit();
    m_templateFileEdit->setPlaceholderText(tr("Please select template config file"));
    m_templateFileEdit->setFixedHeight(32);

    m_selectFileBtn = new QPushButton();
    m_selectFileBtn->setIcon(QIcon(":/images/folder-open.svg"));
    m_selectFileBtn->setFixedSize(32, 32);

    // 连接信号槽
    connect(m_selectFileBtn, &QPushButton::clicked, this, &TemplateImportDialog::onSelectFile);

    // 输入布局
    auto gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(10);
    gridLayout->setVerticalSpacing(10);
    gridLayout->addWidget(nameLabel, 0, 0);
    gridLayout->addWidget(m_templateNameEdit, 0, 1);
    gridLayout->addWidget(nameTipBtn, 0, 2);
    gridLayout->addWidget(fileLabel, 1, 0);
    gridLayout->addWidget(m_templateFileEdit, 1, 1);
    gridLayout->addWidget(m_selectFileBtn, 1, 2);

    // 按钮布局
    auto btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);

    auto confirmBtn = new QPushButton(tr("Confirm"));
    confirmBtn->setFixedSize(100, 40);
    Kiran::WidgetPropertyHelper::setButtonType(confirmBtn, Kiran::BUTTON_Default);
    auto cancelBtn = new QPushButton(tr("Cancel"));
    cancelBtn->setFixedSize(100, 40);

    btnLayout->addStretch();
    btnLayout->addWidget(confirmBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();

    connect(confirmBtn, &QPushButton::clicked, this, &TemplateImportDialog::fileSelected);
    connect(cancelBtn, &QPushButton::clicked, this, &TemplateImportDialog::close);

    auto mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->addLayout(gridLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    this->getWindowContentWidget()->setLayout(mainLayout);
}

QString TemplateImportDialog::getTemplateName() const
{
    return m_templateNameEdit->text();
}

QString TemplateImportDialog::getTemplateFile() const
{
    return m_templateFileEdit->text();
}

void TemplateImportDialog::clear()
{
    m_templateNameEdit->clear();
    m_templateFileEdit->clear();
}

void TemplateImportDialog::onSelectFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Select Template Config File"),
        "",
        tr("Config Files (*.conf)"));

    if (!fileName.isEmpty())
    {
        m_templateFileEdit->setText(fileName);
    }
}

TemplateListPage::TemplateListPage(QWidget *parent) : TablePage(parent),
                                                      m_createTPSetting(nullptr),
                                                      m_editTPSetting(nullptr),
                                                      m_importDialog(nullptr)
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

void TemplateListPage::onImportTemplate()
{
    if (!m_importDialog)
    {
        m_importDialog = new TemplateImportDialog();
        connect(m_importDialog, &TemplateImportDialog::destroyed, [=]
                { m_importDialog = nullptr; });
        connect(m_importDialog, &TemplateImportDialog::fileSelected, this, [this]
                {
                auto templateFile = m_importDialog->getTemplateFile();
                auto templateName = m_importDialog->getTemplateName();
                // 检查输入
                if (templateName.isEmpty())
                {
                    MessageDialog::message(tr("Import Template"),
                                           tr("Import template failed"),
                                           tr("Please enter template name!"),
                                           ":/images/warning.svg",
                                           MessageDialog::StandardButton::Ok);
                    return;
                }

                if (templateFile.isEmpty())
                {
                    MessageDialog::message(tr("Import Template"),
                                           tr("Import template failed"),
                                           tr("Please select template config file!"),
                                           ":/images/warning.svg",
                                           MessageDialog::StandardButton::Ok);
                    return;
                }

                // 读取并解析配置文件
                QFile file(templateFile);
                if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    MessageDialog::message(tr("Import Template"),
                                           tr("Failed to open config file!"),
                                           tr("Cannot read file %1.").arg(templateFile),
                                           ":/images/error.svg",
                                           MessageDialog::StandardButton::Ok);
                    return;
                }
                QByteArray jsonData = file.readAll();
                file.close();
                parseJsonToTemplate(jsonData, templateName); });
    }

    m_importDialog->clear();
    m_importDialog->show();
}

void TemplateListPage::onExportTemplate()
{
    // 用户认证
    auto authDialog = new AuthorizationDialog(this);
    QString user;
    UserConfiguration::getInstance().readConfig(CONFIG_SETTING_TYPE_LOGIN, GROUP_CURRENT_USER, USERNAME, user);
    authDialog->setUserName(user);
    if (authDialog->exec() != QDialog::Accepted)
    {
        return;
    }

    QList<int64_t> ids;
    getCheckedItemsId(ids);

    if (ids.empty())
    {
        MessageDialog::message(tr("Export Template"),
                               tr("Export template failed"),
                               tr("Please select the template to export."),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    // 弹出文件对话框，让用户选择保存路径
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    fileDialog.setWindowTitle(tr("Select Export Directory"));

    if (fileDialog.exec() != QDialog::Accepted)
    {
        return;  // 用户取消操作
    }

    QStringList selectedPaths = fileDialog.selectedFiles();
    if (selectedPaths.isEmpty())
    {
        return;
    }

    QString exportPath = selectedPaths.first();
    if (!exportPath.endsWith("/"))
    {
        exportPath += "/";
    }

    exportTemplate(ids, exportPath);
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
void TemplateListPage::getCreateTemplateFinishResult(const QString objId, const QPair<grpc::Status, container::CreateTemplateReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        m_importDialog->close();

        getTemplateInfo();  // 刷新模板列表
    }
    else
    {
        MessageDialog::message(tr("Import Template"),
                               tr("Failed to import template!"),
                               tr("Error: %1").arg(QString::fromStdString(reply.first.error_message())),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
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
    // 清空之前的JSON数据
    m_templateJsonMap.clear();

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

        // 将ContainerTemplate转换为JSON格式并存储
        QString templateJson = convertTemplateToJson(data);
        m_templateJsonMap[tempId] = templateJson;

        QStandardItem *itemCheck = new QStandardItem();
        itemCheck->setCheckable(true);

        QStandardItem *itemName = new QStandardItem(cfg.name().data());
        itemName->setData(QVariant::fromValue(idMap));
        itemName->setTextAlignment(Qt::AlignCenter);

        QStandardItem *itemDesc = new QStandardItem(cfg.desc().data());
        itemDesc->setTextAlignment(Qt::AlignCenter);

        // 基础配置
        // cpu,内存,网卡
        auto cpuNum = cfg.resource_limit().cpu_limit();
        auto memoryNum = cfg.resource_limit().memory_limit() / 1024;
        auto networkNum = cfg.networks_size();
        QString baseCfg = tr("CPU: %1 core \n Memory: %2 G \n Network: %3 piece")
                              .arg(cpuNum)
                              .arg(memoryNum)
                              .arg(networkNum);
        QStandardItem *itemBaseCfg = new QStandardItem(baseCfg);
        itemBaseCfg->setTextAlignment(Qt::AlignCenter);

        // 高级配置
        // 环境变量、高可用
        // auto envNum = cfg.envs().size();
         auto envNum = cfg.envs_size();
        auto restartPolicy = cfg.restart_policy().name().data();
        QMap<QString, QString> policyMap = {{"no", tr("no")},
                                            {"always", tr("always")},
                                            {"on-failure", tr("on-failure")},
                                            {"unless-stopped", tr("unless-stopped")}};

        QString advanceCfg = tr("Env: %1 \nRestartPolicy: %2")
                                 .arg(envNum)
                                 .arg(policyMap.value(restartPolicy));
        QStandardItem *itemAdvanceCfg = new QStandardItem(advanceCfg);
        itemAdvanceCfg->setTextAlignment(Qt::AlignCenter);

        // 安全等级
        auto level = cfg.security_config().security_level_template();

        QString levelText;
        switch (level)
        {
        case LEVEL_NUM_1:
            levelText = LEVEL_1;
            break;
        case LEVEL_NUM_2:
            levelText = LEVEL_2;
            break;
        case LEVEL_NUM_3:
            levelText = LEVEL_3;
            break;
        case LEVEL_NUM_4:
            levelText = LEVEL_4;
            break;
        case LEVEL_NUM_5:
            levelText = LEVEL_5;
            break;
        default:
            levelText = tr("None");
            break;
        }
        QStandardItem *itemLevel = new QStandardItem(levelText);
        itemLevel->setTextAlignment(Qt::AlignCenter);

        setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemDesc << itemBaseCfg << itemAdvanceCfg << itemLevel);
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
        QString(tr("Security Level")),
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
    Kiran::WidgetPropertyHelper::setButtonType(btnCreate, Kiran::BUTTON_Default);
    connect(btnCreate, &QPushButton::clicked, this, &TemplateListPage::onCreateTemplate);

    QPushButton *btnRemove = new QPushButton(this);
    btnRemove->setText(tr("Remove"));
    btnRemove->setObjectName("btnRemove");
    btnRemove->setFixedSize(QSize(78, 32));
    connect(btnRemove, &QPushButton::clicked, this, &TemplateListPage::onRemoveTemplate);

    auto btnImport = new QPushButton(this);
    btnImport->setText(tr("Import"));
    btnImport->setObjectName("btnImport");
    btnImport->setFixedSize(QSize(78, 32));
    Kiran::WidgetPropertyHelper::setButtonType(btnImport, Kiran::BUTTON_Default);
    connect(btnImport, &QPushButton::clicked, this, &TemplateListPage::onImportTemplate);

    auto btnExport = new QPushButton(this);
    btnExport->setText(tr("Export"));
    btnExport->setObjectName("btnExport");
    btnExport->setFixedSize(QSize(78, 32));
    connect(btnExport, &QPushButton::clicked, this, &TemplateListPage::onExportTemplate);

    addSingleOperationButtons(QList<QPushButton *>() << btnCreate << btnImport);
    addBatchOperationButtons(QList<QPushButton *>() << btnExport << btnRemove);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void TemplateListPage::initTemplateConnect()
{
    // 连接模板相关信号
    connect(&Container::getInstance(), &Container::createTemplateFinished, this,
            &TemplateListPage::getCreateTemplateFinishResult);
    connect(&Container::getInstance(), &Container::listTemplateFinished, this,
            &TemplateListPage::getListTemplateFinishResult);
    connect(&Network::getInstance(), &Network::listNetworkFinished, this,
            &TemplateListPage::getNetworkListResult);
    connect(&Container::getInstance(), &Container::removeTemplateFinished, this,
            &TemplateListPage::getRemoveTemplateFinishResult);
    connect(&Node::getInstance(), &Node::listNodeFinished, this,
            &TemplateListPage::getNodeListResult);
    connect(&Image::getInstance(), &Image::listImageFinished, this,
            &TemplateListPage::getListImageFinishedResult);
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

QString TemplateListPage::convertTemplateToJson(const container::ContainerTemplate &data)
{
    QJsonObject templateJson;
    templateJson["id"] = static_cast<qint64>(data.id());
    templateJson["node_id"] = static_cast<qint64>(data.node_id());

    // 转换ContainerConfigs
    auto cfg = data.conf();
    QJsonObject configJson;
    configJson["container_id"] = QString::fromStdString(cfg.container_id());
    configJson["uuid"] = QString::fromStdString(cfg.uuid());
    configJson["name"] = QString::fromStdString(cfg.name());
    configJson["status"] = QString::fromStdString(cfg.status());
    configJson["desc"] = QString::fromStdString(cfg.desc());
    configJson["image"] = QString::fromStdString(cfg.image());
    configJson["enable_graphic"] = cfg.enable_graphic();
    configJson["cmd"] = QString::fromStdString(cfg.cmd());
    configJson["ipcname"] = QString::fromStdString(cfg.ipcname());
    configJson["hostname"] = QString::fromStdString(cfg.hostname());

    // 转换Mounts
    QJsonArray mountsArray;
    for (int i = 0; i < cfg.mounts_size(); ++i)
    {
        auto mount = cfg.mounts(i);
        QJsonObject mountJson;
        mountJson["type"] = QString::fromStdString(mount.type());
        mountJson["source"] = QString::fromStdString(mount.source());
        mountJson["target"] = QString::fromStdString(mount.target());
        mountJson["read_only"] = mount.read_only();
        mountJson["consistency"] = QString::fromStdString(mount.consistency());
        mountsArray.append(mountJson);
    }
    configJson["mounts"] = mountsArray;

    // 转换Networks
    QJsonArray networksArray;
    for (int i = 0; i < cfg.networks_size(); ++i)
    {
        auto network = cfg.networks(i);
        QJsonObject networkJson;
        networkJson["interface"] = QString::fromStdString(network.interface());
        networkJson["container_id"] = QString::fromStdString(network.container_id());
        networkJson["ip_address"] = QString::fromStdString(network.ip_address());
        networkJson["ip_prefix_len"] = network.ip_prefix_len();
        networkJson["mac_address"] = QString::fromStdString(network.mac_address());
        networkJson["gateway"] = QString::fromStdString(network.gateway());
        networksArray.append(networkJson);
    }
    configJson["networks"] = networksArray;

    // 转换Envs
    QJsonObject envsObject;
    auto envs = cfg.envs();
    for (auto it = envs.begin(); it != envs.end(); ++it)
    {
        envsObject[QString::fromStdString(it->first)] = QString::fromStdString(it->second);
    }
    configJson["envs"] = envsObject;

    // 转换RestartPolicy
    QJsonObject restartPolicyJson;
    restartPolicyJson["name"] = QString::fromStdString(cfg.restart_policy().name());
    restartPolicyJson["max_retry"] = cfg.restart_policy().max_retry();
    configJson["restart_policy"] = restartPolicyJson;

    // 转换ResourceLimit
    QJsonObject resourceLimitJson;
    resourceLimitJson["cpu_limit"] = cfg.resource_limit().cpu_limit();
    resourceLimitJson["cpu_prio"] = static_cast<qint64>(cfg.resource_limit().cpu_prio());
    resourceLimitJson["memory_limit"] = cfg.resource_limit().memory_limit();
    resourceLimitJson["memory_soft_limit"] = cfg.resource_limit().memory_soft_limit();
    resourceLimitJson["disk_limit"] = cfg.resource_limit().disk_limit();
    resourceLimitJson["disk_prio"] = static_cast<qint64>(cfg.resource_limit().disk_prio());
    resourceLimitJson["disk_rbw"] = static_cast<qint64>(cfg.resource_limit().disk_rbw());
    resourceLimitJson["disk_wbw"] = static_cast<qint64>(cfg.resource_limit().disk_wbw());
    configJson["resource_limit"] = resourceLimitJson;

    // 转换ContainerMiscConfig
    QJsonObject miscConfigJson;
    miscConfigJson["app_run"] = cfg.misc_config().app_run();
    configJson["misc_config"] = miscConfigJson;

    // 转换SecurityConfig
    QJsonObject securityConfigJson;
    securityConfigJson["disable_external_network"] = cfg.security_config().disable_external_network();
    securityConfigJson["disable_cmd_operation"] = cfg.security_config().disable_cmd_operation();
    securityConfigJson["security_level_template"] = static_cast<qint64>(cfg.security_config().security_level_template());
    securityConfigJson["network_access_controll"] = cfg.security_config().network_access_controll();

    // 转换ProcProtection - proc_protection
    auto procProtection = cfg.security_config().proc_protection();
    QJsonObject procProtectionJson;
    procProtectionJson["protection_type"] = static_cast<qint64>(procProtection.protection_type());
    procProtectionJson["is_on"] = procProtection.is_on();
    QJsonArray exeListArray;
    for (int i = 0; i < procProtection.exe_list_size(); ++i)
    {
        exeListArray.append(QString::fromStdString(procProtection.exe_list(i)));
    }
    procProtectionJson["exe_list"] = exeListArray;
    securityConfigJson["proc_protection"] = procProtectionJson;

    // 转换ProcProtection - nproc_protection
    auto nprocProtection = cfg.security_config().nproc_protection();
    QJsonObject nprocProtectionJson;
    nprocProtectionJson["protection_type"] = static_cast<qint64>(nprocProtection.protection_type());
    nprocProtectionJson["is_on"] = nprocProtection.is_on();
    QJsonArray nprocExeListArray;
    for (int i = 0; i < nprocProtection.exe_list_size(); ++i)
    {
        nprocExeListArray.append(QString::fromStdString(nprocProtection.exe_list(i)));
    }
    nprocProtectionJson["exe_list"] = nprocExeListArray;
    securityConfigJson["nproc_protection"] = nprocProtectionJson;

    // 转换FileProtection
    auto fileProtection = cfg.security_config().file_protection();
    QJsonObject fileProtectionJson;
    fileProtectionJson["is_on"] = fileProtection.is_on();
    QJsonArray fileListArray;
    for (int i = 0; i < fileProtection.file_list_size(); ++i)
    {
        fileListArray.append(QString::fromStdString(fileProtection.file_list(i)));
    }
    fileProtectionJson["file_list"] = fileListArray;
    securityConfigJson["file_protection"] = fileProtectionJson;

    // 转换NetworkRuleList
    auto networkRuleList = cfg.security_config().network_rule();
    QJsonObject networkRuleListJson;
    networkRuleListJson["is_on"] = networkRuleList.is_on();
    QJsonArray rulesArray;
    for (int i = 0; i < networkRuleList.rules_size(); ++i)
    {
        auto rule = networkRuleList.rules(i);
        QJsonObject ruleJson;
        QJsonArray protocolsArray;
        for (int j = 0; j < rule.protocols_size(); ++j)
        {
            protocolsArray.append(QString::fromStdString(rule.protocols(j)));
        }
        ruleJson["protocols"] = protocolsArray;
        ruleJson["addr"] = QString::fromStdString(rule.addr());
        ruleJson["port"] = static_cast<qint64>(rule.port());
        ruleJson["drop"] = rule.drop();
        rulesArray.append(ruleJson);
    }
    networkRuleListJson["rules"] = rulesArray;
    securityConfigJson["network_rule"] = networkRuleListJson;

    configJson["security_config"] = securityConfigJson;

    templateJson["conf"] = configJson;

    // 将JSON转换为字符串并返回
    QJsonDocument doc(templateJson);
    return doc.toJson(QJsonDocument::Indented);
}

void TemplateListPage::parseJsonToTemplate(const QByteArray &jsonData, const QString &templateName)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);
    if (jsonError.error != QJsonParseError::NoError)
    {
        MessageDialog::message(tr("Import Template"),
                               tr("Invalid config file format!"),
                               tr("JSON parse error"),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    if (!jsonDoc.isObject())
    {
        MessageDialog::message(tr("Import Template"),
                               tr("Invalid config file format!"),
                               tr("Root element is not a JSON object."),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    QJsonObject rootObj = jsonDoc.object();
    QJsonObject confObj = rootObj["conf"].toObject();

    // 创建模板请求
    container::CreateTemplateRequest request;
    auto data = request.mutable_data();
    data->set_node_id(rootObj["node_id"].toInt());

    auto configs = data->mutable_conf();

    // 填充基本配置
    configs->set_name(templateName.toStdString());
    configs->set_desc(confObj["desc"].toString().toStdString());
    configs->set_image(confObj["image"].toString().toStdString());
    configs->set_enable_graphic(confObj["enable_graphic"].toBool());
    configs->set_cmd(confObj["cmd"].toString().toStdString());
    configs->set_ipcname(confObj["ipcname"].toString().toStdString());
    configs->set_hostname(confObj["hostname"].toString().toStdString());

    // 填充资源限制
    QJsonObject resourceLimitObj = confObj["resource_limit"].toObject();
    auto resourceLimit = configs->mutable_resource_limit();
    resourceLimit->set_cpu_limit(resourceLimitObj["cpu_limit"].toDouble());
    resourceLimit->set_cpu_prio(resourceLimitObj["cpu_prio"].toInt());
    resourceLimit->set_memory_limit(resourceLimitObj["memory_limit"].toDouble());
    resourceLimit->set_memory_soft_limit(resourceLimitObj["memory_soft_limit"].toDouble());
    resourceLimit->set_disk_limit(resourceLimitObj["disk_limit"].toDouble());
    resourceLimit->set_disk_prio(resourceLimitObj["disk_prio"].toInt());
    resourceLimit->set_disk_rbw(resourceLimitObj["disk_rbw"].toInt());
    resourceLimit->set_disk_wbw(resourceLimitObj["disk_wbw"].toInt());

    // 填充重启策略
    QJsonObject restartPolicyObj = confObj["restart_policy"].toObject();
    auto restartPolicy = configs->mutable_restart_policy();
    restartPolicy->set_name(restartPolicyObj["name"].toString().toStdString());
    restartPolicy->set_max_retry(restartPolicyObj["max_retry"].toInt());

    // 填充环境变量
    QJsonObject envsObj = confObj["envs"].toObject();
    for (auto it = envsObj.begin(); it != envsObj.end(); ++it)
    {
        (*configs->mutable_envs())[it.key().toStdString()] = it.value().toString().toStdString();
    }

    // 填充挂载点
    QJsonArray mountsArray = confObj["mounts"].toArray();
    for (const auto &mountValue : mountsArray)
    {
        QJsonObject mountObj = mountValue.toObject();
        auto mount = configs->add_mounts();
        mount->set_type(mountObj["type"].toString().toStdString());
        mount->set_source(mountObj["source"].toString().toStdString());
        mount->set_target(mountObj["target"].toString().toStdString());
        mount->set_read_only(mountObj["read_only"].toBool());
        mount->set_consistency(mountObj["consistency"].toString().toStdString());
    }

    // 填充网络配置
    QJsonArray networksArray = confObj["networks"].toArray();
    for (const auto &networkValue : networksArray)
    {
        QJsonObject networkObj = networkValue.toObject();
        auto network = configs->add_networks();
        network->set_interface(networkObj["interface"].toString().toStdString());
        network->set_container_id(networkObj["container_id"].toString().toStdString());
        network->set_ip_address(networkObj["ip_address"].toString().toStdString());
        network->set_ip_prefix_len(networkObj["ip_prefix_len"].toInt());
        network->set_mac_address(networkObj["mac_address"].toString().toStdString());
        network->set_gateway(networkObj["gateway"].toString().toStdString());
    }

    // 填充Misc配置
    QJsonObject miscConfigObj = confObj["misc_config"].toObject();
    auto miscConfig = configs->mutable_misc_config();
    miscConfig->set_app_run(miscConfigObj["app_run"].toBool());

    // 填充安全配置
    QJsonObject securityConfigObj = confObj["security_config"].toObject();
    auto securityConfig = configs->mutable_security_config();
    securityConfig->set_disable_external_network(securityConfigObj["disable_external_network"].toBool());
    securityConfig->set_disable_cmd_operation(securityConfigObj["disable_cmd_operation"].toBool());
    securityConfig->set_security_level_template(securityConfigObj["security_level_template"].toInt());
    securityConfig->set_network_access_controll(securityConfigObj["network_access_controll"].toBool());

    // 填充进程保护配置
    QJsonObject procProtectionObj = securityConfigObj["proc_protection"].toObject();
    auto procProtection = securityConfig->mutable_proc_protection();
    procProtection->set_protection_type(procProtectionObj["protection_type"].toInt());
    procProtection->set_is_on(procProtectionObj["is_on"].toBool());
    QJsonArray exeListArray = procProtectionObj["exe_list"].toArray();
    for (const auto &exeValue : exeListArray)
    {
        procProtection->add_exe_list(exeValue.toString().toStdString());
    }

    // 填充网络进程保护配置
    QJsonObject nprocProtectionObj = securityConfigObj["nproc_protection"].toObject();
    auto nprocProtection = securityConfig->mutable_nproc_protection();
    nprocProtection->set_protection_type(nprocProtectionObj["protection_type"].toInt());
    nprocProtection->set_is_on(nprocProtectionObj["is_on"].toBool());
    QJsonArray nprocExeListArray = nprocProtectionObj["exe_list"].toArray();
    for (const auto &exeValue : nprocExeListArray)
    {
        nprocProtection->add_exe_list(exeValue.toString().toStdString());
    }

    // 填充文件保护配置
    QJsonObject fileProtectionObj = securityConfigObj["file_protection"].toObject();
    auto fileProtection = securityConfig->mutable_file_protection();
    fileProtection->set_is_on(fileProtectionObj["is_on"].toBool());
    QJsonArray fileListArray = fileProtectionObj["file_list"].toArray();
    for (const auto &fileValue : fileListArray)
    {
        fileProtection->add_file_list(fileValue.toString().toStdString());
    }

    // 填充网络规则配置
    QJsonObject networkRuleListObj = securityConfigObj["network_rule"].toObject();
    auto networkRuleList = securityConfig->mutable_network_rule();
    networkRuleList->set_is_on(networkRuleListObj["is_on"].toBool());
    QJsonArray rulesArray = networkRuleListObj["rules"].toArray();
    for (const auto &ruleValue : rulesArray)
    {
        QJsonObject ruleObj = ruleValue.toObject();
        security::NetworkRule *rule = networkRuleList->add_rules();
        QJsonArray protocolsArray = ruleObj["protocols"].toArray();
        for (const auto &protocolValue : protocolsArray)
        {
            rule->add_protocols(protocolValue.toString().toStdString());
        }
        rule->set_addr(ruleObj["addr"].toString().toStdString());
        rule->set_port(ruleObj["port"].toInt());
        rule->set_drop(ruleObj["drop"].toBool());
    }

    Container::getInstance().createTemplate(m_objId, request);
}

void TemplateListPage::exportTemplate(const QList<int64_t> &ids, const QString &exportPath)
{
    // 保存每个选中的模板
    int successCount = 0;
    QStringList failedTemplates;

    for (auto id : ids)
    {
        // 获取模板名称用于文件名
        auto templateName = "template_" + QString::number(id);

        auto templateJson = m_templateJsonMap[id];
        if (templateJson.isEmpty())
        {
            KLOG_WARNING() << "Template JSON data is empty for ID:" << id;
            failedTemplates.append(templateName);
            continue;
        }

        // 从表格中查找对应的模板名称
        for (int row = 0; row < getRowCount(); ++row)
        {
            auto item = getItem(row, 1);  // 模板名称列
            if (item)
            {
                auto idMap = item->data().value<QMap<QString, QVariant>>();
                if (idMap.value(TEMPLATE_ID).toInt() == id)
                {
                    templateName = item->text();
                    break;
                }
            }
        }

        // 确保文件名合法
        QString fileName = exportPath + templateName + ".conf";

        // 保存文件
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << templateJson;
            file.close();
            successCount++;
            KLOG_INFO() << "Successfully exported template to:" << fileName;
        }
        else
        {
            KLOG_WARNING() << "Failed to export template to:" << fileName;
            failedTemplates.append(templateName);
        }
    }

    // 显示操作结果
    if (successCount > 0 && failedTemplates.isEmpty())
    {
        // 全部成功
        MessageDialog::message(tr("Export Template"),
                               tr("Successfully exported %1 template(s)!").arg(successCount),
                               tr("Templates exported to: %1").arg(exportPath),
                               ":/images/success.svg",
                               MessageDialog::StandardButton::Ok);
    }
    else if (successCount > 0 && !failedTemplates.isEmpty())
    {
        // 部分成功
        MessageDialog::message(tr("Export Template"),
                               tr("Successfully exported %1 template(s)!").arg(successCount),
                               tr("Failed to export: %1").arg(failedTemplates.join(", ")),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
    }
    else
    {
        // 全部失败
        MessageDialog::message(tr("Export Template"),
                               tr("Failed to export templates!"),
                               tr("Please check the selected directory permissions."),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void TemplateListPage::getNetworkInfo(int64_t node_id)
{
    KLOG_DEBUG() << "Get network info of node:" << node_id;
    Network::getInstance().listNetwork(m_objId, node_id);
}

void TemplateListPage::getTemplateInfo()
{
    setBusy(true);
    Container::getInstance().listTemplate(m_objId);
}

void TemplateListPage::getNodeInfo()
{
    Node::getInstance().listNode(m_objId);
}

void TemplateListPage::getImageInfo()
{
    Image::getInstance().listImage(m_objId);
}
