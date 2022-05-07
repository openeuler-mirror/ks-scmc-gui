#include "main-window.h"
#include <kiran-log/qt5-log-i.h>
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QIcon>
#include <QMutexLocker>
#include <QPainter>
#include <QTimer>
#include <iostream>
#include "./ui_main-window.h"
#include "common/bubble-tip-button.h"
#include "common/guide-item.h"
#include "page.h"
#include "pages/audit/audit-list/audit-list-page.h"
#include "pages/audit/log-list/log-list-page.h"
#include "pages/container/container-manager/container-page-manager.h"
#include "pages/container/template-manager/template-list-page.h"
#include "pages/image/image-list-page.h"
#include "pages/image/transmission-list.h"
#include "pages/node/node-list-page.h"
#include "pages/node/node-page-manager.h"
#include "pages/user/passwd-update-dialog.h"
#include "table-page.h"

#define GENERAL_OUTLINE QObject::tr("General Outline")
#define CONTAINER_MANAGER QObject::tr("Container Manager")
#define CONTAINER_LIST QObject::tr("Container List")
#define CONTAINER_TEMPLATE QObject::tr("Container Template")
#define AUDIT_CENTER QObject::tr("Audit Center")
#define AUDIT_APPLY_LIST QObject::tr("Audit Apply List")
#define AUDIT_WORNING_LIST QObject::tr("Audit Warning List")
#define AUDIT_LOG_LIST QObject::tr("Audit Log List")
#define IMAGE_MANAGER QObject::tr("Image Manager")
#define NODE_MANAGER QObject::tr("Node Manager")
#define SYSTEM_MANAGER QObject::tr("System Manager")
//#define OUTLINE_PAGES QObject::tr("Outline")

#define TIMEOUT 200
MainWindow::MainWindow(QWidget* parent)
    : KiranTitlebarWindow(parent),
      ui(new Ui::MainWindow),
      m_transmissionList(nullptr),
      m_timer(nullptr),
      m_pwUpdateDlg(nullptr)
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
    if (m_pwUpdateDlg)
    {
        delete m_pwUpdateDlg;
        m_pwUpdateDlg = nullptr;
    }
}

void MainWindow::setUserName(QString name)
{
    ui->btn_user->setText(name);
    m_userName = name;
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
    QString currenItemData = currItem->data(Qt::UserRole).toString();

    setPageName(guideItem->getItemText());
    if (guideItem->getItemText() == GENERAL_OUTLINE)
    {
        m_outline->updateInfo();
        m_stackedWidget->setCurrentWidget(m_outline);
    }
    else
    {
        if (!m_pageMap.value(currenItemData))
            return;
        m_pageMap[currenItemData]->updateInfo();
        m_stackedWidget->setCurrentWidget(m_pageMap.value(currenItemData));
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
    connect(changePasswdAct, &QAction::triggered, this, &MainWindow::onChangePwAction);
    connect(logoutAct, &QAction::triggered, this, &MainWindow::onLogoutAction);

    //创建右侧内容页面
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setObjectName("stackedWidget");
    ui->vlayout_page->addWidget(m_stackedWidget);

    //pageMap.value
    const QMap<GUIDE_ITEM, QString> pageMap = {
        {GUIDE_ITEM_AUDIT_APPLY_LIST, AUDIT_APPLY_LIST},
        {GUIDE_ITEM_AUDIT_LOG_LIST, AUDIT_LOG_LIST},
        {GUIDE_ITEM_CONTAINER_List_PAGE_MANAGER, CONTAINER_LIST},
        {GUIDE_ITEM_CONTAINER_TEMPLATE_LIST, CONTAINER_TEMPLATE},
        {GUIDE_ITEM_NODE_MANAGER, NODE_MANAGER},
        {GUIDE_ITEM_IMAGE_LIST, IMAGE_MANAGER}};
    for (auto iter = pageMap.begin(); iter != pageMap.end(); iter++)
    {
        GUIDE_ITEM itemEnum = iter.key();
        QString desc = iter.value();
        KLOG_INFO() << desc << itemEnum;

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

    //创建左侧侧边栏
    QListWidgetItem* homeItem = createGuideItem(GENERAL_OUTLINE, GUIDE_ITEM_TYPE_NORMAL, ":/images/home.svg");

    QListWidgetItem* nodeManager = createGuideItem(NODE_MANAGER, GUIDE_ITEM_TYPE_NORMAL, ":/images/node-manager.svg");
    QListWidgetItem* imageManager = createGuideItem(IMAGE_MANAGER, GUIDE_ITEM_TYPE_NORMAL, ":/images/image-manager.svg");

    QListWidgetItem* containerManager = createGuideItem(CONTAINER_MANAGER, GUIDE_ITEM_TYPE_GROUP, ":/images/container-manager.svg");
    QListWidgetItem* containerList = createGuideItem(CONTAINER_LIST, GUIDE_ITEM_TYPE_SUB);
    QListWidgetItem* containerTemplate = createGuideItem(CONTAINER_TEMPLATE, GUIDE_ITEM_TYPE_SUB);
    QList<QListWidgetItem*> containerSubItems = {containerList, containerTemplate};
    m_groupMap.insert(containerManager, containerSubItems);
    m_isShowMap.insert(containerManager, false);

    QListWidgetItem* auditCenter = createGuideItem(AUDIT_CENTER, GUIDE_ITEM_TYPE_GROUP, ":/images/audit-center.svg");
    QListWidgetItem* auditApplyList = createGuideItem(AUDIT_APPLY_LIST, GUIDE_ITEM_TYPE_SUB);
    QListWidgetItem* auditWarningList = createGuideItem(AUDIT_WORNING_LIST, GUIDE_ITEM_TYPE_SUB);
    QListWidgetItem* auditLogList = createGuideItem(AUDIT_LOG_LIST, GUIDE_ITEM_TYPE_SUB);
    QList<QListWidgetItem*> auditSubItems = {auditApplyList, auditWarningList, auditLogList};
    m_groupMap.insert(auditCenter, auditSubItems);
    ///TODO: m_isShowMap.insert(auditCenter, false);
    m_isShowMap.insert(auditCenter, false);

    GuideItem* item = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(homeItem));
    item->setSelected(true);
    setPageName(item->getItemText());

    m_outline = new OutlineView;
    m_stackedWidget->addWidget(m_outline);
    connect(m_outline, &OutlineView::outlineCellStepPages, this, &MainWindow::outlineJumpPage);

    ///TODO:set current widget to home
    m_stackedWidget->setCurrentWidget(m_outline);
    ui->listWidget->setCurrentRow(0);
    m_outline->setStyleSheet("background-color: #222222;"
                             "border:none;");
    m_stackedWidget->setContentsMargins(0, 0, 0, 0);
    m_outline->updateInfo();
    //    m_pageMap[GENERAL_OUTLINE]->updateInfo();

    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::onItemClicked);
}

