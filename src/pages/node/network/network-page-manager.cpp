#include "network-page-manager.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include "network-list-page.h"
#include "table-page.h"
NetworkPageManager::NetworkPageManager(QWidget *parent) : Page(parent)

{
    initUI();
}

void NetworkPageManager::updateInfo(QString keyword)
{
    if (m_nodeId > 0)
    {
        m_cbNetwork->setCurrentIndex(NETWORK_IFS_TYPE_REAL);
        m_stackedWidget->setCurrentIndex(NETWORK_IFS_TYPE_REAL);
        m_physicalPage->updateInfo();
    }
}

void NetworkPageManager::setNodeId(int64_t nodeId)
{
    m_nodeId = nodeId;
    m_physicalPage->setNodeId(nodeId);
    m_virtPage->setNodeId(nodeId);
}

void NetworkPageManager::changeTable(int index)
{
    if (m_cbNetwork->currentData().toInt() == NETWORK_IFS_TYPE_REAL)
    {
        m_stackedWidget->setCurrentIndex(NETWORK_IFS_TYPE_REAL);
        m_physicalPage->updateInfo();
    }
    else
    {
        m_stackedWidget->setCurrentIndex(NETWORK_IFS_TYPE_VIRT);
        m_virtPage->updateInfo();
    }
}

void NetworkPageManager::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 10, 0, 0);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(10);
    hLayout->setMargin(0);
    QLabel *label = new QLabel(tr("Network:"), this);
    m_cbNetwork = new QComboBox(this);
    m_cbNetwork->setFixedSize(238, 32);
    m_cbNetwork->addItem(tr("Real Interface"), NETWORK_IFS_TYPE_REAL);
    m_cbNetwork->addItem(tr("Virt Interface"), NETWORK_IFS_TYPE_VIRT);
    m_cbNetwork->setItemDelegate(new QStyledItemDelegate(this));
    hLayout->addWidget(label);
    hLayout->addWidget(m_cbNetwork);
    hLayout->addStretch();

    m_stackedWidget = new QStackedWidget(this);
    createPhysicalNetTable();
    createVirtNetTable();

    mainLayout->addLayout(hLayout);
    mainLayout->addWidget(m_stackedWidget);

    connect(m_cbNetwork, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NetworkPageManager::changeTable);
}

void NetworkPageManager::createPhysicalNetTable()
{
    m_physicalPage = new NetworkListPage(NETWORK_IFS_TYPE_REAL, this);
    m_stackedWidget->addWidget(m_physicalPage);
}

void NetworkPageManager::createVirtNetTable()
{
    m_virtPage = new NetworkListPage(NETWORK_IFS_TYPE_VIRT, this);
    m_stackedWidget->addWidget(m_virtPage);
}
