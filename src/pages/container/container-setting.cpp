#include "container-setting.h"
#include <kiran-log/qt5-log-i.h>
#include <QComboBox>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPair>
#include "advanced-configuration/envs-conf-tab.h"
#include "advanced-configuration/graphic-conf-tab.h"
#include "advanced-configuration/high-availability-tab.h"
#include "advanced-configuration/volumes-conf-tab.h"
#include "base-configuration/cpu-conf-tab.h"
#include "base-configuration/memory-conf-tab.h"
#include "base-configuration/network-conf-tab.h"
#include "common/guide-item.h"
#include "common/message-dialog.h"
#include "def.h"
#include "security-configuration/network-access-ctl-tab.h"
#include "security-configuration/security-list-tab.h"
#include "security-configuration/start-stop-control-tab.h"
#include "ui_container-setting.h"

#define CPU "CPU"
#define MEMORY QObject::tr("Memory")
#define NETWORK_CARD QObject::tr("Network card")
#define ENVS QObject::tr("ENVS")
#define GRAPHIC QObject::tr("Graphic")
#define VOLUMES QObject::tr("Volumes")
#define HIGH_AVAILABILITY QObject::tr("High availability")
#define FILE_PROTECT QObject::tr("File protect")
#define PROCESS_SECURITY QObject::tr("Process security")
#define NETWORK_ACCESS_CONTROL QObject::tr("Network access control")
#define NETWORK_PROCESS_WHITE_LIST QObject::tr("Network process white list")
#define START_STOP_CONTROL QObject::tr("Start stop control")

const std::string TagContainerDescription = "TAG_CONTAINER_DESC";

ContainerSetting::ContainerSetting(ContainerSettingType type, QMap<QString, QVariant> ids, QWidget *parent) : QWidget(parent),
                                                                                                              ui(new Ui::ContainerSetting),
                                                                                                              m_baseConfStack(nullptr),
                                                                                                              m_advancedConfStack(nullptr),
                                                                                                              m_securityConfStack(nullptr),
                                                                                                              m_addMenu(nullptr),
                                                                                                              m_cbImage(nullptr),
                                                                                                              m_cbNode(nullptr),
                                                                                                              m_labImage(nullptr),
                                                                                                              m_templateId(-1),
                                                                                                              m_netWorkCount(0),
                                                                                                              m_type(type),
                                                                                                              m_totalCPU(0.0)

{
    ui->setupUi(this);
    initUI();
    disconnect(&InfoWorker::getInstance(), &InfoWorker::listNodeFinished, 0, 0);
    connect(&InfoWorker::getInstance(), &InfoWorker::listNodeFinished, this, &ContainerSetting::getNodeListResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::listImageFinished, this, &ContainerSetting::getListImageFinishedResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::listNetworkFinished, this, &ContainerSetting::getNetworkListResult);

    m_containerIds.first = ids.value(NODE_ID).toInt();
    m_containerIds.second = ids.value(CONTAINER_ID).toString();
    m_templateId = ids.value(TEMPLATE_ID).toInt();

    getNodeInfo();
    if (type == CONTAINER_SETTING_TYPE_CONTAINER_EDIT)
    {
        if (!ids.isEmpty())
        {
            getContainerInspect();
        }
        connect(&InfoWorker::getInstance(), &InfoWorker::containerInspectFinished, this, &ContainerSetting::getContainerInspectResult);
        connect(&InfoWorker::getInstance(), &InfoWorker::updateContainerFinished, this, &ContainerSetting::getUpdateContainerResult);
    }
    else if (type == CONTAINER_SETTING_TYPE_CONTAINER_CREATE)
    {
        connect(&InfoWorker::getInstance(), &InfoWorker::createContainerFinished, this, &ContainerSetting::getCreateContainerResult);
    }
    else if (type == CONTAINER_SETTING_TYPE_TEMPLATE_CREATE)
    {
        connect(&InfoWorker::getInstance(), &InfoWorker::createTemplateFinished, this, &ContainerSetting::getCreateTemplateFinishResult);
    }
    else if (type == CONTAINER_SETTING_TYPE_TEMPLATE_EDIT)
    {
        if (!ids.isEmpty())
        {
            getTemplateInspect();
        }
        connect(&InfoWorker::getInstance(), &InfoWorker::inspectTemplateFinished, this, &ContainerSetting::getInspectTemplateFinishResult);
        connect(&InfoWorker::getInstance(), &InfoWorker::updateTemplateFinished, this, &ContainerSetting::getUpdateTemplateFinishedResult);
    }
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

bool ContainerSetting::eventFilter(QObject *obj, QEvent *ev)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(ev);
    if (obj == ui->btn_add && mouseEvent->type() == QEvent::MouseButtonPress)
    {
        int x = ui->btn_add->width() / 2 - m_addMenu->sizeHint().width() / 2;
        int y = -m_addMenu->sizeHint().height() - 2;
        QPoint menuPos(x, y);
        m_addMenu->popup(ui->btn_add->mapToGlobal(menuPos));
        return true;
    }
    return false;
}

