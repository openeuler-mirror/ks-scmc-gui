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
#include <QVBoxLayout>
#include <cmath>
#include "outline-cell.h"
#include "ui_outline-cell.h"

using namespace std;

OutlineView::OutlineView(QWidget *parent) : Page(parent),
                                            m_outlineCell_node(static_cast<OutlineCell *>(parent)),
                                            m_outlineCell_container(static_cast<OutlineCell *>(parent)),
                                            m_outlineCell_image(static_cast<OutlineCell *>(parent)),
                                            m_outlineCell_template_container(static_cast<OutlineCell *>(parent)),
                                            m_outlineCell_examine(static_cast<OutlineCell *>(parent)),
                                            m_outlineCell_warning(static_cast<OutlineCell *>(parent)),
                                            m_outlineIntroduction(nullptr)
{
    m_objId = InfoWorker::generateId(this);
    initUI();
    initConnect();
}

OutlineView::~OutlineView()
{
    if (m_outlineIntroduction)
    {
        delete m_outlineIntroduction;
        m_outlineIntroduction = nullptr;
    }
    if (m_outlineCell_node)
    {
        delete m_outlineCell_node;
        m_outlineCell_node = nullptr;
    }
    if (m_outlineCell_container)
    {
        delete m_outlineCell_container;
        m_outlineCell_container = nullptr;
    }
    if (m_outlineCell_image)
    {
        delete m_outlineCell_image;
        m_outlineCell_image = nullptr;
    }
    if (m_outlineCell_template_container)
    {
        delete m_outlineCell_template_container;
        m_outlineCell_template_container = nullptr;
    }
    if (m_outlineCell_examine)
    {
        delete m_outlineCell_examine;
        m_outlineCell_examine = nullptr;
    }
    if (m_outlineCell_warning)
    {
        delete m_outlineCell_warning;
        m_outlineCell_warning = nullptr;
    }
}

void OutlineView::getlNodeList()
{
    InfoWorker::getInstance().listNode(m_objId);
}

void OutlineView::getContainerList()
{
    std::vector<int64_t> vecNodeId;
    InfoWorker::getInstance().listContainer(m_objId, vecNodeId, true);
}

void OutlineView::getImageList()
{
    InfoWorker::getInstance().listDBImage(m_objId);
}

void OutlineView::getlTemplateContainer()
{
    InfoWorker::getInstance().listTemplate(m_objId);
}

void OutlineView::getWarnNums()
{
    InfoWorker::getInstance().listNode(m_objId);
}

QWidget *OutlineView::getScrollCenterWidget()
{
    return m_scrollWidget;
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
    m_outlineIntroduction = new OutlineIntroduction;

    m_outlineCell_node = new OutlineCell;
    m_outlineCell_container = new OutlineCell;
    m_outlineCell_image = new OutlineCell;
    m_outlineCell_template_container = new OutlineCell;
    m_outlineCell_examine = new OutlineCell;
    m_outlineCell_warning = new OutlineCell;

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

//    glayout->setRowStretch(3,1);

//    m_scrollWidget->setLayout(glayout);
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
    connect(&InfoWorker::getInstance(), &InfoWorker::listNodeFinished, this, &OutlineView::getOutlineCellNodeNums);
    connect(&InfoWorker::getInstance(), &InfoWorker::listContainerFinished, this, &OutlineView::getOutlineCellContainerNums);
    connect(&InfoWorker::getInstance(), &InfoWorker::listDBImageFinished, this, &OutlineView::getOutlineCellImageNums);
    connect(&InfoWorker::getInstance(), &InfoWorker::listTemplateFinished, this, &OutlineView::getOutlineCellTemplateContainerNums);
    connect(&InfoWorker::getInstance(), &InfoWorker::listNodeFinished, this, &OutlineView::getOutlineCellWarningNums);

    connect(m_outlineCell_node, &OutlineCell::outlineCellStep, this, &OutlineView::outlineCellStepPages);
    connect(m_outlineCell_node, &OutlineCell::isPress, this, &OutlineView::emitOutlineCellType);

    connect(m_outlineCell_container, &OutlineCell::outlineCellStep, this, &OutlineView::outlineCellStepPages);
    connect(m_outlineCell_container, &OutlineCell::isPress, this, &OutlineView::emitOutlineCellType);

    connect(m_outlineCell_image, &OutlineCell::outlineCellStep, this, &OutlineView::outlineCellStepPages);
    connect(m_outlineCell_image, &OutlineCell::isPress, this, &OutlineView::emitOutlineCellType);

    connect(m_outlineCell_template_container, &OutlineCell::outlineCellStep, this, &OutlineView::outlineCellStepPages);
    connect(m_outlineCell_template_container, &OutlineCell::isPress, this, &OutlineView::emitOutlineCellType);

    connect(m_outlineCell_examine, &OutlineCell::outlineCellStep, this, &OutlineView::outlineCellStepPages);
    connect(m_outlineCell_examine, &OutlineCell::isPress, this, &OutlineView::emitOutlineCellType);

    connect(m_outlineCell_warning, &OutlineCell::outlineCellStep, this, &OutlineView::outlineCellStepPages);
    connect(m_outlineCell_warning, &OutlineCell::isPress, this, &OutlineView::emitOutlineCellType);
}

