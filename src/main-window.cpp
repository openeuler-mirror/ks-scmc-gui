#include "main-window.h"
#include <kiran-log/qt5-log-i.h>
#include <QAction>
#include <QDebug>
#include <QIcon>
#include <QMutexLocker>
#include <QPainter>
#include <QTimer>
#include <iostream>
#include "./ui_main-window.h"
#include "common/bubble-tip-button.h"
#include "common/common-page.h"
#include "common/guide-item.h"
#include "common/transmission-list.h"
#include "pages/container/container-list.h"
#include "pages/image/image-manager.h"
#include "pages/node/node-list.h"

#define TIMEOUT 200
MainWindow::MainWindow(QWidget* parent)
    : KiranTitlebarWindow(parent), ui(new Ui::MainWindow), m_transmissionList(nullptr), m_timer(nullptr)
{
    ui->setupUi(getWindowContentWidget());
    connect(&InfoWorker::getInstance(), &InfoWorker::transferImageStatus, this, &MainWindow::getTransferImageStatus, Qt::BlockingQueuedConnection);
    initUI();
    m_timer = new QTimer(this);
    //connect(m_timer, &QTimer::timeout, this, &MainWindow::)
}

MainWindow::~MainWindow()
{
    KLOG_INFO() << "**********Deconstruction MainWindow";
    delete ui;

    if (m_transmissionList)
    {
        delete m_transmissionList;
        m_transmissionList = nullptr;
    }
}

void MainWindow::setUserName(QString name)
{
    ui->btn_user->setText(name);
}

void MainWindow::onItemClicked(QListWidgetItem* currItem)
{
    //侧边栏展开与收缩
    GuideItem* guideItem = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(currItem));
    if (guideItem->getItemType() != GUIDE_ITEM_TYPE_GROUP)
    {
        guideItem->setSelected(true);
        foreach (GuideItem* item, m_pageItems)
        {
            if (item != guideItem)
            {
                item->setSelected(false);
            }
        }
    }
    if (m_groupMap.contains(currItem))
    {
        if (m_isShowMap.value(currItem))  //hide
        {
            guideItem->setArrow(true);
            QList<QListWidgetItem*> subItems = m_groupMap.value(currItem);
            foreach (QListWidgetItem* subItem, subItems)
            {
                subItem->setHidden(true);
            }
            m_isShowMap.insert(currItem, false);
        }
        else  //show
        {
            guideItem->setArrow(false);
            QList<QListWidgetItem*> subItems = m_groupMap.value(currItem);
            foreach (QListWidgetItem* subItem, subItems)
            {
                subItem->setHidden(false);
            }
            m_isShowMap.insert(currItem, true);
        }
    }
    int currenRow = ui->listWidget->row(currItem);

    if (!m_pageMap.value(currenRow))
        return;
    setPageName(guideItem->getItemText());
    m_pageMap[currenRow]->updateInfo();
    m_stackedWidget->setCurrentWidget(m_pageMap.value(currenRow));
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_btnTransmission)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QPoint point = m_btnTransmission->mapToGlobal(QPoint(0, 0));
            KLOG_INFO() << point;

            m_transmissionList->move(QPoint(point.x() - 350, point.y() + 35));
            m_transmissionList->show();
        }
    }
    else
        m_transmissionList->hide();

    return false;
}

