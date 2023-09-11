/**
 * @file          /ks-scmc-gui/src/pages/container/container-setting.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "container-setting.h"
#include <kiran-log/qt5-log-i.h>
#include <QComboBox>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPair>
#include "advanced-configuration/envs-conf-tab.h"
#include "advanced-configuration/high-availability-tab.h"
#include "advanced-configuration/volumes-conf-tab.h"
#include "base-configuration/cpu-conf-tab.h"
#include "base-configuration/memory-conf-tab.h"
#include "base-configuration/network-conf-tab.h"
#include "common/guide-item.h"
#include "common/message-dialog.h"
#include "def.h"
#include "notification-manager.h"
#include "security-configuration/network-access-ctl-tab.h"
#include "security-configuration/security-list-tab.h"
#include "security-configuration/start-stop-control-tab.h"
#include "ui_container-setting.h"

#define CPU "CPU"
#define MEMORY QObject::tr("Memory")
#define NETWORK_CARD QObject::tr("Network card")
#define ENVS QObject::tr("ENVS")
#define VOLUMES QObject::tr("Volumes")
#define HIGH_AVAILABILITY QObject::tr("High availability")
#define FILE_PROTECT QObject::tr("File protect")
#define PROCESS_SECURITY QObject::tr("Process security")
#define NETWORK_ACCESS_CONTROL QObject::tr("Network access control")
#define NETWORK_PROCESS_WHITE_LIST QObject::tr("Network process white list")
#define START_STOP_CONTROL QObject::tr("Start stop control")

const std::string TagContainerDescription = "TAG_CONTAINER_DESC";

ContainerSetting::ContainerSetting(ContainerSettingType type, QWidget *parent) : QWidget(parent),
                                                                                 ui(new Ui::ContainerSetting),
                                                                                 m_baseConfStack(nullptr),
                                                                                 m_advancedConfStack(nullptr),
                                                                                 m_securityConfStack(nullptr),
                                                                                 m_addMenu(nullptr),
                                                                                 m_templateId(-1),
                                                                                 m_netWorkCount(0),
                                                                                 m_type(type)

{
    ui->setupUi(this);
    m_objId = InfoWorker::generateId(this);

    initUI();
    initConnection();
}

ContainerSetting::~ContainerSetting()
{
    delete ui;
    m_netWorkPages.clear();
    m_advancedItems.clear();
    m_baseItems.clear();
}

void ContainerSetting::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ContainerSetting::setItems(int row, int col, QWidget *item)
{
    ui->gridLayout->addWidget(item, row, col);
}

void ContainerSetting::setTemplateList(QMultiMap<int, QPair<int, QString>> templateMap)
{
    m_templateMap = templateMap;
    auto iter = templateMap.begin();
    while (iter != templateMap.end())
    {
        qint64 templateId = iter.key();
        QPair<int, QString> templateInfo = iter.value();
        QString templateName = templateInfo.second;
        ui->cb_template->addItem(templateName, templateId);
        iter++;
    }
}

void ContainerSetting::setNodeInfos(QMap<int, NodeInfo *> nodeInfoMap)
{
    m_nodeInfoMap = nodeInfoMap;
    ui->cb_node->clear();
    auto iter = nodeInfoMap.begin();
    while (iter != nodeInfoMap.end())
    {
        auto nodeId = iter.key();
        auto nodeInfo = iter.value();
        ui->cb_node->addItem(nodeInfo->nodeAddr, nodeId);
        m_nodeTotalCPU.insert(nodeId, nodeInfo->totalCPU);
        m_nodeTotalMemory.insert(nodeId, nodeInfo->totalMemory);
        iter++;
    }

    //设置cpu核心数
    auto cpuPage = qobject_cast<CPUConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_CPU));
    cpuPage->setTotalCPU(m_nodeTotalCPU.value(ui->cb_node->currentData().toInt()));

    //设置节点内存总数
    auto memoryPage = qobject_cast<MemoryConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY));
    memoryPage->setTotalMemory(m_nodeTotalMemory.value(ui->cb_node->currentData().toInt()));
}

void ContainerSetting::setImageList(QStringList imageList)
{
    m_imageList = imageList;
    ui->cb_image->clear();
    ui->cb_image->addItems(imageList);
}

void ContainerSetting::setNetworkInfos(QMultiMap<int, QString> networksMap)
{
    m_networksMap = networksMap;
    setNodeNetworkList(ui->cb_node->currentData().toInt());
}

void ContainerSetting::getContainerInspect(int nodeID, const QString containerID)
{
    m_nodeID = nodeID;
    m_containerID = containerID;
    InfoWorker::getInstance().containerInspect(m_objId, nodeID, containerID.toStdString());
}

void ContainerSetting::getTemplateInspect()
{
    //获取模板下拉框中的当前模板绑定的模板id
    auto templateId = ui->cb_template->currentData().toInt();
    m_templateId = templateId;

    InfoWorker::getInstance().inspectTemplate(m_objId, templateId);
}

void ContainerSetting::getTemplateInspect(int templateID)
{
    m_templateId = templateID;
    InfoWorker::getInstance().inspectTemplate(m_objId, templateID);
}

bool ContainerSetting::eventFilter(QObject *obj, QEvent *ev)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(ev);
    if (obj == ui->btn_tab_add && mouseEvent->type() == QEvent::MouseButtonPress)
    {
        int x = ui->btn_tab_add->width() / 2 - m_addMenu->sizeHint().width() / 2;
        int y = ui->btn_tab_add->height() + 2;
        QPoint menuPos(x, y);
        m_addMenu->popup(ui->btn_tab_add->mapToGlobal(menuPos));
        return true;
    }
    return QWidget::eventFilter(obj, ev);
}

void ContainerSetting::initUI()
{
    setWindowIcon(QIcon(":/images/logo.png"));
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(840, 700);

    ui->lineEdit_name->setMaxLength(50);
    ui->lineEdit_name->setPlaceholderText(tr("Please input 2 to 50 characters"));
    ui->btn_tip_name->setIcon(QIcon(":/images/tips.svg"));
    ui->btn_tip_name->setToolTip(tr("Only letter, digit or ._- three special characters;\nThe first characters must be letter or digit"));
    ui->lineEdit_describe->setMaxLength(200);
    ui->lineEdit_describe->setPlaceholderText(tr("Please input 0 to 200 characters"));

    ui->tabWidget->setFocusPolicy(Qt::NoFocus);
    ui->tabWidget->setAttribute(Qt::WA_StyledBackground);
    ui->btn_tab_add->setIcon(QIcon(":/images/addition.svg"));
    ui->btn_tab_add->setText(tr("Add"));
    ui->btn_tab_add->installEventFilter(this);
    m_addMenu = new QMenu(this);
    QAction *act = m_addMenu->addAction(tr("Network"));
    act->setData(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_CARD);
    m_addMenu->setObjectName("addMenu");
    connect(m_addMenu, &QMenu::triggered, this, &ContainerSetting::onAddItem);

    m_baseConfStack = new QStackedWidget(ui->tab_base_config);
    QLayout *baseLayout = ui->tab_base_config->layout();
    baseLayout->addWidget(m_baseConfStack);

    m_advancedConfStack = new QStackedWidget(ui->tab_advanced_config);
    QLayout *advancedLayout = ui->tab_advanced_config->layout();
    advancedLayout->addWidget(m_advancedConfStack);

    m_securityConfStack = new QStackedWidget(ui->tab_security_config);
    QLayout *securityLayout = ui->tab_security_config->layout();
    securityLayout->addWidget(m_securityConfStack);

    //创建tabwidget中侧边栏
    QList<QPair<QString, QString>> baseConfItemInfo = {{CPU, ":/images/container-cpu.svg"},
                                                       {MEMORY, ":/images/container-memory.svg"},
                                                       {NETWORK_CARD, ":/images/container-net-card.svg"}};
    for (int i = 0; i < baseConfItemInfo.count(); i++)
    {
        QString name = baseConfItemInfo.at(i).first;
        GuideItem *item = createGuideItem(ui->listwidget_base_config,
                                          name,
                                          GUIDE_ITEM_TYPE_NORMAL,
                                          baseConfItemInfo.at(i).second);
        m_baseItems.append(item);
    }
    m_baseItems.first()->setSelected(true);
    ui->listwidget_base_config->setCurrentRow(0);

    QList<QPair<QString, QString>> advancedConfItemInfo = {{ENVS, ":/images/container-env.png"},
                                                           {VOLUMES, ":/images/container-volumes.png"},
                                                           {HIGH_AVAILABILITY, ":/images/container-high-avail.png"}};
    for (int i = 0; i < advancedConfItemInfo.count(); i++)
    {
        QString name = advancedConfItemInfo.at(i).first;
        GuideItem *item = createGuideItem(ui->listWidget_advanced_config,
                                          name,
                                          GUIDE_ITEM_TYPE_NORMAL,
                                          advancedConfItemInfo.at(i).second);
        m_advancedItems.append(item);
    }
    m_advancedItems.first()->setSelected(true);
    ui->listWidget_advanced_config->setCurrentRow(0);

    QList<QPair<QString, QString>> securityConfItemInfo = {{FILE_PROTECT, ":/images/container-file-protect.png"},
                                                           {PROCESS_SECURITY, ":/images/process-security.png"},
                                                           {NETWORK_ACCESS_CONTROL, ":/images/net-access-control.png"},
                                                           {NETWORK_PROCESS_WHITE_LIST, ":/images/net-process-white-list.png"},
                                                           {START_STOP_CONTROL, ":/images/cmd_operation.png"}};
    for (int i = 0; i < securityConfItemInfo.count(); i++)
    {
        QString name = securityConfItemInfo.at(i).first;
        GuideItem *item = createGuideItem(ui->listWidget_security_config,
                                          name,
                                          GUIDE_ITEM_TYPE_NORMAL,
                                          securityConfItemInfo.at(i).second);
        m_securityItems.append(item);
    }
    m_securityItems.first()->setSelected(true);
    ui->listWidget_security_config->setCurrentRow(0);

    initSummaryUI();
    initBaseConfPages();
    initAdvancedConfPages();
    initSecurityConfPages();

    QList<QComboBox *> cbList = this->findChildren<QComboBox *>();
    foreach (auto cb, cbList)
    {
        cb->setItemDelegate(new QStyledItemDelegate(this));
    }

    connect(ui->listwidget_base_config, &QListWidget::itemClicked, this, &ContainerSetting::onItemClicked);
    connect(ui->listWidget_advanced_config, &QListWidget::itemClicked, this, &ContainerSetting::onItemClicked);
    connect(ui->listWidget_security_config, &QListWidget::itemClicked, this, &ContainerSetting::onItemClicked);
    connect(ui->btn_confirm, &QToolButton::clicked, this, &ContainerSetting::onConfirm);
    connect(ui->btn_cancel, &QToolButton::clicked, this, &ContainerSetting::close);
    connect(ui->cb_node, QOverload<const QString &>::of(&QComboBox::activated), this, &ContainerSetting::onNodeSelectedChanged);
    connect(ui->cb_template, QOverload<const QString &>::of(&QComboBox::activated), this, &ContainerSetting::onTempSelectedChanged);
}

void ContainerSetting::initSummaryUI()
{
    ui->label_template_name->hide();
    ui->cb_template->hide();
    ui->lineEdit_describe->setTextMargins(10, 0, 0, 0);
    ui->lineEdit_name->setTextMargins(10, 0, 0, 0);
    switch (m_type)
    {
    case CONTAINER_SETTING_TYPE_CONTAINER_CREATE:
    {
        setWindowTitle(tr("Create Container"));
        break;
    }
    case CONTAINER_SETTING_TYPE_CONTAINER_EDIT:
    {
        setWindowTitle(tr("Edit Container"));
        ui->btn_tip_name->hide();
        ui->cb_image->setEnabled(false);
        ui->cb_image->setStyleSheet("#cb_image{border:none;margin-left:0px;padding-left:0px;}"
                                    "#cb_image::down-arrow{image:none;}");
        ui->cb_node->setEnabled(false);
        ui->cb_node->setStyleSheet("#cb_node{border:none;margin-left:0px;padding-left:0px;}"
                                   "#cb_node::down-arrow{image:none;}");
        ui->lineEdit_name->setReadOnly(true);
        ui->lineEdit_name->setStyleSheet("#lineEdit_name{border:none;background:#ffffff;}");
        ui->lineEdit_name->setTextMargins(0, 0, 0, 0);
        ui->lineEdit_describe->setTextMargins(0, 0, 0, 0);
        ui->lineEdit_describe->setReadOnly(true);
        ui->lineEdit_describe->setStyleSheet("QLineEdit{border:none;}"
                                             "QToolTip{"
                                             "background-color: rgb(255,255,255);"
                                             "color:#000000;"
                                             "border-radius: 6px;"
                                             "border:0px solid rgb(0,0,0);"
                                             "outline:none; "
                                             "min-height:30px;"
                                             "}");
        break;
    }
    case CONTAINER_SETTING_TYPE_TEMPLATE_CREATE:
        setWindowTitle(tr("Create template"));
        break;
    case CONTAINER_SETTING_TYPE_TEMPLATE_EDIT:
        setWindowTitle(tr("Edit template"));
        break;
    case CONTAINER_SETTING_TYPE_CONTAINER_CREATE_FROM_TEMPLATE:
    {
        setWindowTitle(tr("Create container from template"));
        ui->label_template_name->show();
        ui->cb_template->show();
        break;
    }
    case CONTAINER_SETTING_TYPE_CONTAINER_GENERATE_TEMPLATE:
    {
        setWindowTitle(tr("Generate template"));
        ui->cb_image->setEnabled(false);
        ui->cb_image->setStyleSheet("#cb_image{border:none;margin-left:0px;padding-left:0px;}"
                                    "#cb_image::down-arrow{image:none;}");
        break;
    }
    default:
        break;
    }
}

void ContainerSetting::initConnection()
{
    switch (m_type)
    {
    case CONTAINER_SETTING_TYPE_CONTAINER_EDIT:
        connect(&InfoWorker::getInstance(), &InfoWorker::containerInspectFinished, this, &ContainerSetting::getContainerInspectResult);
        connect(&InfoWorker::getInstance(), &InfoWorker::updateContainerFinished, this, &ContainerSetting::getUpdateContainerResult);
        break;
    case CONTAINER_SETTING_TYPE_CONTAINER_CREATE:
        connect(&InfoWorker::getInstance(), &InfoWorker::createContainerFinished, this, &ContainerSetting::getCreateContainerResult);
        break;
    case CONTAINER_SETTING_TYPE_TEMPLATE_CREATE:
        connect(&InfoWorker::getInstance(), &InfoWorker::createTemplateFinished, this, &ContainerSetting::getCreateTemplateFinishResult);
        break;
    case CONTAINER_SETTING_TYPE_TEMPLATE_EDIT:
        connect(&InfoWorker::getInstance(), &InfoWorker::inspectTemplateFinished, this, &ContainerSetting::getInspectTemplateFinishResult);
        connect(&InfoWorker::getInstance(), &InfoWorker::updateTemplateFinished, this, &ContainerSetting::getUpdateTemplateFinishedResult);
        break;
    case CONTAINER_SETTING_TYPE_CONTAINER_CREATE_FROM_TEMPLATE:
        connect(&InfoWorker::getInstance(), &InfoWorker::inspectTemplateFinished, this, &ContainerSetting::getInspectTemplateFinishResult);
        connect(&InfoWorker::getInstance(), &InfoWorker::createContainerFinished, this, &ContainerSetting::getCreateContainerResult);
        break;
    case CONTAINER_SETTING_TYPE_CONTAINER_GENERATE_TEMPLATE:
        connect(&InfoWorker::getInstance(), &InfoWorker::containerInspectFinished, this, &ContainerSetting::getContainerInspectResult);
        connect(&InfoWorker::getInstance(), &InfoWorker::createTemplateFinished, this, &ContainerSetting::getCreateTemplateFinishResult);
        break;
    default:
        break;
    }
}

GuideItem *ContainerSetting::createGuideItem(QListWidget *parent, QString text, int type, QString icon)
{
    QListWidgetItem *newItem = nullptr;
    GuideItem *customItem = nullptr;

    newItem = new QListWidgetItem(parent);

    customItem = new GuideItem(text, icon, type, parent);
    if (text == NETWORK_CARD)
    {
        customItem->setDeleteBtn();
        ++m_netWorkCount;
        if (m_netWorkCount == 1)
        {
            customItem->setName(QString("%1%2").arg(text).arg("1"));
            customItem->setDeleteBtnVisible(false);
        }
        connect(customItem, &GuideItem::sigDeleteItem, this, &ContainerSetting::onDelItem);
    }
    customItem->setTipLinePosition(TIP_LINE_POSITION_RIGHT);
    parent->addItem(newItem);
    parent->setItemWidget(newItem, customItem);
    newItem->setTextAlignment(Qt::AlignVCenter);
    newItem->setSizeHint(QSize(220, 30));
    return customItem;
}

void ContainerSetting::initBaseConfPages()
{
    CPUConfTab *cpuInfoTab = new CPUConfTab(ui->tab_base_config);
    m_baseConfStack->addWidget(cpuInfoTab);

    MemoryConfTab *memoryConfTab = new MemoryConfTab(ui->tab_base_config);
    m_baseConfStack->addWidget(memoryConfTab);

    NetworkConfTab *networkConfTab = new NetworkConfTab(ui->tab_base_config);
    m_baseConfStack->addWidget(networkConfTab);
    m_netWorkPages.append(networkConfTab);
}

void ContainerSetting::initAdvancedConfPages()
{
    EnvsConfTab *envsConfTab = new EnvsConfTab(ui->tab_advanced_config);
    m_advancedConfStack->addWidget(envsConfTab);

    VolumesConfTab *volumesConfTab = new VolumesConfTab(ui->tab_advanced_config);
    m_advancedConfStack->addWidget(volumesConfTab);

    HighAvailabilityTab *highAvailabilityTab = new HighAvailabilityTab(ui->tab_advanced_config);
    m_advancedConfStack->addWidget(highAvailabilityTab);
}

void ContainerSetting::initSecurityConfPages()
{
    SecurityListTab *fileProtectTab = new SecurityListTab(PROTECT_FILE_LIST, ui->tab_security_config);
    m_securityConfStack->addWidget(fileProtectTab);

    SecurityListTab *processWhiteList = new SecurityListTab(EXEC_WHITELIST, ui->tab_security_config);
    m_securityConfStack->addWidget(processWhiteList);

    NetworkAccessCtlTab *netAccessCtlTab = new NetworkAccessCtlTab(ui->tab_security_config);
    m_securityConfStack->addWidget(netAccessCtlTab);

    SecurityListTab *netWorkProcessWhiteList = new SecurityListTab(NET_EXEC_WHITELIST, ui->tab_security_config);
    m_securityConfStack->addWidget(netWorkProcessWhiteList);

    StartStopControlTab *startStopCtlTab = new StartStopControlTab(ui->tab_security_config);
    m_securityConfStack->addWidget(startStopCtlTab);
}

void ContainerSetting::showLongText(QLineEdit *lineEdit, QString orgText)
{
    QString newStr = orgText;
    QFontMetrics fontWidth(lineEdit->font());
    QString elideNote = fontWidth.elidedText(newStr, Qt::ElideRight, lineEdit->width());
    lineEdit->setText(elideNote);
    lineEdit->setToolTip(tooptipWordWrap(orgText));
    lineEdit->setCursorPosition(0);
}

QString ContainerSetting::tooptipWordWrap(const QString &org)
{
    QString result;
    QFontMetrics fm(fontMetrics());
    int textWidthInPxs = fm.width(org);
    const int rear = org.length();
    int pre = 0, vernier = 1;
    unsigned int pickUpWidthPxs = 0;
    QString pickUp;
    unsigned int curLen = 0;

    if (textWidthInPxs <= width())
    {
        result = org;
        return result;
    }

    while (vernier <= rear)
    {
        curLen = vernier - pre;
        pickUp = org.mid(pre, curLen);
        pickUpWidthPxs = fm.width(pickUp);
        if (pickUpWidthPxs >= width())
        {
            result += pickUp + "\n";  // 插入换行符，或者使用<br/>标签
            pre = vernier;
            pickUpWidthPxs = 0;
        }
        ++vernier;
    }

    if (pickUpWidthPxs < width() && !pickUp.isEmpty())
    {
        result += pickUp;
    }

    return result;
}

void ContainerSetting::deleteItem(QString itemText, int row)
{
    if (itemText == NETWORK_CARD)
    {
        QListWidgetItem *delItem = ui->listwidget_base_config->takeItem(row);
        m_baseItems.removeAt(row);
        auto page = m_baseConfStack->widget(row);
        m_baseConfStack->removeWidget(page);
        m_netWorkPages.removeOne(qobject_cast<NetworkConfTab *>(page));
        --m_netWorkCount;

        delete page;
        page = nullptr;
        delete delItem;
        delItem = nullptr;
    }
}

void ContainerSetting::updateRemovableItem(QString itemText)
{
    int count = 1;
    if (m_netWorkCount > 1)
    {
        for (int i = 0; i < m_baseItems.count(); i++)
        {
            GuideItem *item = m_baseItems.at(i);
            if (item->getItemText() == itemText)
            {
                item->setDeleteBtnVisible(true);
                item->setName(QString("%1%2").arg(itemText).arg(count++));
            }
        }
    }
    else if (m_netWorkCount == 1)
    {
        for (int i = 0; i < m_baseItems.count(); i++)
        {
            GuideItem *item = m_baseItems.at(i);
            if (item->getItemText() == itemText)
            {
                item->setDeleteBtnVisible(false);
                item->setName(QString("%1%2").arg(itemText).arg(count++));
            }
        }
    }
}

void ContainerSetting::setNodeNetworkList(int nodeId)
{
    QList<QString> networks = m_networksMap.values(nodeId);
    KLOG_INFO() << networks;
    foreach (auto networkPage, m_netWorkPages)
    {
        networkPage->initVirtNetworkInfo(networks);
    }
}

bool ContainerSetting::writeContainerConfig(container::ContainerConfigs *cntrCfg)
{
    QString errMsg = "";

    //volume mounts
    auto volumePage = qobject_cast<VolumesConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_VOLUMES));
    if (!volumePage->getVolumeInfo(cntrCfg, errMsg))
    {
        MessageDialog::message(tr("Volumes Data"),
                               tr("Input error"),
                               errMsg,
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return false;
    }

    //network
    foreach (auto networkPage, m_netWorkPages)
    {
        networkPage->getNetworkInfo(cntrCfg);
    }

    //env
    auto envPage = qobject_cast<EnvsConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_ENVS));
    if (!envPage->getEnvInfo(cntrCfg, errMsg))
    {
        MessageDialog::message(tr("Env Data"),
                               tr("Input error"),
                               errMsg,
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return false;
    }

    //High
    auto policy = cntrCfg->mutable_restart_policy();
    auto highAvailabilityPage = qobject_cast<HighAvailabilityTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_HIGH_AVAILABILITY));
    highAvailabilityPage->getRestartPolicy(policy);

    auto limit = cntrCfg->mutable_resouce_limit();

    //cpu
    auto cpuPage = qobject_cast<CPUConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_CPU));
    if (!cpuPage->getCPUInfo(limit, errMsg))
    {
        MessageDialog::message(tr("CPU Data"),
                               tr("Input error"),
                               errMsg,
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return false;
    }

    //memory
    auto memoryPage = qobject_cast<MemoryConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY));
    if (!memoryPage->getMemoryInfo(limit, errMsg))
    {
        MessageDialog::message(tr("Memory Data"),
                               tr("Input error"),
                               errMsg,
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return false;
    }

    //security
    auto securityCfg = cntrCfg->mutable_security_config();

    auto fileProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_FILE_PROTECT));
    if (!fileProtectPage->getSecurityListInfo(securityCfg, errMsg))
    {
        MessageDialog::message(windowTitle(),
                               tr("Input error"),
                               errMsg,
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return false;
    }
    auto processProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_PROCESS_SECURITY));
    if (!processProtectPage->getSecurityListInfo(securityCfg, errMsg))
    {
        MessageDialog::message(windowTitle(),
                               tr("Input error"),
                               errMsg,
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return false;
    }

    auto netProcessProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_PROCESS_WHITE_LIST));
    if (!netProcessProtectPage->getSecurityListInfo(securityCfg, errMsg))
    {
        MessageDialog::message(windowTitle(),
                               tr("Input error"),
                               errMsg,
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return false;
    }

    auto networkAccessCtlPage = qobject_cast<NetworkAccessCtlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_ACCESS_CONTROL));
    networkAccessCtlPage->getNetworkAccessInfo(securityCfg);

    auto startStopCtlPage = qobject_cast<StartStopControlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_START_STOP_CONTROL));
    securityCfg->set_disable_cmd_operation(startStopCtlPage->getStartStopInfo());

    return true;
}

void ContainerSetting::createContainer()
{
    container::CreateRequest request;
    request.set_node_id(ui->cb_node->currentData().toInt());
    auto cntrCfg = request.mutable_configs();
    //cntrCfg->set_container_id("");
    cntrCfg->set_uuid("");
    cntrCfg->set_name(ui->lineEdit_name->text().toStdString());
    //cntrCfg->set_status("");
    cntrCfg->set_desc(ui->lineEdit_describe->text().toStdString());
    cntrCfg->set_image(ui->cb_image->currentText().toStdString());

    if (writeContainerConfig(cntrCfg))
    {
        InfoWorker::getInstance().createContainer(m_objId, request);
    }
}

void ContainerSetting::updateContainer()
{
    QString errMsg = "";
    container::UpdateRequest request;
    request.set_node_id(m_nodeID);
    request.set_container_id(m_containerID.toStdString());

    auto rsrcCfg = request.mutable_resource_limit();
    auto cpuPage = qobject_cast<CPUConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_CPU));
    if (!cpuPage->getCPUInfo(rsrcCfg, errMsg))
    {
        MessageDialog::message(tr("CPU Data"),
                               tr("Input error"),
                               errMsg,
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    auto memoryPage = qobject_cast<MemoryConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY));
    if (!memoryPage->getMemoryInfo(rsrcCfg, errMsg))
    {
        MessageDialog::message(tr("Memory Data"),
                               tr("Input error"),
                               errMsg,
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    auto policy = request.mutable_restart_policy();
    auto highAvailabilityPage = qobject_cast<HighAvailabilityTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_HIGH_AVAILABILITY));
    highAvailabilityPage->getRestartPolicy(policy);

    foreach (auto networkPage, m_netWorkPages)
    {
        networkPage->getNetworkInfo(&request);
    }

    //security
    auto securityCfg = request.mutable_security_config();

    auto fileProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_FILE_PROTECT));
    if (!fileProtectPage->getSecurityListInfo(securityCfg, errMsg))
    {
        MessageDialog::message(windowTitle(),
                               tr("Input error"),
                               errMsg,
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    auto processProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_PROCESS_SECURITY));
    if (!processProtectPage->getSecurityListInfo(securityCfg, errMsg))
    {
        MessageDialog::message(windowTitle(),
                               tr("Input error"),
                               errMsg,
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    auto netProcessProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_PROCESS_WHITE_LIST));
    if (!netProcessProtectPage->getSecurityListInfo(securityCfg, errMsg))
    {
        MessageDialog::message(windowTitle(),
                               tr("Input error"),
                               errMsg,
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    auto networkAccessCtlPage = qobject_cast<NetworkAccessCtlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_ACCESS_CONTROL));
    networkAccessCtlPage->getNetworkAccessInfo(securityCfg);

    auto startStopCtlPage = qobject_cast<StartStopControlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_START_STOP_CONTROL));
    securityCfg->set_disable_cmd_operation(startStopCtlPage->getStartStopInfo());

    InfoWorker::getInstance().updateContainer(m_objId, request);
}

void ContainerSetting::createTemplate()
{
    container::CreateTemplateRequest request;

    auto data = request.mutable_data();
    data->set_node_id(ui->cb_node->currentData().toInt());

    auto cntrCfg = data->mutable_conf();

    cntrCfg->set_name(ui->lineEdit_name->text().toStdString());
    cntrCfg->set_desc(ui->lineEdit_describe->text().toStdString());
    KLOG_INFO() << "********** get template image:" << ui->cb_image->currentText();
    cntrCfg->set_image(ui->cb_image->currentText().toStdString());

    if (writeContainerConfig(cntrCfg))
    {
        InfoWorker::getInstance().createTemplate(m_objId, request);
    }
}

void ContainerSetting::updateTemplate()
{
    container::UpdateTemplateRequest request;
    auto data = request.mutable_data();
    data->set_id(m_templateId);
    data->set_node_id(ui->cb_node->currentData().toInt());

    auto cntrCfg = data->mutable_conf();
    cntrCfg->set_name(ui->lineEdit_name->text().toStdString());
    cntrCfg->set_desc(ui->lineEdit_describe->text().toStdString());
    KLOG_INFO() << "********** get template image:" << ui->cb_image->currentText();
    cntrCfg->set_image(ui->cb_image->currentText().toStdString());

    if (writeContainerConfig(cntrCfg))
    {
        InfoWorker::getInstance().updateTemplate(m_objId, request);
    }
}

void ContainerSetting::onItemClicked(QListWidgetItem *item)
{
    QListWidget *listwidget = qobject_cast<QListWidget *>(sender());
    GuideItem *guideItem = qobject_cast<GuideItem *>(listwidget->itemWidget(item));
    int index = listwidget->row(item);
    if (listwidget == ui->listwidget_base_config)
    {
        m_baseConfStack->setCurrentIndex(index);
        foreach (GuideItem *item, m_baseItems)
        {
            if (item == guideItem)
                item->setSelected(true);
            else
                item->setSelected(false);
        }
    }

    else if (listwidget == ui->listWidget_advanced_config)
    {
        m_advancedConfStack->setCurrentIndex(index);
        foreach (GuideItem *item, m_advancedItems)
        {
            if (item == guideItem)
                item->setSelected(true);
            else
                item->setSelected(false);
        }
    }
    else if (listwidget == ui->listWidget_security_config)
    {
        m_securityConfStack->setCurrentIndex(index);
        foreach (GuideItem *item, m_securityItems)
        {
            if (item == guideItem)
                item->setSelected(true);
            else
                item->setSelected(false);
        }
    }
}

void ContainerSetting::onAddItem(QAction *action)
{
    int type = action->data().toInt();
    switch (type)
    {
    case TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_CARD:
    {
        GuideItem *item = createGuideItem(ui->listwidget_base_config,
                                          NETWORK_CARD,
                                          GUIDE_ITEM_TYPE_NORMAL,
                                          ":/images/container-net-card.svg");
        m_baseItems.append(item);
        updateRemovableItem(NETWORK_CARD);
        NetworkConfTab *networkConfPage = new NetworkConfTab(this);
        m_netWorkPages.append(networkConfPage);
        m_baseConfStack->addWidget(networkConfPage);
        int nodeId = ui->cb_node->currentData().toInt();
        networkConfPage->initVirtNetworkInfo(m_networksMap.values(nodeId));
        break;
    }
    default:
        break;
    }
}

void ContainerSetting::onDelItem()
{
    GuideItem *guideItem = qobject_cast<GuideItem *>(sender());
    auto ret = MessageDialog::message(tr("Delete Network card"),
                                      tr("Are you sure you want to delete the network card?"),
                                      tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                      ":/images/warning.svg",
                                      MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
    if (ret == MessageDialog::StandardButton::Yes)
    {
        int row = 0;
        while (row < ui->listwidget_base_config->count() && m_netWorkCount > 1)
        {
            QListWidgetItem *item = ui->listwidget_base_config->item(row);
            if (ui->listwidget_base_config->itemWidget(item) == guideItem)
            {
                deleteItem(NETWORK_CARD, row);
                updateRemovableItem(NETWORK_CARD);
                break;
            }
            row++;
        }
    }
}

void ContainerSetting::onConfirm()
{
    switch (m_type)
    {
    case CONTAINER_SETTING_TYPE_CONTAINER_CREATE:
    case CONTAINER_SETTING_TYPE_CONTAINER_CREATE_FROM_TEMPLATE:
        createContainer();
        break;
    case CONTAINER_SETTING_TYPE_CONTAINER_EDIT:
        updateContainer();
        break;
    case CONTAINER_SETTING_TYPE_TEMPLATE_CREATE:
        createTemplate();
        break;
    case CONTAINER_SETTING_TYPE_TEMPLATE_EDIT:
        updateTemplate();
        break;
    case CONTAINER_SETTING_TYPE_CONTAINER_GENERATE_TEMPLATE:
        createTemplate();
        break;
    default:
        break;
    }
}

void ContainerSetting::onNodeSelectedChanged(QString newStr)
{
    //更新网络列表
    setNodeNetworkList(ui->cb_node->currentData().toInt());

    //更新cpu最大值
    auto cpuPage = qobject_cast<CPUConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_CPU));
    cpuPage->setTotalCPU(m_nodeTotalCPU.value(ui->cb_node->currentData().toInt()));

    //设置节点内存总数
    auto memoryPage = qobject_cast<MemoryConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY));
    memoryPage->setTotalMemory(m_nodeTotalMemory.value(ui->cb_node->currentData().toInt()));
}

void ContainerSetting::onTempSelectedChanged(QString newStr)
{
    int index = ui->cb_template->findText(newStr);
    auto templateId = ui->cb_template->itemData(index).toInt();

    getTemplateInspect(templateId);

    QPair<int, QString> templateInfo = m_templateMap.value(templateId);
    qint64 nodeId = templateInfo.first;
    ui->cb_node->setCurrentIndex(ui->cb_node->findData(nodeId));
}

void ContainerSetting::getCreateContainerResult(QString objId, const QPair<grpc::Status, container::CreateReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        emit sigUpdateContainer();
        close();
    }
    else
    {
        KLOG_DEBUG() << QString::fromStdString(reply.first.error_message());
        MessageDialog::message(tr("Create Container"),
                               tr("Create container failed!"),
                               tr("Error: ") + reply.first.error_message().data(),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ContainerSetting::getContainerInspectResult(QString objId, const QPair<grpc::Status, container::InspectReply> &reply)
{
    if (m_objId != objId)
        return;

    if (!reply.first.ok())
        return;

    //init ui
    auto info = reply.second.configs();

    //名字
    showLongText(ui->lineEdit_name, info.name().data());
    KLOG_INFO() << "container name:" << info.name().data() << "image:" << info.image().data();
    //描述
    if (!QString::fromStdString(info.desc().data()).isEmpty())
    {
        showLongText(ui->lineEdit_describe, info.desc().data());
    }
    else
        ui->lineEdit_describe->setText(tr("none"));
    //镜像
    ui->cb_image->setCurrentText(info.image().data());
    //节点
    ui->cb_node->setCurrentIndex(ui->cb_node->findData(m_nodeID));

    // network
    auto size = info.networks_size();
    KLOG_INFO() << "network config size:" << size;
    for (int i = 0; i < size - 1; ++i)
    {
        //创建侧边栏页stacked页，由于初始页面已经创建过一次，创建个数-1
        GuideItem *item = createGuideItem(ui->listwidget_base_config,
                                          NETWORK_CARD,
                                          GUIDE_ITEM_TYPE_NORMAL,
                                          ":/images/container-net-card.svg");
        m_baseItems.append(item);
        NetworkConfTab *networkConfTab = new NetworkConfTab(ui->tab_base_config);
        m_baseConfStack->addWidget(networkConfTab);
        m_netWorkPages.append(networkConfTab);
    }
    updateRemovableItem(NETWORK_CARD);

    //更新网络页面信息
    QList<QString> networkList = m_networksMap.values(m_nodeID);
    for (int i = 0; i < size; i++)
    {
        auto networkConfig = info.networks(i);
        NetworkConfTab *networkPage = m_netWorkPages.at(i);
        if (m_type == CONTAINER_SETTING_TYPE_CONTAINER_GENERATE_TEMPLATE)
            networkConfig.set_ip_address("");
        networkPage->setNetworkInfo(&networkConfig, networkList);  //设置网卡列表和网卡信息
    }

    //volume
    auto volumesPage = qobject_cast<VolumesConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_VOLUMES));
    volumesPage->setVolumeInfo(&info);

    //env
    auto envPage = qobject_cast<EnvsConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_ENVS));
    envPage->setEnvInfo(&info);

    if (m_type == CONTAINER_SETTING_TYPE_CONTAINER_EDIT)
    {
        volumesPage->setDisabled(true);
        envPage->setDisabled(true);
    }

    //high-availability
    auto highAvailabilityPage = qobject_cast<HighAvailabilityTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_HIGH_AVAILABILITY));
    auto policy = info.restart_policy();
    highAvailabilityPage->setRestartPolicy(&policy);

    auto limit = info.resouce_limit();
    //cpu
    auto cpuPage = qobject_cast<CPUConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_CPU));
    cpuPage->setCPUInfo(&limit);

    //memory
    auto memoryPage = qobject_cast<MemoryConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY));
    memoryPage->setMemoryInfo(&limit);

    //security
    auto securityCfg = info.security_config();

    auto fileProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_FILE_PROTECT));
    fileProtectPage->setSecurityListInfo(&securityCfg);

    auto processProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_PROCESS_SECURITY));
    processProtectPage->setSecurityListInfo(&securityCfg);

    auto netProcessProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_PROCESS_WHITE_LIST));
    netProcessProtectPage->setSecurityListInfo(&securityCfg);

    auto networkAccessCtlPage = qobject_cast<NetworkAccessCtlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_ACCESS_CONTROL));
    networkAccessCtlPage->setNetworkAccessInfo(&securityCfg);

    auto startStopCtlPage = qobject_cast<StartStopControlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_START_STOP_CONTROL));
    startStopCtlPage->setStartStopInfo(securityCfg.disable_cmd_operation());
}

void ContainerSetting::getUpdateContainerResult(QString objId, const QPair<grpc::Status, container::UpdateReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        sigUpdateContainer();
        close();
    }
    else
    {
        MessageDialog::message(tr("Update Container"),
                               tr("Update container failed!"),
                               tr("Error: %1").arg(reply.first.error_message().data()),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ContainerSetting::getInspectTemplateFinishResult(QString objId, const QPair<grpc::Status, container::InspectTemplateReply> &reply)
{
    if (m_objId != objId)
        return;

    if (!reply.first.ok())
        return;

    //init ui
    int nodeID = reply.second.data().node_id();
    KLOG_INFO() << "template node id: " << nodeID;

    auto info = reply.second.data().conf();

    //名字
    ui->lineEdit_name->setText(info.name().data());
    ui->lineEdit_name->setCursorPosition(0);
    //描述
    if (!QString::fromStdString(info.desc().data()).isEmpty())
        ui->lineEdit_describe->setText(info.desc().data());
    else
        ui->lineEdit_describe->setText(tr("none"));
    ui->lineEdit_describe->setCursorPosition(0);
    //节点
    ui->cb_node->setCurrentIndex(ui->cb_node->findData(nodeID));
    //镜像
    KLOG_INFO() << "!!!!!!! template image: " << QString::fromStdString(info.image());
    ui->cb_image->setCurrentText(QString::fromStdString(info.image()));

    //volume
    auto volumesPage = qobject_cast<VolumesConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_VOLUMES));
    volumesPage->setVolumeInfo(&info);

    // network
    auto size = info.networks_size();
    KLOG_INFO() << "network config size:" << size;
    if (size > m_netWorkPages.size())
    {
        for (int i = 0; i < (size - m_netWorkPages.size()); ++i)
        {
            //创建网卡tab页，由于初始页面已经创建过一次，创建个数 - m_netWorkPages.size()
            GuideItem *item = createGuideItem(ui->listwidget_base_config,
                                              NETWORK_CARD,
                                              GUIDE_ITEM_TYPE_NORMAL,
                                              ":/images/container-net-card.svg");
            m_baseItems.append(item);
            NetworkConfTab *networkConfTab = new NetworkConfTab(ui->tab_base_config);
            m_baseConfStack->addWidget(networkConfTab);
            m_netWorkPages.append(networkConfTab);
        }
    }
    else if (size < m_netWorkPages.size())
    {
        //删除页面上多余的网卡tab页
        int row = 0;
        int count = m_netWorkPages.size() - size;
        while (row < ui->listwidget_base_config->count() && m_netWorkCount > 1 && count > 0)
        {
            QListWidgetItem *item = ui->listwidget_base_config->item(row);
            GuideItem *guidItem = qobject_cast<GuideItem *>(ui->listwidget_base_config->itemWidget(item));
            if (guidItem->getItemText() == NETWORK_CARD)
            {
                deleteItem(NETWORK_CARD, row);
                count--;
            }
            row++;
        }
    }

    updateRemovableItem(NETWORK_CARD);
    //更新网络页面信息
    QList<QString> networkList = m_networksMap.values(nodeID);
    for (int i = 0; i < size; i++)
    {
        auto networkConfig = info.networks(i);
        NetworkConfTab *networkPage = m_netWorkPages.at(i);
        networkPage->setNetworkInfo(&networkConfig, networkList);  //设置网卡列表和网卡信息
    }

    //env
    auto envPage = qobject_cast<EnvsConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_ENVS));
    envPage->setEnvInfo(&info);

    //high-availability
    auto highAvailabilityPage = qobject_cast<HighAvailabilityTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_HIGH_AVAILABILITY));
    auto policy = info.restart_policy();
    highAvailabilityPage->setRestartPolicy(&policy);

    auto limit = info.resouce_limit();
    //cpu
    auto cpuPage = qobject_cast<CPUConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_CPU));
    cpuPage->setCPUInfo(&limit);

    //memory
    auto memoryPage = qobject_cast<MemoryConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY));
    memoryPage->setMemoryInfo(&limit);

    //security
    auto securityCfg = info.security_config();

    auto fileProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_FILE_PROTECT));
    fileProtectPage->setSecurityListInfo(&securityCfg);

    auto processProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_PROCESS_SECURITY));
    processProtectPage->setSecurityListInfo(&securityCfg);

    auto netProcessProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_PROCESS_WHITE_LIST));
    netProcessProtectPage->setSecurityListInfo(&securityCfg);

    auto networkAccessCtlPage = qobject_cast<NetworkAccessCtlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_ACCESS_CONTROL));
    networkAccessCtlPage->setNetworkAccessInfo(&securityCfg);

    auto startStopCtlPage = qobject_cast<StartStopControlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_START_STOP_CONTROL));
    startStopCtlPage->setStartStopInfo(securityCfg.disable_cmd_operation());
}

void ContainerSetting::getCreateTemplateFinishResult(QString objId, const QPair<grpc::Status, container::CreateTemplateReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        if (m_type == CONTAINER_SETTING_TYPE_CONTAINER_GENERATE_TEMPLATE)
            NotificationManager::sendNotify(tr("Create template successful!"), tr("You can see it in container template page."));
        emit sigUpdateTemplate();
        close();
    }
    else
    {
        MessageDialog::message(tr("Create template"),
                               tr("Create template failed!"),
                               tr("error: %1").arg(reply.first.error_message().data()),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ContainerSetting::getUpdateTemplateFinishedResult(QString objId, const QPair<grpc::Status, container::UpdateTemplateReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        emit sigUpdateTemplate();
        close();
    }
    else
    {
        MessageDialog::message(tr("Update template"),
                               tr("Update template failed!"),
                               tr("error: %1").arg(reply.first.error_message().data()),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}