void OutlineView::emitOutlineCellType()
{
    if (m_outlineCell_node->is_mouse_Press)
    {
        emit outlineCellStepPages(ONUTLINE_CELL_NODE);
        m_outlineCell_node->is_mouse_Press = false;
    }
    if (m_outlineCell_container->is_mouse_Press)
    {
        emit outlineCellStepPages(ONUTLINE_CELL_CONTAINER);
        m_outlineCell_container->is_mouse_Press = false;
    }
    if (m_outlineCell_image->is_mouse_Press)
    {
        emit outlineCellStepPages(ONUTLINE_CELL_IMAGE);
        m_outlineCell_image->is_mouse_Press = false;
    }
    if (m_outlineCell_template_container->is_mouse_Press)
    {
        emit outlineCellStepPages(ONUTLINE_CELL_TEMPLATE_CONTAINER);
        m_outlineCell_template_container->is_mouse_Press = false;
    }
    if (m_outlineCell_examine->is_mouse_Press)
    {
        emit outlineCellStepPages(ONUTLINE_CELL_EXAMINE);
        m_outlineCell_examine->is_mouse_Press = false;
    }
    if (m_outlineCell_warning->is_mouse_Press)
    {
        emit outlineCellStepPages(ONUTLINE_CELL_NODE_WARNING);
        m_outlineCell_warning->is_mouse_Press = false;
    }
}

void OutlineView::updateInfo(QString keyword)
{
    getlNodeList();
    getContainerList();
    getImageList();
    getlTemplateContainer();
    getWarnNums();
    //    m_outlineCell_image->ui->Name_counts->setText("666");
}

void OutlineView::updateWarningSums()
{
    getWarnNums();
}

QString OutlineView::getApproveSums()
{
    return m_outlineCell_examine->ui->Name_counts->text();
}

QString OutlineView::getWarningSums()
{
    return m_outlineCell_warning->ui->Name_counts->text();
}

void OutlineView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void OutlineView::getOutlineCellNodeNums(const QString objId, const QPair<grpc::Status, node::ListReply> &reply)
{
    KLOG_INFO() << "getOutlineCellNodeNums" << m_objId << objId;
    if (m_objId == objId)
    {
        int online = 0;
        int offline = 0;
        int size = 0;
        if (reply.first.ok())
        {
            size = reply.second.nodes_size();

            for (auto node : reply.second.nodes())
            {
                if (node.has_status())
                {
                    if (node.status().state() == 10)
                        online++;
                    if (node.status().state() == 0)
                        offline++;
                }
            }
        }
        m_outlineCell_node->ui->Name_counts->setText(QString::number(size, 10));
        m_outlineCell_node->ui->online_counts->setText(QString::number(online, 10));
        m_outlineCell_node->ui->offline_counts->setText(QString::number(offline, 10));
    }
}