void MainWindow::initUI()
{
    setIcon(QIcon(":/images/logo.png"));

    //创建消息提示按钮
    m_btnTransmission = new BubbleTipButton(":/images/transmit.svg", this);
    m_btnTransmission->setObjectName("btn_transmission");
    m_btnTransmission->setFixedSize(40, 32);
    m_btnTransmission->setCursor(Qt::PointingHandCursor);
    m_btnTransmission->installEventFilter(this);
    ui->hlayout_btn->addWidget(m_btnTransmission);
    //connect(m_btnTransmission, &BubbleTipButton::clicked, this, &MainWindow::popupTransmissionList);

    m_btnApproval = new BubbleTipButton(":/images/approve.svg", this);
    m_btnApproval->setObjectName("btn_approval");
    m_btnApproval->setFixedSize(40, 32);
    m_btnApproval->setCursor(Qt::PointingHandCursor);
    ui->hlayout_btn->addWidget(m_btnApproval);

    m_btnWarning = new BubbleTipButton(":/images/warning-info.svg", this);
    m_btnWarning->setObjectName("btn_warning");
    m_btnWarning->setFixedSize(40, 32);
    m_btnWarning->setCursor(Qt::PointingHandCursor);
    ui->hlayout_btn->addWidget(m_btnWarning);

    //m_btnApproval->setTipMsg(99);
    //创建传输列表控件
    m_transmissionList = new TransmissionList();
    m_transmissionList->setObjectName("transmissionList");
    connect(m_transmissionList, &TransmissionList::transferItemDeleted, this, &MainWindow::onTransferItemDeleted);

    //创建用户菜单
    QMenu* userMenu = new QMenu(this);
    userMenu->setObjectName("userMenu");
    QAction* changePasswdAct = userMenu->addAction(tr("Change Password"));
    QAction* logoutAct = userMenu->addAction(tr("Logout"));
    QAction* aboutAct = userMenu->addAction(tr("About"));
    ui->btn_user->setMenu(userMenu);
    connect(logoutAct, &QAction::triggered, this, &MainWindow::onLogoutAction);

    //创建右侧内容页面
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setObjectName("stackedWidget");
    ui->vlayout_page->addWidget(m_stackedWidget);

    const QMap<GUIDE_ITEM, QString> pageMap = {
        {GUIDE_ITEM_CONTAINER_LIST, tr("Container List")},
        {GUIDE_ITEM_NODE_MANAGER, tr("Node List")},
        {GUIDE_ITEM_IMAGE_MANAGER, tr("Image Manager")}};
    for (auto iter = pageMap.begin(); iter != pageMap.end(); iter++)
    {
        GUIDE_ITEM itemEnum = iter.key();
        QString desc = iter.value();
        qInfo() << desc << itemEnum;

        auto subPage = createSubPage(itemEnum);
        if (!subPage)
        {
            qWarning() << "sub page is null,ignore!";
            continue;
        }
        m_stackedWidget->addWidget(subPage);
        m_pageMap[itemEnum] = subPage;
    }

    //创建左侧侧边栏
    QListWidgetItem* homeItem = createGuideItem(tr("Home Page"), GUIDE_ITEM_TYPE_NORMAL, ":/images/home.svg");
    QListWidgetItem* auditCenter = createGuideItem(tr("Audit Center"), GUIDE_ITEM_TYPE_GROUP, ":/images/audit-center.svg");
    QListWidgetItem* auditApplyList = createGuideItem(tr("Audit Apply List"), GUIDE_ITEM_TYPE_SUB);
    QListWidgetItem* auditWarningList = createGuideItem(tr("Audit Warning List"), GUIDE_ITEM_TYPE_SUB);
    QListWidgetItem* auditLogList = createGuideItem(tr("Audit Log List"), GUIDE_ITEM_TYPE_SUB);
    QList<QListWidgetItem*> auditSubItems = {auditApplyList, auditWarningList, auditLogList};
    m_groupMap.insert(auditCenter, auditSubItems);
    ///TODO: m_isShowMap.insert(auditCenter, false);
    m_isShowMap.insert(auditCenter, false);

    QListWidgetItem* containerManager = createGuideItem(tr("Container Manager"), GUIDE_ITEM_TYPE_GROUP, ":/images/container-manager.svg");
    QListWidgetItem* containerList = createGuideItem(tr("Container List"), GUIDE_ITEM_TYPE_SUB);
    QListWidgetItem* containerTemplate = createGuideItem(tr("Container Template"), GUIDE_ITEM_TYPE_SUB);
    QList<QListWidgetItem*> containerSubItems = {containerList, containerTemplate};
    m_groupMap.insert(containerManager, containerSubItems);
    m_isShowMap.insert(containerManager, true);

    QListWidgetItem* imageManager = createGuideItem(tr("Image Manager"), GUIDE_ITEM_TYPE_NORMAL, ":/images/image-manager.svg");
    QListWidgetItem* nodeManager = createGuideItem(tr("Node Manager"), GUIDE_ITEM_TYPE_NORMAL, ":/images/node-manager.svg");

    //show first
    GuideItem* guideItem = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(containerManager));
    guideItem->setArrow(false);

    GuideItem* item = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(containerList));
    item->setSelected(true);
    setPageName(item->getItemText());

    foreach (QListWidgetItem* subItem, containerSubItems)
    {
        subItem->setHidden(false);
    }
    ///TODO:set current widget to home
    m_stackedWidget->setCurrentWidget(m_pageMap.value(GUIDE_ITEM_CONTAINER_LIST));
    ui->listWidget->setCurrentRow(GUIDE_ITEM_CONTAINER_LIST);
    m_pageMap[GUIDE_ITEM_CONTAINER_LIST]->updateInfo();

    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::onItemClicked);
}