void ContainerSetting::initUI()
{
    setWindowIcon(QIcon(":/images/logo.png"));
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(840, 700);

    ui->tabWidget->setFocusPolicy(Qt::NoFocus);
    ui->tabWidget->setAttribute(Qt::WA_StyledBackground);
    ui->btn_add->setIcon(QIcon(":/images/addition.svg"));
    ui->btn_add->setText(tr("Add"));
    ui->btn_add->installEventFilter(this);
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
                                                           {GRAPHIC, ":/images/container-graphic.png"},
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
    KLOG_INFO() << cbList.size();
    foreach (auto cb, cbList)
    {
        cb->setItemDelegate(new QStyledItemDelegate(this));
    }

    connect(ui->listwidget_base_config, &QListWidget::itemClicked, this, &ContainerSetting::onItemClicked);
    connect(ui->listWidget_advanced_config, &QListWidget::itemClicked, this, &ContainerSetting::onItemClicked);
    connect(ui->listWidget_security_config, &QListWidget::itemClicked, this, &ContainerSetting::onItemClicked);
    connect(ui->btn_confirm, &QToolButton::clicked, this, &ContainerSetting::onConfirm);
    connect(ui->btn_cancel, &QToolButton::clicked, this, &ContainerSetting::close);
    connect(ui->cb_node, QOverload<const QString &>::of(&QComboBox::currentTextChanged), this, &ContainerSetting::onNodeSelectedChanged);
}

