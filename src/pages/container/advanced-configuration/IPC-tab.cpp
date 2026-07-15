/**
 * @file          src/pages/container/advanced-configuration/IPC.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "IPC-tab.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>

IPCTab::IPCTab(QWidget* parent) : QWidget(parent)
{
    initUI();
}

void IPCTab::setIPC()
{
}

int IPCTab::getIPC()
{
    return 0;
}

void IPCTab::initUI()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto layout = new QHBoxLayout();
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);

    auto label = new QLabel(tr("IPC"), this);
    m_combobox = new QComboBox(this);
    m_combobox->setFixedSize(400, 36);
    m_combobox->addItem(tr("None"));

    layout->addWidget(label);
    layout->addWidget(m_combobox);
    layout->addStretch();

    mainLayout->addLayout(layout);
    mainLayout->addStretch();
}
