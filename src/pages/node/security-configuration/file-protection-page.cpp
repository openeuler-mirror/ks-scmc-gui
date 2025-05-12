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
    bool flag = false;

    node::UpdateFileProtectRequest req;
    req.set_node_id(m_nodeID);

    auto securityCfg = req.mutable_security_config();

    auto fileProtect = securityCfg->mutable_file_protection();
    fileProtect->set_is_on(m_protectEnabled);

    auto fileList = m_fileList->getSecurityInfos();
    for (auto file : fileList)
    {
        KLOG_DEBUG() << "Set node protect file " << file << "to backend.";
        fileProtect->add_file_list(file.toStdString());
    }
    Node::getInstance().updateFileProtect(m_objId, req);
}

void FileProtectionPage::setProtectEnabled(bool enabled)
{
    m_protectEnabled = enabled;
    m_fileList->setEnabled(enabled);
}

void FileProtectionPage::getUpdateFileProtectFinished(const QString objId, const QPair<grpc::Status, node::UpdateFileProtectReply> &reply)
{
    if (m_objId != objId)
        return;

    if (!reply.first.ok())
    {
        KLOG_INFO() << "Failed to update node:" << m_nodeID << "file protect list:" << reply.first.error_message().data();
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
        KLOG_INFO() << "Failed to get node:" << m_nodeID << "file protect list:" << reply.first.error_message().data();
        return;
    }

    auto fileProtect = reply.second.file_protection();

    if (fileProtect.is_on())
        m_btnOpen->setChecked(true);
    else
        m_btnClose->setChecked(true);

    // int count = 0;
    // for (auto file : fileProtect.file_list())
    // {
    //     if (count > 0)
    //         createItem(count);

    //     auto listItem = m_fileList->item(count);
    //     SecurityListItem *item = qobject_cast<SecurityListItem *>(m_fileList->itemWidget(listItem));
    //     item->setInfo(QString::fromStdString(file));
    //     count++;
    // }
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

    connect(btnSave, &QPushButton::clicked, this, &FileProtectionPage::save);
    connect(m_btnOpen, &QRadioButton::toggled, this, [this]()
            { setProtectEnabled(true); });
    connect(m_btnClose, &QRadioButton::toggled, this, [this]()
            { setProtectEnabled(false); });

    // 默认关闭
    m_btnClose->setChecked(true);
}