void MainWindow::outlinePageChange(QString str)
{
    setPageName(str);
    QListWidgetItem* currItem = new QListWidgetItem;

    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        currItem = ui->listWidget->item(i);
        if (currItem->data(Qt::UserRole).toString() == str)
        {
            GuideItem* guideItem = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(currItem));
            m_stackedWidget->setCurrentWidget(m_pageMap.value(str));
            guideItem->setSelected(true);
            ui->listWidget->setItemSelected(currItem, true);
            m_pageMap[str]->updateInfo();
            break;
        }
    }
}

void MainWindow::outlineJumpPage(OutlineCellType type)
{
    auto item = ui->listWidget->item(0);
    GuideItem* outlineItem = qobject_cast<GuideItem*>(ui->listWidget->itemWidget(item));
    //    guideItem->setSelected(true);
    switch (type)
    {
    case ONUTLINE_CELL_NODE:
    {
        outlinePageChange(NODE_MANAGER);
        outlineItem->setSelected(false);
        break;
    }
    case ONUTLINE_CELL_CONTAINER:
    {
        outlinePageChange(CONTAINER_LIST);
        outlineItem->setSelected(false);
        break;
    }
    case ONUTLINE_CELL_IMAGE:
    {
        outlinePageChange(IMAGE_MANAGER);
        outlineItem->setSelected(false);
        break;
    }
    case ONUTLINE_CELL_TEMPLATE_CONTAINER:
    {
        break;
    }
    case ONUTLINE_CELL_EXAMINE:
    {
        outlinePageChange(AUDIT_APPLY_LIST);
        outlineItem->setSelected(false);
        break;
    }
    case ONUTLINE_CELL_NODE_WARNING:
    {
        break;
    }
    default:
        break;
    }
}

Page* MainWindow::createSubPage(GUIDE_ITEM itemEnum)
{
    Page* page = nullptr;

    switch (itemEnum)
    {
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
        page = new ImageListPage(this);
        break;
    }
    case GUIDE_ITEM_AUDIT_APPLY_LIST:
    {
        page = new AuditListPage(this);
        break;
    }
    case GUIDE_ITEM_AUDIT_LOG_LIST:
    {
        page = new LogListPage(this);
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