void ContainerSetting::initSummaryUI()
{
    switch (m_type)
    {
    case CONTAINER_SETTING_TYPE_CONTAINER_CREATE:
    {
        setWindowTitle(tr("Create Container"));
        m_cbImage = new QComboBox(this);
        m_cbImage->setParent(this);
        m_cbImage->setFixedSize(QSize(200, 30));
        m_cbImage->setItemDelegate(new QStyledItemDelegate(this));
        QGridLayout *layout = dynamic_cast<QGridLayout *>(ui->page_container->layout());
        layout->addWidget(m_cbImage, 2, 1);
        break;
    }
    case CONTAINER_SETTING_TYPE_CONTAINER_EDIT:
    {
        setWindowTitle(tr("Edit Container"));
        m_labImage = new QLabel(this);
        QGridLayout *layout = dynamic_cast<QGridLayout *>(ui->page_container->layout());
        layout->addWidget(m_labImage, 2, 1);
        ui->cb_node->setEnabled(false);
        ui->cb_node->setStyleSheet("border:none;margin-left:0px;padding-left:0px;");
        ui->lineEdit_name->setReadOnly(true);
        ui->lineEdit_name->setStyleSheet("#lineEdit_name{border:none;background:#ffffff;}");
        ui->lineEdit_describe->setStyleSheet("border:none;");
        ui->lineEdit_describe->setReadOnly(true);
        break;
    }
    case CONTAINER_SETTING_TYPE_TEMPLATE_CREATE:
        setWindowTitle(tr("Create template"));
        ui->label_image->hide();
        break;
    case CONTAINER_SETTING_TYPE_TEMPLATE_EDIT:
        setWindowTitle(tr("Edit template"));
        ui->label_image->hide();
        ui->lineEdit_name->setReadOnly(true);
        ui->lineEdit_name->setStyleSheet("#lineEdit_name{border:none;background:#ffffff;}");
        ui->lineEdit_describe->setStyleSheet("border:none;");
        ui->lineEdit_describe->setReadOnly(true);
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
            customItem->setDeleteBtnVisible(false);
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
    CPUConfTab *cpuInfoTab = new CPUConfTab(m_totalCPU, ui->tab_base_config);
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

    GraphicConfTab *graphicConfTab = new GraphicConfTab(ui->tab_advanced_config);
    m_advancedConfStack->addWidget(graphicConfTab);

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

void ContainerSetting::getContainerInspect()
{
    InfoWorker::getInstance().containerInspect(m_containerIds.first, m_containerIds.second.toStdString());
}

void ContainerSetting::getTemplateInspect()
{
    InfoWorker::getInstance().inspectTemplate(m_templateId);
}

void ContainerSetting::getNodeInfo()
{
    InfoWorker::getInstance().listNode();
}

void ContainerSetting::getImageInfo(int64_t node_id)
{
    KLOG_INFO() << "getImageInfo" << node_id;
    InfoWorker::getInstance().listImage(node_id);
}

void ContainerSetting::createContainer()
{
    container::CreateRequest request;
    ErrorCode ret;
    request.set_node_id(m_nodeInfo.key(ui->cb_node->currentText()));
    auto cntrCfg = request.mutable_configs();
    //cntrCfg->set_container_id("");
    cntrCfg->set_uuid("");
    cntrCfg->set_name(ui->lineEdit_name->text().toStdString());
    //cntrCfg->set_status("");
    cntrCfg->set_desc(ui->lineEdit_describe->text().toStdString());
    if (m_cbImage)
        cntrCfg->set_image(m_cbImage->currentText().toStdString());
    //Graph
    auto graphicPage = qobject_cast<GraphicConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_GRAPHIC));
    cntrCfg->set_enable_graphic(graphicPage->isGraphic());

    //volume mounts
    auto volumePage = qobject_cast<VolumesConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_VOLUMES));
    ret = volumePage->getVolumeInfo(cntrCfg);
    if (ret == INPUT_NULL_ERROR)
    {
        MessageDialog::message(tr("Volumes Data"),
                               tr("Input error"),
                               tr("Please improve the contents in Volumes table!"),
                               tr(":/images/error.svg"),
                               MessageDialog::StandardButton::Ok);
        return;
    }

    foreach (auto networkPage, m_netWorkPages)
    {
        networkPage->getNetworkInfo(cntrCfg);
    }

    //env
    auto envPage = qobject_cast<EnvsConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_ENVS));
    ret = envPage->getEnvInfo(cntrCfg);
    if (ret == INPUT_NULL_ERROR)
    {
        MessageDialog::message(tr("Env Data"),
                               tr("Input error"),
                               tr("Please improve the contents in Env table!"),
                               tr(":/images/error.svg"),
                               MessageDialog::StandardButton::Ok);
        return;
    }

    //High
    auto policy = cntrCfg->mutable_restart_policy();
    auto highAvailabilityPage = qobject_cast<HighAvailabilityTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_HIGH_AVAILABILITY));
    highAvailabilityPage->getRestartPolicy(policy);

    auto limit = cntrCfg->mutable_resouce_limit();

    //cpu
    auto cpuPage = qobject_cast<CPUConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_CPU));
    cpuPage->getCPUInfo(limit);

    //memory
    auto memoryPage = qobject_cast<MemoryConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY));
    ret = memoryPage->getMemoryInfo(limit);
    if (ret == INPUT_ARG_ERROR)
    {
        MessageDialog::message(tr("Memory Data"),
                               tr("Input error"),
                               tr("Memory soft limit can't be greater than the maximum limit !"),
                               tr(":/images/error.svg"),
                               MessageDialog::StandardButton::Ok);
        return;
    }

    //limit->set_disk_limit();

    //security
    auto securityCfg = cntrCfg->mutable_security_config();

    auto fileProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_FILE_PROTECT));
    fileProtectPage->getSecurityListInfo(securityCfg);

    //    auto processProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_PROCESS_SECURITY));
    //    processProtectPage->getSecurityListInfo(securityCfg);

    auto netProcessProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_PROCESS_WHITE_LIST));
    netProcessProtectPage->getSecurityListInfo(securityCfg);

    auto networkAccessCtlPage = qobject_cast<NetworkAccessCtlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_ACCESS_CONTROL));
    networkAccessCtlPage->getNetworkAccessInfo(securityCfg);

    auto startStopCtlPage = qobject_cast<StartStopControlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_START_STOP_CONTROL));
    securityCfg->set_disable_cmd_operation(startStopCtlPage->getStartStopInfo());

    InfoWorker::getInstance().createContainer(request);
}

