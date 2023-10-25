/**
 * @file          /ks-scmc-gui/src/pages/container/security-configuration/start-stop-control-tab.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "start-stop-control-tab.h"
#include <QVBoxLayout>
StartStopControlTab::StartStopControlTab(QWidget *parent) : QWidget(parent), m_checkBox(nullptr)
{
    initUI();
}

void StartStopControlTab::setStartStopInfo(bool isRestrict)
{
    m_checkBox->setChecked(isRestrict);
}

bool StartStopControlTab::getStartStopInfo()
{
    return m_checkBox->isChecked();
}

void StartStopControlTab::initUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setContentsMargins(20, 20, 10, 0);
    layout->setSpacing(0);

    m_checkBox = new QCheckBox(this);
    m_checkBox->setObjectName("startStopCtlCheckBox");
    m_checkBox->setChecked(false);
    m_checkBox->setText(tr("Forbid operating this container from command line"));
    m_checkBox->setStyleSheet("#startStopCtlCheckBox::indicator:checked{color:#2eb3ff;}");

    layout->addWidget(m_checkBox);
    layout->addStretch();
}