void OutlineView::getOutlineCellContainerNums(const QString objId, const QPair<grpc::Status, container::ListReply> &reply)
{
    KLOG_INFO() << "getOutlineCellContainerNums" << m_objId << objId;
    if (m_objId == objId)
    {
        int size = 0;
        int online = 0;
        int offline = 0;
        if (reply.first.ok())
        {
            size = reply.second.containers_size();

            for (auto container : reply.second.containers())
            {
                if (container.info().state() == "running")
                    online++;
                else
                    offline++;
            }
        }
        m_outlineCell_container->ui->Name_counts->setText(QString::number(size, 10));
        m_outlineCell_container->ui->online_counts->setText(QString::number(online, 10));
        m_outlineCell_container->ui->offline_counts->setText(QString::number(offline, 10));
    }
}

void OutlineView::getOutlineCellImageNums(const QString objId, const QPair<grpc::Status, image::ListDBReply> &reply)
{
    KLOG_INFO() << "getOutlineCellImageNums" << m_objId << objId;
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            int row = 0;
            int size;
            long int image_size = 0;
            size = reply.second.images_size();
            for (auto image : reply.second.images())
            {
                image_size += image.size();
                //    int size = reply.second.
                row++;
            }

            double image_size_sum;
            image_size_sum = double(image_size) / pow(2, 30);

            if (image_size_sum < 1)
            {
                image_size_sum = double(image_size) / pow(2, 20);
                QString str = QString::number(image_size_sum, 'f', 2);

                m_outlineCell_image->ui->Name_counts->setText(QString::number(size, 10));
                m_outlineCell_image->ui->label_offline_txt->setText(str + "MB");
            }
            else
            {
                QString str = QString::number(image_size_sum, 'f', 2);

                m_outlineCell_image->ui->Name_counts->setText(QString::number(size, 10));
                m_outlineCell_image->ui->label_offline_txt->setText(str + "GB");
            }

            getOutlineCellExamineNums(objId, reply);
        }
    }
}

void OutlineView::getOutlineCellTemplateContainerNums(const QString objId, const QPair<grpc::Status, container::ListTemplateReply> &reply)
{
    KLOG_INFO() << "getOutlineCellTemplateContainerNums" << m_objId << objId;
    if (m_objId == objId)
    {
        int size = 0;
        if (reply.first.ok())
        {
            size = reply.second.data_size();
        }
        m_outlineCell_template_container->ui->Name_counts->setText(QString::number(size, 10));
    }
}

void OutlineView::getOutlineCellExamineNums(const QString objId, const QPair<grpc::Status, image::ListDBReply> &reply)
{
    KLOG_INFO() << "getOutlineCellExamineNums" << m_objId << objId;
    if (m_objId == objId)
    {
        int count = 0;
        if (reply.first.ok())
        {
            int row = 0;

            for (auto image : reply.second.images())
            {
                if (image.approval_status() == 0)
                    count++;
                row++;
            }
            emit sigApproveSumNums(count);
        }
        m_outlineCell_examine->ui->Name_counts->setText(QString::number(count, 10));
    }
}

void OutlineView::getOutlineCellWarningNums(const QString objId, const QPair<grpc::Status, node::ListReply> &reply)
{
    KLOG_INFO() << "getOutlineCellWarningNums" << m_objId << objId;
    if (m_objId == objId)
    {
        int64_t read_warn_count = 0;
        if (reply.first.ok())
        {
            int size = reply.second.nodes_size();
            if (size <= 0)
                return;
            int row = 0;
            for (auto node : reply.second.nodes())
            {
                read_warn_count += node.unread_warn();
                row++;
            }
        }
        emit sigWarnSumNums(int(read_warn_count));
        m_outlineCell_warning->ui->Name_counts->setText(QString::number(read_warn_count, 10));
    }
}