void ContainerSetting::updateContainer()
{
    container::UpdateRequest request;
    request.set_node_id(m_containerIds.first);
    request.set_container_id(m_containerIds.second.toStdString());

    auto rsrcCfg = request.mutable_resource_limit();
    auto cpuPage = qobject_cast<CPUConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_CPU));
    cpuPage->getCPUInfo(rsrcCfg);

    auto memoryPage = qobject_cast<MemoryConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY));
    memoryPage->getMemoryInfo(rsrcCfg);

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
    fileProtectPage->getSecurityListInfo(securityCfg);

    //    auto processProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_PROCESS_SECURITY));
    //    processProtectPage->getSecurityListInfo(securityCfg);

    auto netProcessProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_PROCESS_WHITE_LIST));
    netProcessProtectPage->getSecurityListInfo(securityCfg);

    auto networkAccessCtlPage = qobject_cast<NetworkAccessCtlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_ACCESS_CONTROL));
    networkAccessCtlPage->getNetworkAccessInfo(securityCfg);

    auto startStopCtlPage = qobject_cast<StartStopControlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_START_STOP_CONTROL));
    securityCfg->set_disable_cmd_operation(startStopCtlPage->getStartStopInfo());

    InfoWorker::getInstance().updateContainer(request);
}

void ContainerSetting::createTemplate()
{
    container::CreateTemplateRequest request;
    ErrorCode ret;

    auto data = request.mutable_data();
    auto cntrCfg = data->mutable_conf();
    cntrCfg->set_name(ui->lineEdit_name->text().toStdString());
    cntrCfg->set_desc(ui->lineEdit_describe->text().toStdString());

    //Graph
    auto graphicPage = qobject_cast<GraphicConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_GRAPHIC));
    cntrCfg->set_enable_graphic(graphicPage->isGraphic());

    //volume mounts
    auto volumePage = qobject_cast<VolumesConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_VOLUMES));
    ret = volumePage->getVolumeInfo(cntrCfg);
    if (ret == INPUT_NULL_ERROR)
    {
        MessageDialog::message(tr("Volumes Data"),
                               tr("Input error"),
                               tr("Please improve the contents in Volumes table!"),
                               tr(":/images/error.svg"),
                               MessageDialog::StandardButton::Ok);
        return;
    }

    //network
    foreach (auto networkPage, m_netWorkPages)
    {
        networkPage->getNetworkInfo(cntrCfg);
    }

    //env
    auto envPage = qobject_cast<EnvsConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_ENVS));
    ret = envPage->getEnvInfo(cntrCfg);
    if (ret == INPUT_NULL_ERROR)
    {
        MessageDialog::message(tr("Env Data"),
                               tr("Input error"),
                               tr("Please improve the contents in Env table!"),
                               tr(":/images/error.svg"),
                               MessageDialog::StandardButton::Ok);
        return;
    }

    //High
    auto policy = cntrCfg->mutable_restart_policy();
    auto highAvailabilityPage = qobject_cast<HighAvailabilityTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_HIGH_AVAILABILITY));
    highAvailabilityPage->getRestartPolicy(policy);

    auto limit = cntrCfg->mutable_resouce_limit();

    //cpu
    auto cpuPage = qobject_cast<CPUConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_CPU));
    cpuPage->getCPUInfo(limit);

    //memory
    auto memoryPage = qobject_cast<MemoryConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY));
    ret = memoryPage->getMemoryInfo(limit);
    if (ret == INPUT_ARG_ERROR)
    {
        MessageDialog::message(tr("Memory Data"),
                               tr("Input error"),
                               tr("Memory soft limit can't be greater than the maximum limit !"),
                               tr(":/images/error.svg"),
                               MessageDialog::StandardButton::Ok);
        return;
    }

    //security
    auto securityCfg = cntrCfg->mutable_security_config();

    auto fileProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_FILE_PROTECT));
    fileProtectPage->getSecurityListInfo(securityCfg);

    //    auto processProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_PROCESS_SECURITY));
    //    processProtectPage->getSecurityListInfo(securityCfg);

    auto netProcessProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_PROCESS_WHITE_LIST));
    netProcessProtectPage->getSecurityListInfo(securityCfg);

    auto networkAccessCtlPage = qobject_cast<NetworkAccessCtlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_ACCESS_CONTROL));
    networkAccessCtlPage->getNetworkAccessInfo(securityCfg);

    auto startStopCtlPage = qobject_cast<StartStopControlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_START_STOP_CONTROL));
    securityCfg->set_disable_cmd_operation(startStopCtlPage->getStartStopInfo());

    InfoWorker::getInstance().createTemplate(request);
}

