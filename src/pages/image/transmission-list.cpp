/**
 * @file          /ks-scmc-gui/src/pages/image/transmission-list.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "transmission-list.h"
#include <kiran-log/qt5-log-i.h>
#include <QLabel>
#include <QPainter>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "transmission-item.h"

#define LIST_ITEM_HEIGHT 80
#define LIST_ITEM_WIDTH 360
#define LIST_ITEM_SPACE 2
#define LIST_ITEM_VISIBLE_NUM 5
#define LIST_HEIGHT LIST_ITEM_VISIBLE_NUM *(LIST_ITEM_HEIGHT + LIST_ITEM_SPACE)
#define LIST_WIDTH 375

TransmissionList::TransmissionList(QWidget *parent) : QWidget(parent), m_listWidget(nullptr), m_stackedWidget(nullptr), m_transfersNum(0)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    initUI();
    m_transfersItems.clear();
}

TransmissionList::~TransmissionList()
{
}

void TransmissionList::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

void TransmissionList::addItem(QString name, QString version, ImageTransmissionStatus status, int rate)
{
    KLOG_INFO() << "addItem" << name << version;
    QListWidgetItem *newItem = nullptr;
    TransmissionItem *customItem = nullptr;

    newItem = new QListWidgetItem(m_listWidget);

    customItem = new TransmissionItem(m_listWidget);
    customItem->setName(name);
    customItem->setVersion(version);
    customItem->setStatus(status);
    customItem->setRate(rate);

    m_listWidget->addItem(newItem);
    m_listWidget->setItemWidget(newItem, customItem);
    newItem->setTextAlignment(Qt::AlignVCenter);

    newItem->setSizeHint(QSize(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
    m_listWidget->setGridSize(QSize(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT + LIST_ITEM_SPACE));

    auto num = ++m_transfersNum;
    setTransfersNum(num);
    m_transfersItems.append(customItem);
    adjustSize();

    connect(customItem, &TransmissionItem::sigClose, this, &TransmissionList::deleteItem);
}

void TransmissionList::updateItem(QString name, QString version, ImageTransmissionStatus status, int rate)
{
    if (!m_transfersItems.size())
        return;

    QString itemMark = name + "-" + version;
    foreach (auto item, m_transfersItems)
    {
        if (item->name() + "-" + item->version() == itemMark)
        {
            item->setStatus(status);
            item->setRate(rate);
            break;
        }
    }
}

void TransmissionList::removeItem(QString name, QString version)
{
    if (!m_transfersItems.size())
        return;

    QString itemMark = name + "-" + version;
    foreach (auto item, m_transfersItems)
    {
        if (item->name() + "-" + item->version() == itemMark)
        {
            m_transfersItems.removeOne(item);
            auto num = --m_transfersNum;
            setTransfersNum(num);
            delete item;
            item = nullptr;
            break;
        }
    }
}

void TransmissionList::initUI()
{
    setWindowFlags(Qt::Widget | Qt::Popup | Qt::FramelessWindowHint);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(1);
    setFixedWidth(LIST_ITEM_WIDTH);
    setFixedHeight(LIST_ITEM_HEIGHT);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget);
    mainLayout->addStretch();

    QWidget *defaultWidget = new QWidget(m_stackedWidget);
    defaultWidget->setObjectName("defaultWidget");
    defaultWidget->setStyleSheet("#defaultWidget{border-radius:4px;background:#222222;border:1px solid #393939;}");
    QVBoxLayout *layout = new QVBoxLayout(defaultWidget);
    layout->setMargin(0);
    QLabel *defaultLabel = new QLabel(tr("No transfer task"), defaultWidget);
    defaultLabel->setAlignment(Qt::AlignCenter);
    defaultWidget->setFixedHeight(LIST_ITEM_HEIGHT);
    layout->addWidget(defaultLabel, Qt::AlignCenter);

    m_listWidget = new QListWidget(this);
    m_listWidget->setObjectName("transmissionListWidget");
    m_listWidget->setResizeMode(QListView::Adjust);
    m_listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listWidget->setFixedHeight(LIST_HEIGHT);

    m_stackedWidget->addWidget(defaultWidget);
    m_stackedWidget->addWidget(m_listWidget);
}

void TransmissionList::adjustSize()
{
    //设置高度
    auto height = 0;
    if (0 == m_listWidget->count())
    {
        height = LIST_ITEM_HEIGHT;
    }
    else if (m_listWidget->count() > 0 && m_listWidget->count() <= LIST_ITEM_VISIBLE_NUM)
    {
        height = (LIST_ITEM_HEIGHT + LIST_ITEM_SPACE) * m_listWidget->count();
    }
    else
    {
        height = (LIST_ITEM_HEIGHT + LIST_ITEM_SPACE) * LIST_ITEM_VISIBLE_NUM;
    }
    m_listWidget->setFixedHeight(height);
    setFixedHeight(height);
}

int TransmissionList::getTransfersNum()
{
    return m_transfersNum;
}

void TransmissionList::setTransfersNum(int num)
{
    m_transfersNum = num;
    KLOG_INFO() << "set transfers num" << num;
    if (num > 0)
        m_stackedWidget->setCurrentIndex(1);
    else
        m_stackedWidget->setCurrentIndex(0);
    emit transfersNumChanged(num);
}

void TransmissionList::deleteItem()
{
    TransmissionItem *transmissionItem = qobject_cast<TransmissionItem *>(sender());
    int row = 0;
    while (row < m_listWidget->count())
    {
        QListWidgetItem *item = m_listWidget->item(row);
        if (m_listWidget->itemWidget(item) == transmissionItem)
        {
            QListWidgetItem *delItem = m_listWidget->takeItem(m_listWidget->row(item));
            m_transfersItems.removeAt(row);
            auto num = --m_transfersNum;
            setTransfersNum(num);
            adjustSize();
            emit transferItemDeleted(transmissionItem->name(), transmissionItem->version(), transmissionItem->status());

            delete delItem;
            delItem = nullptr;

            break;
        }
        row++;
    }
}
