#include "transmission-list.h"
#include <kiran-log/qt5-log-i.h>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "transmission-item.h"
TransmissionList::TransmissionList(QWidget *parent) : QWidget(parent), m_listWidget(nullptr), m_stackedWidget(nullptr), m_transfersNum(0)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    initUI();
    m_transfersItems.clear();
}

TransmissionList::~TransmissionList()
{
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

    newItem->setSizeHint(QSize(360, 80));
    m_listWidget->setGridSize(QSize(360, 82));

    auto num = ++m_transfersNum;
    setTransfersNum(num);
    m_transfersItems.append(customItem);
    adjustSize();

    connect(customItem, &TransmissionItem::sigClose, this, &TransmissionList::deleteItem);
}

void TransmissionList::updateItem(QString name, QString version, ImageTransmissionStatus status, int rate)
{
    QString itemMark = name + "-" + version;
    if (m_transfersItems.size() != 0)
    {
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
}

void TransmissionList::removeItem(QString name, QString version)
{
    QString itemMark = name + "-" + version;
    if (m_transfersItems.size() != 0)
    {
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
}

void TransmissionList::initUI()
{
    setWindowFlags(Qt::Widget | Qt::Popup | Qt::FramelessWindowHint);
    setFixedWidth(360);
    setFixedHeight(80);

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
    defaultWidget->setFixedHeight(80);
    layout->addWidget(defaultLabel, Qt::AlignCenter);

    m_listWidget = new QListWidget(this);
    m_listWidget->setObjectName("transmissionListWidget");

    m_stackedWidget->addWidget(defaultWidget);
    m_stackedWidget->addWidget(m_listWidget);
}

void TransmissionList::adjustSize()
{
    if (m_transfersItems.size() > 0)
        this->setFixedHeight(m_transfersItems.size() * (80 + 2));
}

int TransmissionList::getTransfersNum()
{
    return m_transfersNum;
}

void TransmissionList::setTransfersNum(int num)
{
    m_transfersNum = num;
    KLOG_INFO() << "setTransfersNum" << num;
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