void ContainerSetting::updateTemplate()
{
    container::UpdateTemplateRequest request;
    ErrorCode ret;
    auto data = request.mutable_data();
    data->set_id(m_templateId);

    auto cntrCfg = data->mutable_conf();
    cntrCfg->set_name(ui->lineEdit_name->text().toStdString());
    cntrCfg->set_desc(ui->lineEdit_describe->text().toStdString());

    //Graph
    auto graphicPage = qobject_cast<GraphicConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_GRAPHIC));
    cntrCfg->set_enable_graphic(graphicPage->isGraphic());

    //volume mounts
    auto volumePage = qobject_cast<VolumesConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_VOLUMES));
    ret = volumePage->getVolumeInfo(cntrCfg);
    if (ret == INPUT_NULL_ERROR)
    {
        MessageDialog::message(tr("Volumes Data"),
                               tr("Input error"),
                               tr("Please improve the contents in Volumes table!"),
                               tr(":/images/error.svg"),
                               MessageDialog::StandardButton::Ok);
        return;
    }

    //network
    foreach (auto networkPage, m_netWorkPages)
    {
        networkPage->getNetworkInfo(cntrCfg);
    }

    //env
    auto envPage = qobject_cast<EnvsConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_ENVS));
    ret = envPage->getEnvInfo(cntrCfg);
    if (ret == INPUT_NULL_ERROR)
    {
        MessageDialog::message(tr("Env Data"),
                               tr("Input error"),
                               tr("Please improve the contents in Env table!"),
                               tr(":/images/error.svg"),
                               MessageDialog::StandardButton::Ok);
        return;
    }

    //High
    auto policy = cntrCfg->mutable_restart_policy();
    auto highAvailabilityPage = qobject_cast<HighAvailabilityTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_HIGH_AVAILABILITY));
    highAvailabilityPage->getRestartPolicy(policy);

    auto limit = cntrCfg->mutable_resouce_limit();

    //cpu
    auto cpuPage = qobject_cast<CPUConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_CPU));
    cpuPage->getCPUInfo(limit);

    //memory
    auto memoryPage = qobject_cast<MemoryConfTab *>(m_baseConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY));
    ret = memoryPage->getMemoryInfo(limit);
    if (ret == INPUT_ARG_ERROR)
    {
        MessageDialog::message(tr("Memory Data"),
                               tr("Input error"),
                               tr("Memory soft limit can't be greater than the maximum limit !"),
                               tr(":/images/error.svg"),
                               MessageDialog::StandardButton::Ok);
        return;
    }

    //security
    auto securityCfg = cntrCfg->mutable_security_config();

    auto fileProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_FILE_PROTECT));
    fileProtectPage->getSecurityListInfo(securityCfg);

    //    auto processProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_PROCESS_SECURITY));
    //    processProtectPage->getSecurityListInfo(securityCfg);

    auto netProcessProtectPage = qobject_cast<SecurityListTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_PROCESS_WHITE_LIST));
    netProcessProtectPage->getSecurityListInfo(securityCfg);

    auto networkAccessCtlPage = qobject_cast<NetworkAccessCtlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_ACCESS_CONTROL));
    networkAccessCtlPage->getNetworkAccessInfo(securityCfg);

    auto startStopCtlPage = qobject_cast<StartStopControlTab *>(m_securityConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_START_STOP_CONTROL));
    securityCfg->set_disable_cmd_operation(startStopCtlPage->getStartStopInfo());

    InfoWorker::getInstance().updateTemplate(request);
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
        networkConfPage->setVirtNetworkList(m_networkReply);
        break;
    }
    default:
        break;
    }
}

