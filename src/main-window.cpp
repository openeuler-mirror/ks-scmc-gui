/**
 * @file          /ks-scmc-gui/src/main-window.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "main-window.h"
#include <kiran-log/qt5-log-i.h>
#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <QMutexLocker>
#include <QPainter>
#include <QToolTip>
#include "./ui_main-window.h"
#include "common/about-page.h"
#include "common/bubble-tip-button.h"
#include "common/guide-item.h"
#include "message-dialog.h"
#include "page.h"
#include "pages/audit/log-list/log-list-page.h"
#include "pages/audit/warning-list/warning-list-page.h"
#include "pages/container/container-manager/container-page-manager.h"
#include "pages/container/template-manager/template-list-page.h"
#include "pages/image/image-approval-page.h"
#include "pages/image/image-list-page.h"
#include "pages/image/security-switch-page.h"
#include "pages/image/transmission-list.h"
#include "pages/node/node-page-manager.h"
#include "pages/user/passwd-update-dialog.h"
#include "table-page.h"

#define GENERAL_OUTLINE QObject::tr("General Outline")

#define CONTAINER_MANAGER QObject::tr("Container Manager")
#define CONTAINER_LIST QObject::tr("Container List")
#define CONTAINER_TEMPLATE QObject::tr("Container Template")

#define WARNING_LOG QObject::tr("Warning Log")
#define WORNING_LIST QObject::tr("Warning List")
#define LOG_LIST QObject::tr("Log List")

#define IMAGE_STOREHOUSE QObject::tr("Image Storehouse")
#define IMAGE_APPROVAL QObject::tr("Image Approval")
#define IMAGE_APPROVAL_LIST QObject::tr("Image Approval List")
#define IMAGE_APPROVABLE_CONTROLLER QObject::tr("Approvable Controller")

#define NODE_MANAGER QObject::tr("Node Manager")

//#define SYSTEM_MANAGER QObject::tr("System Manager")
//#define OUTLINE_PAGES QObject::tr("Outline")

#define TIMEOUT 200
MainWindow::MainWindow(QString name, QWidget* parent)
    : KiranTitlebarWindow(parent),
      ui(new Ui::MainWindow),
      m_userName(name),
      m_transmissionList(nullptr),
      m_pwUpdateDlg(nullptr)
{
    ui->setupUi(getWindowContentWidget());
    initUI();
    ui->btn_user->setText(name);
    connect(&InfoWorker::getInstance(), &InfoWorker::transferImageStatus, this, &MainWindow::getTransferImageStatus, Qt::BlockingQueuedConnection);
    connect(m_stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(changePage(int)));
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
    if (m_pwUpdateDlg)
    {
        delete m_pwUpdateDlg;
        m_pwUpdateDlg = nullptr;
    }
}

void MainWindow::onItemClicked(QListWidgetItem* currItem)
{
    //侧边栏展开与收缩
    GuideItem* guideItem = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(currItem));
    //点击的不是组项
    if (!m_groupMap.contains(currItem))
    {
        guideItem->setSelected(true);
        foreach (GuideItem* item, m_pageItems)
        {
            if (item != guideItem)
            {
                item->setSelected(false);
            }
        }
        //跳转、更新右侧页面
        QString currenItemData = currItem->data(Qt::UserRole).toString();
        setPageName(guideItem->getItemText());

        if (!m_pageMap.value(currenItemData))
            return;
        m_pageMap[currenItemData]->updateInfo();
        m_stackedWidget->setCurrentWidget(m_pageMap.value(currenItemData));
    }
    //点击的是组项
    else
    {
        //同步右侧页面对应的侧边项
        auto currPage = qobject_cast<Page*>(m_stackedWidget->currentWidget());
        auto currPageData = m_pageMap.key(currPage);
        for (int i = 0; i < ui->listWidget->count(); i++)
        {
            auto item = ui->listWidget->item(i);
            if (item->data(Qt::UserRole).toString() == currPageData)
                item->setSelected(true);
        }

        //侧边项收缩与展开
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

            m_transmissionList->move(QPoint(point.x() - 350, point.y() + 35));
            m_transmissionList->show();
            return true;
        }
    }
    //    else if (m_filterInput && obj == this && event->type() == QEvent::MouseButtonPress)
    //    {
    //        KLOG_INFO() << "filter input";
    //        return true;
    //    }
    else
    {
        if (m_transmissionList)
            m_transmissionList->hide();
    }

    return false;
}

//void MainWindow::closeEvent(QCloseEvent* event)
//{
//    InfoWorker::getInstance().appQuit();
//    if (QThreadPool::globalInstance()->activeThreadCount())
//    {
//        MessageDialog::message(tr("Quit Application"),
//                               tr("There are threads that have not finished!"),
//                               tr("Please wait for the thread to end before closing!"),
//                               ":/images/warning.svg",
//                               MessageDialog::StandardButton::Ok);
//        event->ignore();
//        return;
//        //        if (ret == MessageDialog::StandardButton::Cancel)
//        //        {
//        //            event->ignore();
//        //        }
//        //        else
//        //        {
//        //            m_timerId = this->startTimer(500);

//        //            KLOG_INFO() << "start timer" << m_timerId;
//        //            QEventLoop loop;
//        //            //QThread::msleep(10000);
//        //            while (true)
//        //            {
//        //                loop.processEvents(QEventLoop::ExcludeUserInputEvents, 500);
//        //                //                if (!QThreadPool::globalInstance()->activeThreadCount())
//        //                //                {
//        //                //                    break;
//        //                //                }
//        //                //if (!m_filterInput)
//        //                if (m_isClose)
//        //                    break;
//        //            }
//        //            event->accept();
//        //        }
//        //        return;
//    }
//    event->accept();
//}

//void MainWindow::timerEvent(QTimerEvent* event)
//{
//    KLOG_INFO() << "timerEvent " << event->timerId();
//    if (event->timerId() == m_timerId)
//    {
//        KLOG_INFO() << m_timerId;
//        if (0 == QThreadPool::globalInstance()->activeThreadCount())
//        {
//            m_filterInput = false;
//            killTimer(m_timerId);
//            m_isClose = true;
//        }
//        else
//            m_filterInput = true;
//        KLOG_INFO() << "m_filterInput=" << m_filterInput << "m_isClose = " << m_isClose;
//    }
//}

void MainWindow::initUI()
{
    setIcon(QIcon(":/images/logo.png"));
    setTitle(tr("KylinSec Container System V1(Security Version)"));
    //创建消息提示按钮
    m_btnTransmission = new BubbleTipButton(":/images/transmit.svg", this);
    m_btnTransmission->setObjectName("btn_transmission");
    m_btnTransmission->setFixedSize(40, 32);
    m_btnTransmission->setCursor(Qt::PointingHandCursor);
    m_btnTransmission->setToolTip(tr("Transmission"));
    m_btnTransmission->installEventFilter(this);
    ui->hlayout_btn->addWidget(m_btnTransmission);
    //connect(m_btnTransmission, &BubbleTipButton::clicked, this, &MainWindow::popupTransmissionList);

    m_btnApproval = new BubbleTipButton(":/images/approve.svg", this);
    m_btnApproval->setObjectName("btn_approval");
    m_btnApproval->setFixedSize(40, 32);
    m_btnApproval->setCursor(Qt::PointingHandCursor);
    m_btnApproval->setToolTip(tr("Approval"));
    ui->hlayout_btn->addWidget(m_btnApproval);
    connect(m_btnApproval, &BubbleTipButton::clicked, this, &MainWindow::onApprovalPage);

    m_btnWarning = new BubbleTipButton(":/images/warning-info.svg", this);
    m_btnWarning->setObjectName("btn_warning");
    m_btnWarning->setFixedSize(40, 32);
    m_btnWarning->setCursor(Qt::PointingHandCursor);
    m_btnWarning->setToolTip(tr("Warning"));
    ui->hlayout_btn->addWidget(m_btnWarning);
    connect(m_btnWarning, &BubbleTipButton::clicked, this, &MainWindow::onWarningPage);

    QPalette palette;
    palette.setColor(QPalette::Inactive, QPalette::ToolTipBase, QColor("#ffffff"));
    palette.setColor(QPalette::Inactive, QPalette::ToolTipText, QColor("#000000"));
    QToolTip::setPalette(palette);

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
    userMenu->addSeparator();
    QAction* aboutAct = userMenu->addAction(tr("About"));
    ui->btn_user->setMenu(userMenu);
    connect(changePasswdAct, &QAction::triggered, this, &MainWindow::onChangePwAction);
    connect(logoutAct, &QAction::triggered, this, &MainWindow::onLogoutAction);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::onAboutAction);

    //创建右侧堆叠页面
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setObjectName("stackedWidget");
    ui->vlayout_page->addWidget(m_stackedWidget);
    loadUserPage();

    //创建左侧侧边栏
    loadUserItem();

    //set current widget to home
    m_stackedWidget->setCurrentWidget(m_pageMap[GENERAL_OUTLINE]);
    ui->listWidget->setCurrentRow(0);

    GuideItem* item = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(ui->listWidget->item(0)));
    item->setSelected(true);
    setPageName(item->getItemText());

    m_pageMap[GENERAL_OUTLINE]->updateInfo();

    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::onItemClicked);
}

void MainWindow::outlinePageChange(QString str)
{
    if (!m_pageMap[str])
        return;
    bool find = false;
    setPageName(str);
    auto outlineWidgetItem = ui->listWidget->item(0);
    KLOG_INFO() << "current item :" << ui->listWidget->currentRow();
    GuideItem* outlineItem = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(outlineWidgetItem));
    outlineItem->setSelected(false);

    QListWidgetItem* item = nullptr;

    //判断要跳转的页面是否是侧边栏子项，若是，则将组项展开，子项设 置选中状态，更新对应界面
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        item = ui->listWidget->item(i);
        if (item->data(Qt::UserRole).toString() == str)
        {
            ui->listWidget->setCurrentItem(item);
            item->setSelected(true);
            GuideItem* guideItem = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(item));
            guideItem->setSelected(true);

            auto i = m_groupMap.constBegin();
            while (i != m_groupMap.constEnd())
            {
                if (i.value().contains(item))
                {
                    auto item = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(i.key()));
                    if (!m_isShowMap.value(i.key()))  //show
                    {
                        item->setArrow(false);
                        foreach (QListWidgetItem* subItem, i.value())
                        {
                            subItem->setHidden(false);
                        }
                        m_isShowMap.insert(i.key(), true);
                    }
                    m_pageMap[str]->updateInfo();
                    m_stackedWidget->setCurrentWidget(m_pageMap[str]);
                    find = true;
                    break;
                }
                ++i;
            }

            if (!find)
            {
                m_pageMap[str]->updateInfo();
                m_stackedWidget->setCurrentWidget(m_pageMap[str]);
            }
            break;
        }
    }
}

void MainWindow::outlineJumpPage(OutlineCellType type)
{
    switch (type)
    {
    case ONUTLINE_CELL_NODE:
    {
        outlinePageChange(NODE_MANAGER);
        break;
    }
    case ONUTLINE_CELL_CONTAINER:
    {
        outlinePageChange(CONTAINER_LIST);
        break;
    }
    case ONUTLINE_CELL_IMAGE:
    {
        outlinePageChange(IMAGE_STOREHOUSE);
        break;
    }
    case ONUTLINE_CELL_TEMPLATE_CONTAINER:
    {
        outlinePageChange(CONTAINER_TEMPLATE);
        break;
    }
    case ONUTLINE_CELL_EXAMINE:
    {
        outlinePageChange(IMAGE_APPROVAL_LIST);
        break;
    }
    case ONUTLINE_CELL_NODE_WARNING:
    {
        outlinePageChange(WORNING_LIST);
        break;
    }
    default:
        break;
    }
}

void MainWindow::changePage(int)
{
    QString info = "exitTimedRefresh";
    Page* page = qobject_cast<Page*>(m_stackedWidget->currentWidget());
    QString data = page->getData().toString();
    if (data != NODE_MANAGER)
    {
        if (m_pageMap[NODE_MANAGER])
            m_pageMap[NODE_MANAGER]->updateInfo(info);
    }
    if (data != CONTAINER_LIST)
    {
        if (m_pageMap[CONTAINER_LIST])
            m_pageMap[CONTAINER_LIST]->updateInfo(info);
    }
}

void MainWindow::onUpdateWarnTipsSums()
{
    OutlineView* outline = qobject_cast<OutlineView*>(m_pageMap[GENERAL_OUTLINE]);
    outline->updateWarningSums();
}

Page* MainWindow::createSubPage(GUIDE_ITEM itemEnum)
{
    Page* page = nullptr;

    switch (itemEnum)
    {
    case GUIDE_ITEM_HONE:
    {
        OutlineView* outline = new OutlineView(this);
        connect(outline, &OutlineView::outlineCellStepPages, this, &MainWindow::outlineJumpPage);
        connect(outline, &OutlineView::sigApproveSumNums, this, &MainWindow::setApprovalTipNums);
        connect(outline, &OutlineView::sigWarnSumNums, this, &MainWindow::setWarningTipNums);
        page = outline;
        break;
    }
    case GUIDE_ITEM_CONTAINER_List_PAGE_MANAGER:
    {
        page = new ContainerPageManager(this);
        break;
    }
    case GUIDE_ITEM_CONTAINER_TEMPLATE_LIST:
    {
        page = new TemplateListPage(this);
        break;
    }
    case GUIDE_ITEM_NODE_MANAGER:
    {
        //page = new NodeListPage(this);
        page = new NodePageManager(this);
        break;
    }
    case GUIDE_ITEM_IMAGE_LIST:
    {
        ImageListPage* imagePage = new ImageListPage(this);
        connect(imagePage, &ImageListPage::sigUpdateAPproveTipSums, this, &MainWindow::setApprovalTipNums);
        page = imagePage;
        break;
    }
    case GUIDE_ITEM_IMAGE_APPROVAL_LIST:
    {
        //TODO:修改ImageApprovalPage为镜像审批页面ImageApprovalPage
        ImageApprovalPage* approvePage = new ImageApprovalPage(this);
        connect(approvePage, &ImageApprovalPage::sigUpdateTipSumsProxy, this, &MainWindow::setApprovalTipNums);
        page = approvePage;
        break;
    }
    case GUIDE_ITEM_IMAGE_APPROVABLE_CONTROLLER:
    {
        SecuritySwitchPage* ctr = new SecuritySwitchPage(this);
        page = ctr;
        break;
    }
    case GUIDE_ITEM_LOG_LIST:
    {
        page = new LogListPage(this);
        break;
    }
    case GUIDE_ITEM_WARNING_LIST:
    {
        WarningListPage* warnPage = new WarningListPage(this);
        connect(warnPage, &WarningListPage::sigReadedUpdateWaringSums, this, &MainWindow::onUpdateWarnTipsSums);
        page = warnPage;
        break;
    }
    default:

        break;
    }
    return page;
}

void MainWindow::loadUserPage()
{
    QMap<GUIDE_ITEM, QString> pageMap;
    if (m_userName == "sysadm")
    {
        pageMap = {
            {GUIDE_ITEM_HONE, GENERAL_OUTLINE},
            {GUIDE_ITEM_CONTAINER_List_PAGE_MANAGER, CONTAINER_LIST},
            {GUIDE_ITEM_CONTAINER_TEMPLATE_LIST, CONTAINER_TEMPLATE},
            {GUIDE_ITEM_NODE_MANAGER, NODE_MANAGER},
            {GUIDE_ITEM_IMAGE_LIST, IMAGE_STOREHOUSE}};
    }
    else if (m_userName == "secadm")
    {
        pageMap = {
            {GUIDE_ITEM_HONE, GENERAL_OUTLINE},
            {GUIDE_ITEM_IMAGE_APPROVAL_LIST, IMAGE_APPROVAL_LIST},
            {GUIDE_ITEM_IMAGE_APPROVABLE_CONTROLLER, IMAGE_APPROVABLE_CONTROLLER}};
    }
    else if (m_userName == "audadm")
    {
        pageMap = {
            {GUIDE_ITEM_HONE, GENERAL_OUTLINE},
            {GUIDE_ITEM_WARNING_LIST, WORNING_LIST},
            {GUIDE_ITEM_LOG_LIST, LOG_LIST}};
    }

    for (auto iter = pageMap.begin(); iter != pageMap.end(); iter++)
    {
        GUIDE_ITEM itemEnum = iter.key();
        auto subPage = createSubPage(itemEnum);
        if (!subPage)
        {
            KLOG_WARNING() << "sub page is null,ignore!";
            continue;
        }
        subPage->setData(QVariant(iter.value()));
        m_stackedWidget->addWidget(subPage);
        m_pageMap[iter.value()] = subPage;
    }
}

void MainWindow::loadUserItem()
{
    QListWidgetItem* homeItem = createGuideItem(GENERAL_OUTLINE, GUIDE_ITEM_TYPE_NORMAL,
                                                ":/images/home.svg");
    if (m_userName == "sysadm")
    {
        QListWidgetItem* nodeManager = createGuideItem(NODE_MANAGER, GUIDE_ITEM_TYPE_NORMAL,
                                                       ":/images/node-manager.svg");
        QListWidgetItem* imageStorehouse = createGuideItem(IMAGE_STOREHOUSE, GUIDE_ITEM_TYPE_NORMAL,
                                                           ":/images/image-manager.svg");

        QListWidgetItem* containerManager = createGuideItem(CONTAINER_MANAGER, GUIDE_ITEM_TYPE_GROUP,
                                                            ":/images/container-manager.svg");
        containerManager->setFlags(containerManager->flags() & ~Qt::ItemIsSelectable);

        QListWidgetItem* containerList = createGuideItem(CONTAINER_LIST, GUIDE_ITEM_TYPE_SUB);
        QListWidgetItem* containerTemplate = createGuideItem(CONTAINER_TEMPLATE, GUIDE_ITEM_TYPE_SUB);

        QList<QListWidgetItem*> containerSubItems = {containerList, containerTemplate};
        m_groupMap.insert(containerManager, containerSubItems);
        m_isShowMap.insert(containerManager, true);
    }
    else if (m_userName == "secadm")
    {
        QListWidgetItem* imageApproval = createGuideItem(IMAGE_APPROVAL, GUIDE_ITEM_TYPE_GROUP,
                                                         ":/images/image-manager.svg");
        imageApproval->setFlags(imageApproval->flags() & ~Qt::ItemIsSelectable);

        QListWidgetItem* imageApprovalList = createGuideItem(IMAGE_APPROVAL_LIST, GUIDE_ITEM_TYPE_SUB);
        QListWidgetItem* imageApprovableCtr = createGuideItem(IMAGE_APPROVABLE_CONTROLLER, GUIDE_ITEM_TYPE_SUB);
        QList<QListWidgetItem*> iamgeApprovalSubItems = {imageApprovalList, imageApprovableCtr};
        m_groupMap.insert(imageApproval, iamgeApprovalSubItems);
        m_isShowMap.insert(imageApproval, true);
    }
    else if (m_userName == "audadm")
    {
        QListWidgetItem* warningLog = createGuideItem(WARNING_LOG, GUIDE_ITEM_TYPE_GROUP,
                                                      ":/images/audit-center.svg");
        warningLog->setFlags(warningLog->flags() & ~Qt::ItemIsSelectable);

        QListWidgetItem* warningList = createGuideItem(WORNING_LIST, GUIDE_ITEM_TYPE_SUB);
        QListWidgetItem* logList = createGuideItem(LOG_LIST, GUIDE_ITEM_TYPE_SUB);
        QList<QListWidgetItem*> auditSubItems = {warningList, logList};
        m_groupMap.insert(warningLog, auditSubItems);
        m_isShowMap.insert(warningLog, true);
    }
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
    //    if (type == GUIDE_ITEM_TYPE_SUB)
    //    {
    //        newItem->setHidden(true);
    //        m_pageItems.append(customItem);
    //    }
    //    else if (type == GUIDE_ITEM_TYPE_NORMAL)
    //    {
    //        m_pageItems.append(customItem);
    //    }
    if (type == GUIDE_ITEM_TYPE_GROUP)
        customItem->setArrow(false);  //默认展开
    else
        m_pageItems.append(customItem);
    ui->listWidget->setGridSize(QSize(220, 56));

    newItem->setData(Qt::UserRole, text);

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

void MainWindow::onChangePwAction(bool checked)
{
    Q_UNUSED(checked);
    if (!m_pwUpdateDlg)
    {
        m_pwUpdateDlg = new PasswdUpdateDialog(m_userName);
        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_pwUpdateDlg->move(screenGeometry.x() + (screenGeometry.width() - m_pwUpdateDlg->width()) / 2,
                            screenGeometry.y() + (screenGeometry.height() - m_pwUpdateDlg->height()) / 2);
        m_pwUpdateDlg->show();

        connect(m_pwUpdateDlg, &PasswdUpdateDialog::destroyed,
                [=] {
                    KLOG_INFO() << " PasswdUpdateDialog destroy";
                    m_pwUpdateDlg->deleteLater();
                    m_pwUpdateDlg = nullptr;
                });
        connect(m_pwUpdateDlg, &PasswdUpdateDialog::sigUpdatePasswdSuccessful, this, &MainWindow::onUpdatePwSuccessful);
    }
}

void MainWindow::onAboutAction(bool checked)
{
    Q_UNUSED(checked);
    AboutPage* about = new AboutPage(this);
    int x = this->x() + this->width() / 2 - about->width() / 2;
    int y = this->y() + this->height() / 2 - about->height() / 2;

    about->move(x, y);
    about->show();
}

void MainWindow::onUpdatePwSuccessful()
{
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

void MainWindow::onApprovalPage(bool check)
{
    Q_UNUSED(check);

    auto currItem = ui->listWidget->currentItem();
    KLOG_INFO() << "current item :" << ui->listWidget->currentRow();
    GuideItem* outlineItem = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(currItem));
    outlineItem->setSelected(false);
    outlineJumpPage(ONUTLINE_CELL_EXAMINE);
}

void MainWindow::onWarningPage(bool check)
{
    Q_UNUSED(check);
    auto currItem = ui->listWidget->currentItem();
    KLOG_INFO() << "current item :" << ui->listWidget->currentRow();
    GuideItem* outlineItem = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(currItem));
    outlineItem->setSelected(false);
    outlineJumpPage(ONUTLINE_CELL_NODE_WARNING);
}

void MainWindow::setApprovalTipNums(int nums)
{
    m_btnApproval->setTipMsg(nums);
}

void MainWindow::setWarningTipNums(int nums)
{
    m_btnWarning->setTipMsg(nums);
}

void MainWindow::getTransferImageStatus(ImageTransmissionStatus status, QString name, QString version, int rate)
{
    KLOG_INFO() << "getTransferImageInfo:" << status << name << version << rate;

    QString transferImage = name + "-" + version;

    QMutexLocker locker(&m_mutex);
    if (!m_transferImages.contains(transferImage, Qt::CaseInsensitive) &&
        !InfoWorker::getInstance().isTransferStoped(name, version))
    {
        m_transmissionList->addItem(name, version, status, rate);
        m_transferImages.append(transferImage);
        m_btnTransmission->setTipMsg(m_transferImages.size());
    }
    else
    {
        m_transmissionList->updateItem(name, version, status, rate);
        //        if (status != IMAGE_TRANSMISSION_STATUS_DOWNLOADING && status != IMAGE_TRANSMISSION_STATUS_UPLOADING)
        //        {
        //            auto num = m_btnTransmission->getTipMsgNum();
        //            m_btnTransmission->setTipMsg(--num);
        //        }
    }
}

void MainWindow::onTransferItemDeleted(QString name, QString version, ImageTransmissionStatus status)
{
    QString transferImage = name + "-" + version;
    if (!transferImage.isEmpty())
    {
        if (m_transferImages.contains(transferImage, Qt::CaseInsensitive))
        {
            QMutexLocker locker(&m_mutex);
            if (status == IMAGE_TRANSMISSION_STATUS_DOWNLOADING || status == IMAGE_TRANSMISSION_STATUS_UPLOADING)
            {
                InfoWorker::getInstance().stopTransfer(name, version, true);
            }
            m_transferImages.removeOne(transferImage);
            m_btnTransmission->setTipMsg(m_transferImages.size());
            KLOG_INFO() << "stop transfer:" << name << version << ",tipMsg:" << m_transferImages.size();
        }
    }
}