void OutlineView::setOutlineCellNode()
{
    m_outlineCell_node->ui->label->setText(tr("node"));
    m_outlineCell_node->ui->Name_label->setStyleSheet("QLabel {"
                                                      "background: transparent;"
                                                      "border:none;"
                                                      "font: NotoSansCJKsc-Regular;"
                                                      "font-size: 22px;"
                                                      "border-radius:0px;"
                                                      "color: #ffffff;}");
    m_outlineCell_node->ui->Name_counts->setStyleSheet("QLabel {"
                                                       "background: transparent;"
                                                       "border:none;"
                                                       "font: NotoSansCJKsc-Regular;"
                                                       "font-size: 22px;"
                                                       "border-radius:0px;"
                                                       "color: #ffffff;}");
    m_outlineCell_node->ui->Name_label->setText(tr("Number of nodes: "));
    m_outlineCell_node->ui->Name_counts->setText("0");

    m_outlineCell_node->ui->label_online_txt->setStyleSheet("QLabel {"
                                                            "background: transparent;"
                                                            "border:none;"
                                                            "font: NotoSansCJKsc-Regular;"
                                                            "font-size: 14px;"
                                                            "border-radius:0px;"
                                                            "color: #ffffff;}");
    m_outlineCell_node->ui->label_online_txt->setText(tr("online: "));

    m_outlineCell_node->ui->label_offline_txt->setStyleSheet("QLabel {"
                                                             "background: transparent;"
                                                             "border:none;"
                                                             "font: NotoSansCJKsc-Regular;"
                                                             "font-size: 14px;"
                                                             "border-radius:0px;"
                                                             "color: #ffffff;}");
    m_outlineCell_node->ui->label_offline_txt->setText(tr("offline: "));

    m_outlineCell_node->ui->online_counts->setStyleSheet("QLabel {"
                                                         "background: transparent;"
                                                         "border:none;"
                                                         "font: NotoSansCJKsc-Regular;"
                                                         "font-size: 36px;"
                                                         "border-radius:0px;"
                                                         "color: #ffffff;}");
    m_outlineCell_node->ui->online_counts->setText(tr("0"));
    m_outlineCell_node->ui->offline_counts->setStyleSheet("QLabel {"
                                                          "background: transparent;"
                                                          "border:none;"
                                                          "font: NotoSansCJKsc-Regular;"
                                                          "font-size: 36px;"
                                                          "border-radius:0px;"
                                                          "color: #ffffff;}");
    m_outlineCell_node->ui->offline_counts->setText(tr("0"));

    m_outlineCell_node->ui->outline_pix->setStyleSheet("QLabel{"
                                                       //                                                        "border-image:url(:/images/node-number.png);"
                                                       "background-image:url(:/images/node-number.png);"
                                                       "background-position:center;"
                                                       "background-repeat:no-repeat;"
                                                       "}");
    m_outlineCell_node->ui->outline_pix->setMinimumSize(78, 78);
}

