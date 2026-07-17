/**
 * @file          src/pages/node/security-configuration/file-protection-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "file-protection-page.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "common/notification-manager.h"
#include "common/security-widgets/security-list-item.h"
#include "common/security-widgets/security-list.h"

FileProtectionPage::FileProtectionPage(QWidget *parent) : Page(parent),
                                                          m_nodeID(-1),
                                                          m_btnOpen(nullptr),
                                                          m_btnClose(nullptr),
                                                          m_fileList(nullptr),
                                                          m_protectEnabled(false)
{
    m_objId = Node::generateId(this);
    initUI();

    connect(&Node::getInstance(), &Node::getFileProtectFinished, this, &FileProtectionPage::getFileProtectFinished);
    connect(&Node::getInstance(), &Node::updateFileProtectFinished, this, &FileProtectionPage::getUpdateFileProtectFinished);
}

void FileProtectionPage::updateInfo(QString keyword)
{
    m_fileList->clearItems();
    Node::getInstance().getFileProtect(m_objId, m_nodeID);
}

void FileProtectionPage::setNodeId(qint64 nodeID)
{
    m_nodeID = nodeID;
}

void FileProtectionPage::save()
{
    node::UpdateFileProtectRequest req;
    req.set_node_id(m_nodeID);

    auto securityCfg = req.mutable_security_config();

    auto fileProtect = securityCfg->mutable_file_protection();
    fileProtect->set_is_on(m_protectEnabled);

    QStringList fileList;
    if (!m_fileList->getSecurityInfos(fileList))
    {
        NotificationManager::sendNotify(tr("Failed to save the configuration"), tr("Please check the input parameters."));
        return;
    }

    for (auto file : fileList)
    {
        fileProtect->add_file_list(file.toStdString());
    }

    KLOG_DEBUG() << "Set node:" << m_nodeID << "protect file to backend: "
                 << "enable:" << m_protectEnabled << "files:" << fileList;
    Node::getInstance().updateFileProtect(m_objId, req);
}

void FileProtectionPage::setProtectEnabled(bool enabled)
{
    m_protectEnabled = enabled;
    m_fileList->setItemsEnable(enabled);
}

void FileProtectionPage::getUpdateFileProtectFinished(const QString objId, const QPair<grpc::Status, node::UpdateFileProtectReply> &reply)
{
    if (m_objId != objId)
        return;

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Failed to update node:" << m_nodeID << "file protect list:" << reply.first.error_message().data();
        return;
    }

    NotificationManager::sendNotify(tr("Successful to update node protected files!"), "");
}

void FileProtectionPage::getFileProtectFinished(const QString objId, const QPair<grpc::Status, node::GetFileProtectReply> &reply)
{
    if (m_objId != objId)
        return;

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Failed to get node:" << m_nodeID << "file protect list:" << reply.first.error_message().data();
        return;
    }

    if (!reply.second.has_file_protection())
    {
        KLOG_WARNING() << "Failed to get node:" << m_nodeID << "file protect list:"
                       << "file_protection not found";
        return;
    }

    auto fileProtect = reply.second.file_protection();

    if (fileProtect.is_on())
        m_btnOpen->setChecked(true);
    else
        m_btnClose->setChecked(true);

    QStringList fileList;
    for (auto file : fileProtect.file_list())
    {
        fileList << file.data();
    }

    KLOG_DEBUG() << "Get node:" << m_nodeID << "protect file from backend: "
                 << "enable:" << fileProtect.is_on() << "files:" << fileList;
    m_fileList->setSecurityInfos(fileList);
    setProtectEnabled(fileProtect.is_on());
}

void FileProtectionPage::initUI()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 状态按钮
    auto statusLayout = new QHBoxLayout();
    statusLayout->setSpacing(20);
    auto label = new QLabel(tr("Open Status"));
    m_btnOpen = new QRadioButton(tr("Open"));
    m_btnClose = new QRadioButton(tr("Close"));
    // 默认关闭
    m_btnClose->setChecked(true);

    statusLayout->addWidget(label);
    statusLayout->addWidget(m_btnOpen);
    statusLayout->addWidget(m_btnClose);
    statusLayout->addStretch();

    // 文件保护列表
    m_fileList = new SecurityList(tr("Key file"), this);

    // 保存按钮
    auto btnLayout = new QHBoxLayout();
    auto btnSave = new QPushButton(tr("Save"));
    btnSave->setFixedSize(78, 32);
    Kiran::WidgetPropertyHelper::setButtonType(btnSave, Kiran::BUTTON_Default);

    btnLayout->addStretch();
    btnLayout->addWidget(btnSave);
    btnLayout->addStretch();

    mainLayout->addLayout(statusLayout);
    mainLayout->addWidget(m_fileList);
    mainLayout->addLayout(btnLayout);

    setProtectEnabled(false);

    connect(btnSave, &QPushButton::clicked, this, &FileProtectionPage::save);
    connect(m_btnOpen, &QRadioButton::clicked, this, [this]()
            { setProtectEnabled(true); });
    connect(m_btnClose, &QRadioButton::clicked, this, [this]()
            { setProtectEnabled(false); });
}