void ContainerSetting::onDelItem()
{
    KLOG_INFO() << "onDelItem";
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
                QListWidgetItem *delItem = ui->listwidget_base_config->takeItem(ui->listwidget_base_config->row(item));
                m_baseItems.removeAt(row);
                auto page = m_baseConfStack->widget(row);
                m_baseConfStack->removeWidget(page);
                m_netWorkPages.removeOne(qobject_cast<NetworkConfTab *>(page));
                --m_netWorkCount;
                updateRemovableItem(NETWORK_CARD);

                delete page;
                page = nullptr;
                delete delItem;
                delItem = nullptr;

                break;
            }
            row++;
        }
    }
    else
        KLOG_INFO() << "cancel";
}

void ContainerSetting::onConfirm()
{
    switch (m_type)
    {
    case CONTAINER_SETTING_TYPE_CONTAINER_CREATE:
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
    default:
        break;
    }
}

void ContainerSetting::onNodeSelectedChanged(QString newStr)
{
    KLOG_INFO() << "onNodeSelectedChanged :" << newStr;
    InfoWorker::getInstance().listNetwork(m_nodeInfo.key(newStr));
    getImageInfo(m_nodeInfo.key(newStr));
}

void ContainerSetting::getNodeListResult(const QPair<grpc::Status, node::ListReply> &reply)
{
    KLOG_INFO() << "getNodeListResult";
    if (reply.first.ok())
    {
        m_nodeInfo.clear();
        ui->cb_node->clear();
        for (auto n : reply.second.nodes())
        {
            m_nodeInfo.insert(n.id(), QString("%1").arg(n.address().data()));
            ui->cb_node->addItem(QString("%1").arg(n.address().data()));
            m_totalCPU = n.status().cpu_stat().total();
        }
        if (m_type == CONTAINER_SETTING_TYPE_CONTAINER_EDIT)
            ui->cb_node->setCurrentText(m_nodeInfo.value(m_containerIds.first));
    }
}

void ContainerSetting::getCreateContainerResult(const QPair<grpc::Status, container::CreateReply> &reply)
{
    KLOG_INFO() << "getCreateContainerResult";
    if (reply.first.ok())
    {
        KLOG_INFO() << "create container successful!";
        emit sigUpdateContainer();
        close();
    }
    else
    {
        KLOG_DEBUG() << QString::fromStdString(reply.first.error_message());
        MessageDialog::message(tr("Create Container"),
                               tr("Create container failed!"),
                               tr("Error: ") + reply.first.error_message().data(),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ContainerSetting::getContainerInspectResult(const QPair<grpc::Status, container::InspectReply> &reply)
{
    KLOG_INFO() << "getContainerInspectResult";
    if (reply.first.ok())
    {
        //init ui
        auto info = reply.second.configs();
        ui->lineEdit_name->setText(info.name().data());
        KLOG_INFO() << info.name().data() << info.image().data();

        if (m_labImage)
            m_labImage->setText(info.image().data());

        if (!QString::fromStdString(info.desc().data()).isEmpty())
            ui->lineEdit_describe->setText(info.desc().data());
        else
            ui->lineEdit_describe->setText(tr("none"));

        //Graph
        //info.enable_graphic();
        auto graphPage = qobject_cast<GraphicConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_GRAPHIC));
        graphPage->setGraphicInfo(&info);
        graphPage->setDisabled(true);

        //volume
        auto volumesPage = qobject_cast<VolumesConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_VOLUMES));
        volumesPage->setVolumeInfo(&info);

        volumesPage->setDisabled(true);

        // network
        auto size = info.networks_size();
        KLOG_INFO() << "network_config_size:" << size;
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
        InfoWorker::getInstance().listNetwork(m_containerIds.first);
        //更新网络页面信息
        for (int i = 0; i < size; i++)
        {
            auto networkConfig = info.networks(i);
            NetworkConfTab *networkPage = m_netWorkPages.at(i);
            networkPage->setNetworkInfo(&networkConfig);
        }

        //env
        auto envPage = qobject_cast<EnvsConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_ENVS));
        envPage->setEnvInfo(&info);
        envPage->setDisabled(true);

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
}