void OutlineView::setOutlineCellContainer()
{
    m_outlineCell_container->ui->label->setText(tr("Container"));
    m_outlineCell_container->ui->Name_label->setStyleSheet("QLabel {"
                                                           "background: transparent;"
                                                           "border:none;"
                                                           "font: NotoSansCJKsc-Regular;"
                                                           "font-size: 22px;"
                                                           "border-radius:0px;"
                                                           "color: #ffffff;}");
    m_outlineCell_container->ui->Name_counts->setStyleSheet("QLabel {"
                                                            "background: transparent;"
                                                            "border:none;"
                                                            "font: NotoSansCJKsc-Regular;"
                                                            "font-size: 22px;"
                                                            "border-radius:0px;"
                                                            "color: #ffffff;}");
    m_outlineCell_container->ui->Name_label->setText(tr("Number of Container: "));
    m_outlineCell_container->ui->Name_counts->setText("0");

    m_outlineCell_container->ui->label_online_txt->setStyleSheet("QLabel {"
                                                                 "background: transparent;"
                                                                 "border:none;"
                                                                 "font: NotoSansCJKsc-Regular;"
                                                                 "font-size: 14px;"
                                                                 "border-radius:0px;"
                                                                 "color: #ffffff;}");
    m_outlineCell_container->ui->label_online_txt->setText(tr("online: "));

    m_outlineCell_container->ui->label_offline_txt->setStyleSheet("QLabel {"
                                                                  "background: transparent;"
                                                                  "border:none;"
                                                                  "font: NotoSansCJKsc-Regular;"
                                                                  "font-size: 14px;"
                                                                  "border-radius:0px;"
                                                                  "color: #ffffff;}");
    m_outlineCell_container->ui->label_offline_txt->setText(tr("offline: "));

    m_outlineCell_container->ui->online_counts->setStyleSheet("QLabel {"
                                                              "background: transparent;"
                                                              "border:none;"
                                                              "font: NotoSansCJKsc-Regular;"
                                                              "font-size: 36px;"
                                                              "border-radius:0px;"
                                                              "color: #ffffff;}");
    m_outlineCell_container->ui->online_counts->setText(tr("0"));
    m_outlineCell_container->ui->offline_counts->setStyleSheet("QLabel {"
                                                               "background: transparent;"
                                                               "border:none;"
                                                               "font: NotoSansCJKsc-Regular;"
                                                               "font-size: 36px;"
                                                               "border-radius:0px;"
                                                               "color: #ffffff;}");
    m_outlineCell_container->ui->offline_counts->setText(tr("0"));

    m_outlineCell_container->ui->outline_pix->setStyleSheet("QLabel{"
                                                            //                                                        "border-image:url(:/images/node-number.png);"
                                                            "background-image:url(:/images/container-number.png);"
                                                            "background-position:center;"
                                                            "background-repeat:no-repeat;"
                                                            "}");
    m_outlineCell_container->ui->outline_pix->setMinimumSize(78, 78);
}

void OutlineView::setOutlineCellImage()
{
    m_outlineCell_image->ui->label->setText(tr("Image"));
    m_outlineCell_image->ui->Name_label->setStyleSheet("QLabel {"
                                                       "background: transparent;"
                                                       "border:none;"
                                                       "font: NotoSansCJKsc-Regular;"
                                                       "font-size: 22px;"
                                                       "border-radius:0px;"
                                                       "color: #ffffff;}");
    m_outlineCell_image->ui->Name_counts->setStyleSheet("QLabel {"
                                                        "background: transparent;"
                                                        "border:none;"
                                                        "font: NotoSansCJKsc-Regular;"
                                                        "font-size: 22px;"
                                                        "border-radius:0px;"
                                                        "color: #ffffff;}");
    m_outlineCell_image->ui->Name_label->setText(tr("Number of Image: "));
    m_outlineCell_image->ui->Name_counts->setText("0");

    m_outlineCell_image->ui->label_online_txt->setStyleSheet("QLabel {"
                                                             "background: transparent;"
                                                             "border:none;"
                                                             "font: NotoSansCJKsc-Regular;"
                                                             "font-size: 14px;"
                                                             "border-radius:0px;"
                                                             "color: #ffffff;}");
    m_outlineCell_image->ui->label_online_txt->setText(tr("Image Capacity: "));

    m_outlineCell_image->ui->label_offline_txt->setStyleSheet("QLabel {"
                                                              "background: transparent;"
                                                              "border:none;"
                                                              "font: NotoSansCJKsc-Regular;"
                                                              "font-size: 14px;"
                                                              "border-radius:0px;"
                                                              "color: #ffffff;}");
    m_outlineCell_image->ui->label_offline_txt->setText(tr("0G"));

    m_outlineCell_image->ui->label_online->deleteLater();
    m_outlineCell_image->ui->label_offline->deleteLater();
    m_outlineCell_image->ui->verticalLayout_6->deleteLater();
    m_outlineCell_image->ui->verticalLayout_7->deleteLater();
    m_outlineCell_image->ui->online_counts->deleteLater();
    m_outlineCell_image->ui->offline_counts->deleteLater();

    m_outlineCell_image->ui->outline_pix->setStyleSheet("QLabel{"
                                                        //                                                        "border-image:url(:/images/image-number.png);"
                                                        "background-image:url(:/images/image-number.png);"
                                                        "background-position:center;"
                                                        "background-repeat:no-repeat;"
                                                        "}");
    m_outlineCell_image->ui->outline_pix->setMinimumSize(78, 78);
}

