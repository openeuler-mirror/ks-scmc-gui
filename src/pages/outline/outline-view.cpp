/**
 * @file          /ks-scmc-gui/src/pages/outline/outline-view.cpp
 * @brief
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "outline-view.h"
#include <kiran-log/qt5-log-i.h>
#include <QLabel>
#include <QPainter>
#include <QStandardItem>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>
#include <cmath>
#include "outline-cell.h"
#include "ui_outline-cell.h"
#define TIMEOUT 5000

OutlineView::OutlineView(QWidget *parent) : Page(parent)
{
    m_objId = InfoWorker::generateId(this);
    initUI();
    initConnect();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [this] {
        updateInfo();
    });
}

OutlineView::~OutlineView()
{
    if (m_timer)
    {
        m_timer->stop();
    }
}

void OutlineView::initUI()
{
#if 1
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setGeometry(0, 0, 1666, 875);
    m_scrollArea->setWidgetResizable(false);
    m_scrollArea->setContentsMargins(0, 0, 0, 0);

    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);  //Qt::ScrollBarAlwaysOn
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_scrollWidget = new QWidget();  //(m_scrollArea);
    m_scrollWidget->setGeometry(0, 0, 1666, 875);
    m_scrollWidget->setMinimumSize(1666, 875);
    m_scrollWidget->setContentsMargins(0, 0, 0, 0);
#endif
    setWindowFlags(Qt::FramelessWindowHint);  // 设置窗口标志
    setMinimumSize(600, 400);                 // 设置最小尺寸
    setContentsMargins(0, 0, 0, 0);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);  // 设置尺寸属性

    QGridLayout *glayout = new QGridLayout();
    QVBoxLayout *vlayout = new QVBoxLayout(m_scrollWidget);
    m_outlineIntroduction = new OutlineIntroduction(this);

    m_outlineCell_node = new OutlineCell(ONUTLINE_CELL_NODE, this);
    m_outlineCell_container = new OutlineCell(ONUTLINE_CELL_CONTAINER, this);
    m_outlineCell_image = new OutlineCell(ONUTLINE_CELL_IMAGE, this);
    m_outlineCell_template_container = new OutlineCell(ONUTLINE_CELL_TEMPLATE_CONTAINER, this);
    m_outlineCell_examine = new OutlineCell(ONUTLINE_CELL_EXAMINE, this);
    m_outlineCell_warning = new OutlineCell(ONUTLINE_CELL_NODE_WARNING, this);

    setOutlineCellNode();
    setOutlineCellContainer();
    setOutlineCellImage();
    setOutlineCellTemplateContainer();
    setOutlineCellExamine();
    setOutlineCellWarning();

    m_outlineCell_node->setMinimumHeight(280);
    m_outlineCell_warning->setMinimumHeight(280);
    m_outlineIntroduction->setMinimumHeight(204);

    glayout->addWidget(m_outlineCell_node, 0, 0);
    glayout->addWidget(m_outlineCell_container, 0, 1);
    glayout->addWidget(m_outlineCell_image, 0, 2);
    glayout->addWidget(m_outlineCell_template_container, 1, 0);
    glayout->addWidget(m_outlineCell_examine, 1, 1);
    glayout->addWidget(m_outlineCell_warning, 1, 2);

    glayout->setVerticalSpacing(10);
    glayout->setHorizontalSpacing(10);

    vlayout->addWidget(m_outlineIntroduction);
    vlayout->addLayout(glayout);
    vlayout->addStretch();
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->setSpacing(10);

#if 1
    m_scrollArea->setWidget(m_scrollWidget);

    //窗口变大的时候滚动区域的大小也可以跟随调整
    m_layout = new QHBoxLayout;
    m_layout->addWidget(m_scrollArea);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    this->setLayout(m_layout);
#endif

    this->setStyleSheet("border:none;");
}

void OutlineView::initConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::dashboardFinished, this, &OutlineView::getDashboardResult);

    connect(m_outlineCell_node, &OutlineCell::clicked, this, &OutlineView::outlineCellStepPages);
    connect(m_outlineCell_container, &OutlineCell::clicked, this, &OutlineView::outlineCellStepPages);
    connect(m_outlineCell_image, &OutlineCell::clicked, this, &OutlineView::outlineCellStepPages);
    connect(m_outlineCell_template_container, &OutlineCell::clicked, this, &OutlineView::outlineCellStepPages);
    connect(m_outlineCell_examine, &OutlineCell::clicked, this, &OutlineView::outlineCellStepPages);
    connect(m_outlineCell_warning, &OutlineCell::clicked, this, &OutlineView::outlineCellStepPages);
}

void OutlineView::updateInfo(QString keyword)
{
    InfoWorker::getInstance().dashboard(m_objId);
}

void OutlineView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    Page::paintEvent(event);
}

void OutlineView::showEvent(QShowEvent *event)
{
    m_timer->start(TIMEOUT);
    Page::showEvent(event);
}

void OutlineView::hideEvent(QHideEvent *event)
{
    m_timer->stop();
    Page::hideEvent(event);
}

void OutlineView::setOutlineCellNode()
{
    m_outlineCell_node->ui->label->setText(tr("node"));
    m_outlineCell_node->ui->Name_label->setText(tr("Number of nodes: "));
    m_outlineCell_node->ui->Name_counts->setText("0");

    m_outlineCell_node->ui->label_online_txt->setText(tr("online: "));
    m_outlineCell_node->ui->label_offline_txt->setText(tr("offline: "));
    m_outlineCell_node->ui->online_counts->setText(tr("0"));
    m_outlineCell_node->ui->offline_counts->setText(tr("0"));

    m_outlineCell_node->setIcon(":/images/node-number.png");
}

void OutlineView::setOutlineCellContainer()
{
    m_outlineCell_container->ui->label->setText(tr("Container"));
    m_outlineCell_container->ui->Name_label->setText(tr("Number of Container: "));
    m_outlineCell_container->ui->Name_counts->setText("0");

    m_outlineCell_container->ui->label_online_txt->setText(tr("online: "));
    m_outlineCell_container->ui->label_offline_txt->setText(tr("offline: "));
    m_outlineCell_container->ui->online_counts->setText(tr("0"));
    m_outlineCell_container->ui->offline_counts->setText(tr("0"));

    m_outlineCell_container->setIcon(":/images/container-number.png");
}

void OutlineView::setOutlineCellImage()
{
    m_outlineCell_image->ui->label->setText(tr("Image"));
    m_outlineCell_image->ui->Name_label->setText(tr("Number of Image: "));
    m_outlineCell_image->ui->Name_counts->setText("0");

    m_outlineCell_image->ui->label_online_txt->setText(tr("Image Capacity: "));
    m_outlineCell_image->ui->label_offline_txt->setText(tr("0G"));

    m_outlineCell_image->ui->label_online->deleteLater();
    m_outlineCell_image->ui->label_offline->deleteLater();
    m_outlineCell_image->ui->verticalLayout_6->deleteLater();
    m_outlineCell_image->ui->verticalLayout_7->deleteLater();
    m_outlineCell_image->ui->online_counts->deleteLater();
    m_outlineCell_image->ui->offline_counts->deleteLater();

    m_outlineCell_image->setIcon(":/images/image-number.png");
}

void OutlineView::setOutlineCellTemplateContainer()
{
    m_outlineCell_template_container->ui->label->setText(tr("Template"));
    m_outlineCell_template_container->ui->Name_label->setText(tr("Number of Template: "));
    m_outlineCell_template_container->ui->Name_counts->setText("0");

    m_outlineCell_template_container->ui->label_online_txt->deleteLater();
    m_outlineCell_template_container->ui->label_offline_txt->deleteLater();
    m_outlineCell_template_container->ui->label_online->deleteLater();
    m_outlineCell_template_container->ui->label_offline->deleteLater();
    m_outlineCell_template_container->ui->verticalLayout_6->deleteLater();
    m_outlineCell_template_container->ui->verticalLayout_7->deleteLater();
    m_outlineCell_template_container->ui->online_counts->deleteLater();
    m_outlineCell_template_container->ui->offline_counts->deleteLater();
    m_outlineCell_template_container->ui->horizontalLayout_2->deleteLater();
    m_outlineCell_template_container->ui->verticalLayout->setContentsMargins(0, 0, 0, 21);

    m_outlineCell_template_container->setIcon(":/images/container-template-number.png");
}

void OutlineView::setOutlineCellExamine()
{
    m_outlineCell_examine->ui->label->setText(tr("Approval"));
    m_outlineCell_examine->ui->Name_label->setText(tr("Number of Approval: "));
    m_outlineCell_examine->ui->Name_counts->setText("0");

    m_outlineCell_examine->ui->label_online_txt->deleteLater();
    m_outlineCell_examine->ui->label_offline_txt->deleteLater();
    m_outlineCell_examine->ui->label_online->deleteLater();
    m_outlineCell_examine->ui->label_offline->deleteLater();
    m_outlineCell_examine->ui->verticalLayout_6->deleteLater();
    m_outlineCell_examine->ui->verticalLayout_7->deleteLater();
    m_outlineCell_examine->ui->online_counts->deleteLater();
    m_outlineCell_examine->ui->offline_counts->deleteLater();
    m_outlineCell_examine->ui->horizontalLayout_2->deleteLater();
    m_outlineCell_examine->ui->verticalLayout->setContentsMargins(0, 0, 0, 21);

    m_outlineCell_examine->setIcon(":/images/approve-number.png");
}

void OutlineView::setOutlineCellWarning()
{
    m_outlineCell_warning->ui->label->setText(tr("Unread Warning"));
    m_outlineCell_warning->ui->Name_label->setText(tr("Number of Unread Warn: "));
    m_outlineCell_warning->ui->Name_counts->setText("0");

    m_outlineCell_warning->ui->label_online_txt->deleteLater();
    m_outlineCell_warning->ui->label_offline_txt->deleteLater();
    m_outlineCell_warning->ui->label_online->deleteLater();
    m_outlineCell_warning->ui->label_offline->deleteLater();
    m_outlineCell_warning->ui->verticalLayout_6->deleteLater();
    m_outlineCell_warning->ui->verticalLayout_7->deleteLater();
    m_outlineCell_warning->ui->online_counts->deleteLater();
    m_outlineCell_warning->ui->offline_counts->deleteLater();
    m_outlineCell_warning->ui->horizontalLayout_2->deleteLater();
    m_outlineCell_warning->ui->verticalLayout->setContentsMargins(0, 0, 0, 21);

    m_outlineCell_warning->setIcon(":/images/warning-number.png");
}

void OutlineView::getDashboardResult(const QString objId, const QPair<grpc::Status, sys::DashboardReply> &reply)
{
    if (m_objId != objId)
        return;

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Can't get system dashboard information!";
        return;
    }

    //node status
    int nodeTotal = reply.second.node().total_count();
    int nodeOnline = reply.second.node().online_count();
    int nodeOffline = reply.second.node().offline_count();
    m_outlineCell_node->ui->Name_counts->setText(QString::number(nodeTotal, 10));
    m_outlineCell_node->ui->online_counts->setText(QString::number(nodeOnline, 10));
    m_outlineCell_node->ui->offline_counts->setText(QString::number(nodeOffline, 10));

    //container status
    int containerTotal = reply.second.container().total_count();
    int containerOnline = reply.second.container().online_count();
    int containerOffline = reply.second.container().offline_count();
    m_outlineCell_container->ui->Name_counts->setText(QString::number(containerTotal, 10));
    m_outlineCell_container->ui->online_counts->setText(QString::number(containerOnline, 10));
    m_outlineCell_container->ui->offline_counts->setText(QString::number(containerOffline, 10));

    int templateTotal = reply.second.container().template_count();
    m_outlineCell_template_container->ui->Name_counts->setText(QString::number(templateTotal, 10));

    //image status
    int imageTotal = reply.second.image().total_count();
    m_outlineCell_image->ui->Name_counts->setText(QString::number(imageTotal, 10));

    auto imageSize = reply.second.image().total_size();
    double image_size_sum;
    image_size_sum = double(imageSize) / pow(2, 30);

    if (image_size_sum < 1)
    {
        image_size_sum = double(imageSize) / pow(2, 20);
        QString str = QString::number(image_size_sum, 'f', 2);
        m_outlineCell_image->ui->label_offline_txt->setText(str + "MB");
    }
    else
    {
        QString str = QString::number(image_size_sum, 'f', 2);
        m_outlineCell_image->ui->label_offline_txt->setText(str + "GB");
    }

    //audit status
    int approveCount = reply.second.audit().image_to_approve_count();
    m_outlineCell_examine->ui->Name_counts->setText(QString::number(approveCount, 10));
    emit sigApproveSumNums(approveCount);

    //warnning status
    auto unreadWarnCount = reply.second.log().unread_warn_count();
    m_outlineCell_warning->ui->Name_counts->setText(QString::number(unreadWarnCount, 10));
    emit sigWarnSumNums(unreadWarnCount);

    KLOG_DEBUG() << "Node info:"
                 << "total:" << nodeTotal << "online number: " << nodeOnline << "offline number:" << nodeOffline << "\n"
                 << "Container info:"
                 << "total:" << containerTotal << "online number:" << containerOnline << "offline number:" << containerOffline << "\n"
                 << "Template info:"
                 << "tatol:" << templateTotal << "\n"
                 << "Image info:"
                 << "tatol:" << imageTotal << "size:" << imageSize << "\n"
                 << "Unapproved info:"
                 << "tatol:" << approveCount << "\n"
                 << "Unread warnning info:"
                 << "tatol: " << unreadWarnCount << "\n";
}
