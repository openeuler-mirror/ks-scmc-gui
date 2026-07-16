/**
 * @file          src/pages/image/security-switch-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "security-switch-page.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-switch-button.h>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include "notification-manager.h"
SecuritySwitchPage::SecuritySwitchPage(QWidget *parent) : Page(parent), m_checkbox(nullptr)
{
    m_objId = System::generateId(this);
    initUI();
    connect(&System::getInstance(), &System::getSecuritySwitchFinished, this, &SecuritySwitchPage::getSecuritySwitchResult);
    connect(&System::getInstance(), &System::setSecuritySwitchFinished, this, &SecuritySwitchPage::setSecuritySwitchResult);
}

void SecuritySwitchPage::updateInfo(QString keyword)
{
    System::getInstance().getSecuritySwitch(m_objId);
}

void SecuritySwitchPage::getSecuritySwitchResult(const QString objId, const QPair<grpc::Status, sys::GetSecuritySwitchReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        m_checkbox->setChecked(reply.second.is_on());
    }
    else
    {
        KLOG_WARNING() << "Get security switch result failed!" << reply.first.error_message().data();
        NotificationManager::sendNotify(tr("Get security switch result failed!"),
                                        QString::fromStdString(reply.first.error_message()));
    }
}

void SecuritySwitchPage::setSecuritySwitchResult(const QString objId, const QPair<grpc::Status, sys::SetSecuritySwitchReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        NotificationManager::sendNotify(tr("Set security switch %1").arg(reply.second.is_on() ? tr("on") : tr("off")),
                                        "");
    }
    else
    {
        KLOG_WARNING() << "Set security switch failed!" << reply.first.error_message().data();
        NotificationManager::sendNotify(tr("Set security switch failed!"),
                                        QString::fromStdString(reply.first.error_message()));
        m_checkbox->setChecked(!m_checkbox->isChecked());
    }
}

void SecuritySwitchPage::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(20, 0, 20, 0);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(10);
    QLabel *label = new QLabel(this);
    label->setText(tr("Enable image approval,and verity signature files"));

    m_checkbox = new KiranSwitchButton(this);
    m_checkbox->setObjectName("cb_global_security");
    m_checkbox->setFocusPolicy(Qt::NoFocus);
    m_checkbox->setChecked(true);

    layout->addWidget(label);
    layout->addWidget(m_checkbox);
    layout->addStretch();

    mainLayout->addLayout(layout);
    mainLayout->addStretch();

    connect(m_checkbox, &QCheckBox::clicked, this, &SecuritySwitchPage::setStatus);
}

void SecuritySwitchPage::setStatus(bool isEnable)
{
    InfoWorker::getInstance().setSecuritySwitch(m_objId, isEnable);
}
