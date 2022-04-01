#include "transmission-list.h"
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "transmission-item.h"
TransmissionList::TransmissionList(QWidget *parent) : QWidget(parent), m_listWidget(nullptr)
{
    initUI();
}

TransmissionList::~TransmissionList()
{
}

void TransmissionList::addItem(QString name, QString version, QString status, int rate)
{
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
}

void TransmissionList::initUI()
{
    setWindowFlags(Qt::Widget | Qt::Popup | Qt::FramelessWindowHint);
    setFixedWidth(360);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget);
    mainLayout->addStretch();

    QWidget *defaultWidget = new QWidget(m_stackedWidget);
    QVBoxLayout *layout = new QVBoxLayout(defaultWidget);
    layout->setMargin(0);
    QLabel *defaultLabel = new QLabel(tr("No transfer task"), defaultWidget);
    defaultLabel->setAlignment(Qt::AlignHCenter);
    defaultLabel->setFixedHeight(80);
    layout->addWidget(defaultLabel, Qt::AlignHCenter);
    m_stackedWidget->addWidget(defaultLabel);

    m_listWidget = new QListWidget(this);
    m_stackedWidget->addWidget(m_listWidget);
}

int TransmissionList::getTransfersNum()
{
    return m_transfersNum;
}

void TransmissionList::setTransfersNum(int num)
{
    m_transfersNum = num;
    if (num > 0)
        m_stackedWidget->setCurrentIndex(1);
    else
        m_stackedWidget->setCurrentIndex(0);
    emit transfersNumChanged(num);
}