CommonPage* MainWindow::createSubPage(GUIDE_ITEM itemEnum)
{
    CommonPage* page = nullptr;

    switch (itemEnum)
    {
    case GUIDE_ITEM_CONTAINER_LIST:
    {
        page = new ContainerList(this);
        break;
    }
    case GUIDE_ITEM_CONTAINER_TEMPLATE:
    {
    }
    case GUIDE_ITEM_NODE_MANAGER:
    {
        page = new NodeList(this);
        break;
    }
    case GUIDE_ITEM_IMAGE_MANAGER:
    {
        page = new ImageManager(this);
        break;
    }
    default:
        break;
    }
    return page;
}

QListWidgetItem* MainWindow::createGuideItem(QString text, int type, QString icon)
{
    QListWidgetItem* newItem = nullptr;
    GuideItem* customItem = nullptr;

    newItem = new QListWidgetItem(ui->listWidget);

    customItem = new GuideItem(text, icon, type, ui->listWidget);
    customItem->setTipLinePosition(TIP_LINE_POSITION_RIGHT);

    ui->listWidget->addItem(newItem);
    ui->listWidget->setItemWidget(newItem, customItem);
    newItem->setTextAlignment(Qt::AlignVCenter);

    newItem->setSizeHint(QSize(220, 40));
    if (type == GUIDE_ITEM_TYPE_SUB)
    {
        newItem->setHidden(true);
        m_pageItems.append(customItem);
    }
    else if (type == GUIDE_ITEM_TYPE_NORMAL)
    {
        m_pageItems.append(customItem);
    }
    else
        customItem->setArrow(true);
    ui->listWidget->setGridSize(QSize(220, 50));

    return newItem;
}

void MainWindow::setPageName(QString name)
{
    ui->lab_page_name->setText(name);
}

void MainWindow::onLogoutAction(bool checked)
{
    Q_UNUSED(checked);
    emit sigLogout();
}

void MainWindow::popupTransmissionList()
{
    BubbleTipButton* btn = qobject_cast<BubbleTipButton*>(sender());
    QPoint point = btn->mapToGlobal(QPoint(0, 0));
    KLOG_INFO() << point;

    m_transmissionList->move(QPoint(point.x() - 350, point.y() + 35));
    m_transmissionList->show();
}

void MainWindow::getTransferImageStatus(ImageTransmissionStatus status, QString name, QString version, int rate)
{
    KLOG_INFO() << "getTransferImageInfo:" << status << name << version << rate;

    QString transferImage = name + "-" + version;

    QMutexLocker locker(&m_mutex);
    if (!m_transferImages.contains(transferImage, Qt::CaseInsensitive) && !InfoWorker::getInstance().isTransferStoped(name, version))
    {
        m_transmissionList->addItem(name, version, status, rate);
        m_transferImages.append(transferImage);
        m_btnTransmission->setTipMsg(m_transferImages.size());
    }
    else
    {
        m_transmissionList->updateItem(name, version, status, rate);
        if (status != IMAGE_TRANSMISSION_STATUS_DOWNLOADING && status != IMAGE_TRANSMISSION_STATUS_UPLOADING)
        {
            auto num = m_btnTransmission->getTipMsgNum();
            m_btnTransmission->setTipMsg(--num);
        }
    }
}

void MainWindow::onTransferItemDeleted(QString name, QString version, ImageTransmissionStatus status)
{
    QString transferImage = name + "-" + version;
    if (!transferImage.isEmpty())
    {
        if (m_transferImages.contains(transferImage, Qt::CaseInsensitive))
        {
            if (status == IMAGE_TRANSMISSION_STATUS_DOWNLOADING || status == IMAGE_TRANSMISSION_STATUS_UPLOADING)
            {
                InfoWorker::getInstance().stopTransfer(name, version, true);
            }
            QMutexLocker locker(&m_mutex);
            m_transferImages.removeOne(transferImage);
            m_btnTransmission->setTipMsg(m_transferImages.size());
            KLOG_INFO() << "stop transfer:" << name << version << ",tipMsg:" << m_transferImages.size();
        }
    }
}
