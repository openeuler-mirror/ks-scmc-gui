/**
 * @file          src/pages/node/security-configuration/file-protection-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "file-protection-page.h"
#include <widget-property-helper.h>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
FileProtectionPage::FileProtectionPage(QWidget* parent) : Page(parent),
                                                          m_fileList(nullptr),
                                                          m_protectEnabled(false)
{
    initUI();
}

void FileProtectionPage::updateInfo(QString keyword)
{
}

void FileProtectionPage::save()
{
}

void FileProtectionPage::setProtectEnabled(bool enabled)
{
    m_protectEnabled = enabled;
    m_fileList->setEnabled(enabled);
}

void FileProtectionPage::getUpdateFileProtectFinished(const QString objId, const QPair<grpc::Status, node::UpdateFileProtectReply>& reply)
{
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
    auto btnOpen = new QRadioButton(tr("Open"));
    auto btnClose = new QRadioButton(tr("Close"));
    btnClose->setChecked(true);

    statusLayout->addWidget(label);
    statusLayout->addWidget(btnOpen);
    statusLayout->addWidget(btnClose);
    statusLayout->addStretch();

    // 文件保护列表
    m_fileList = new QListWidget(this);
    m_fileList->setFocusPolicy(Qt::NoFocus);
    m_fileList->setFrameShape(QFrame::NoFrame);
    m_fileList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_fileList->setEnabled(false);

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
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    connect(btnSave, &QPushButton::clicked, this, &FileProtectionPage::save);
    connect(btnOpen, &QRadioButton::toggled, this, [this]()
            { setProtectEnabled(true); });
    connect(btnClose, &QRadioButton::toggled, this, [this]()
            { setProtectEnabled(false); });
}