void OutlineView::setOutlineCellTemplateContainer()
{
    m_outlineCell_template_container->ui->label->setText(tr("Template Container"));
    m_outlineCell_template_container->ui->Name_label->setStyleSheet("QLabel {"
                                                                    "background: transparent;"
                                                                    "border:none;"
                                                                    "font: NotoSansCJKsc-Regular;"
                                                                    "font-size: 22px;"
                                                                    "border-radius:0px;"
                                                                    "color: #ffffff;}");
    m_outlineCell_template_container->ui->Name_counts->setStyleSheet("QLabel {"
                                                                     "background: transparent;"
                                                                     "border:none;"
                                                                     "font: NotoSansCJKsc-Regular;"
                                                                     "font-size: 22px;"
                                                                     "border-radius:0px;"
                                                                     "color: #ffffff;}");
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

    m_outlineCell_template_container->ui->outline_pix->setStyleSheet("QLabel{"
                                                                     "background-image:url(:/images/container-template-number.png);"
                                                                     "background-position:center;"
                                                                     "background-repeat:no-repeat;"
                                                                     "}");
    m_outlineCell_template_container->ui->outline_pix->setMinimumSize(78, 78);
}

void OutlineView::setOutlineCellExamine()
{
    m_outlineCell_examine->ui->label->setText(tr("Reviewed"));
    m_outlineCell_examine->ui->Name_label->setStyleSheet("QLabel {"
                                                         "background: transparent;"
                                                         "border:none;"
                                                         "font: NotoSansCJKsc-Regular;"
                                                         "font-size: 22px;"
                                                         "border-radius:0px;"
                                                         "color: #ffffff;}");
    m_outlineCell_examine->ui->Name_counts->setStyleSheet("QLabel {"
                                                          "background: transparent;"
                                                          "border:none;"
                                                          "font: NotoSansCJKsc-Regular;"
                                                          "font-size: 22px;"
                                                          "border-radius:0px;"
                                                          "color: #ffffff;}");
    m_outlineCell_examine->ui->Name_label->setText(tr("Number of Reviewed: "));
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

    m_outlineCell_examine->ui->outline_pix->setStyleSheet("QLabel{"
                                                          "background-image:url(:/images/approve-number.png);"
                                                          "background-position:center;"
                                                          "background-repeat:no-repeat;"
                                                          "}");
    m_outlineCell_examine->ui->outline_pix->setMinimumSize(78, 78);
}

void OutlineView::setOutlineCellWarning()
{
    m_outlineCell_warning->ui->label->setText(tr("Give Alarm"));
    m_outlineCell_warning->ui->Name_label->setStyleSheet("QLabel {"
                                                         "background: transparent;"
                                                         "border:none;"
                                                         "font: NotoSansCJKsc-Regular;"
                                                         "font-size: 22px;"
                                                         "border-radius:0px;"
                                                         "color: #ffffff;}");
    m_outlineCell_warning->ui->Name_counts->setStyleSheet("QLabel {"
                                                          "background: transparent;"
                                                          "border:none;"
                                                          "font: NotoSansCJKsc-Regular;"
                                                          "font-size: 22px;"
                                                          "border-radius:0px;"
                                                          "color: #ffffff;}");
    m_outlineCell_warning->ui->Name_label->setText(tr("Number of Give Alarm: "));
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

    m_outlineCell_warning->ui->outline_pix->setStyleSheet("QLabel{"
                                                          "background-image:url(:/images/warning-number.png);"
                                                          "background-position:center;"
                                                          "background-repeat:no-repeat;"
                                                          "}");
    m_outlineCell_warning->ui->outline_pix->setMinimumSize(78, 78);
}
