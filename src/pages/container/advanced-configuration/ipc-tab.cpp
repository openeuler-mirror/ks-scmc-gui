/**
 * @file          src/pages/container/advanced-configuration/IPC.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "ipc-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>

IPCTab::IPCTab(QWidget* parent) : QWidget(parent),
                                  m_combobox(nullptr),
                                  m_nodeID(-1),
                                  m_currentIPC("")
{
    initUI();
}

void IPCTab::update(qint64 nodeID)
{
    KLOG_DEBUG() << "update ipc list of node:" << nodeID;
    m_nodeID = nodeID;
    auto reply = IPC::getInstance().syncListIPC(m_nodeID);
    handleIPCList(reply);
}

QString IPCTab::getIPC()
{
    auto name = m_combobox->currentText();

    KLOG_DEBUG() << "Get ipc from UI: " << name;
    return name == tr("None") ? "" : name;
}

void IPCTab::setIPC(qint64 nodeID, QString ipc)
{
    if (ipc.isEmpty())
        ipc = tr("None");

    KLOG_DEBUG() << "Set ipc to UI: " << ipc;
    m_currentIPC = ipc;

    if (m_combobox->findText(ipc) == -1)
    {
        KLOG_WARNING() << "Failed to find IPC" << ipc << "in combobox list";
        // 找不到，则重新获取该节点下的ipc列表；
        update(nodeID);
        return;
    }
    m_combobox->setCurrentText(ipc);
}

void IPCTab::handleIPCList(const QPair<grpc::Status, ipcbase::IpcListReply>& reply)
{
    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Failed to get node" << m_nodeID << "IPC list: " << reply.first.error_message().data();
        return;
    }

    m_combobox->clear();
    m_combobox->addItem(tr("None"));

    for (auto info : reply.second.ipcname_ids())
    {
        auto ipc = QString::fromStdString(info.name());
        m_combobox->addItem(ipc);
        KLOG_DEBUG() << "Get ipc list from backend: " << ipc;
    }

    if (!m_currentIPC.isEmpty())
    {
        m_combobox->setCurrentText(m_currentIPC);
    }
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