void ContainerSetting::getUpdateContainerResult(const QPair<grpc::Status, container::UpdateReply> &reply)
{
    if (reply.first.ok())
    {
        KLOG_INFO() << "update container successful!";
        sigUpdateContainer();
        close();
    }
    else
    {
        KLOG_DEBUG() << QString::fromStdString(reply.first.error_message());
        MessageDialog::message(tr("Update Container"),
                               tr("Update container failed!"),
                               tr("Error: %1").arg(reply.first.error_message().data()),
                               tr(":/images/warning.svg"),
                               MessageDialog::StandardButton::Ok);
    }
}

void ContainerSetting::getInspectTemplateFinishResult(const QPair<grpc::Status, container::InspectTemplateReply> &reply)
{
    KLOG_INFO() << "getInspectTemplateFinishResult";
    if (reply.first.ok())
    {
        //init ui
        auto info = reply.second.data().conf();
        ui->lineEdit_name->setText(info.name().data());
        KLOG_INFO() << info.name().data();

        if (!QString::fromStdString(info.desc().data()).isEmpty())
            ui->lineEdit_describe->setText(info.desc().data());
        else
            ui->lineEdit_describe->setText(tr("none"));

        //Graph
        //info.enable_graphic();
        auto graphPage = qobject_cast<GraphicConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_GRAPHIC));
        graphPage->setGraphicInfo(&info);

        //volume
        auto volumesPage = qobject_cast<VolumesConfTab *>(m_advancedConfStack->widget(TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_VOLUMES));
        volumesPage->setVolumeInfo(&info);

        // network
        auto size = info.networks_size();
        KLOG_INFO() << "network_config_size:" << size;
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
        InfoWorker::getInstance().listNetwork(m_containerIds.first);
        //更新网络页面信息
        for (int i = 0; i < size; i++)
        {
            auto networkConfig = info.networks(i);
            NetworkConfTab *networkPage = m_netWorkPages.at(i);
            networkPage->setNetworkInfo(&networkConfig);
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
}

void ContainerSetting::getCreateTemplateFinishResult(const QPair<grpc::Status, container::CreateTemplateReply> &reply)
{
    KLOG_INFO() << "getCreateTemplateFinishResult";
    if (reply.first.ok())
    {
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

void ContainerSetting::getUpdateTemplateFinishedResult(const QPair<grpc::Status, container::UpdateTemplateReply> &reply)
{
    KLOG_INFO() << "getUpdateTemplateFinishedResult";
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

void ContainerSetting::getListImageFinishedResult(const QPair<grpc::Status, image::ListReply> &reply)
{
    KLOG_INFO() << "getListImageFinishedResult";
    if (reply.first.ok())
    {
        if (m_cbImage)
        {
            m_cbImage->clear();
            for (auto info : reply.second.images())
                m_cbImage->addItem(QString::fromStdString(info.name()));
        }
    }
    else
    {
        MessageDialog::message(tr("List Image"),
                               tr("Get image List failed!"),
                               tr("Error: %1").arg(reply.first.error_message().data()),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ContainerSetting::getNetworkListResult(const QPair<grpc::Status, network::ListReply> &reply)
{
    KLOG_INFO() << "getNetworkListResult";
    m_networkReply = reply;
    if (reply.first.ok())
    {
        foreach (auto networkPage, m_netWorkPages)
        {
            networkPage->setVirtNetworkList(reply);
            networkPage->setVirtNetwork();
        }
    }
    else
        KLOG_INFO() << "getNetworkListResult failed";
}
