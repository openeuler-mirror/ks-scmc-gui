/**
 * @file          /ks-scmc-gui/src/pages/image/image-approval-page.cpp
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "image-approval-page.h"
#include <kiran-log/qt5-log-i.h>

ImageApprovalPage::ImageApprovalPage(QWidget *parent) : TabPage(parent), m_imagelist_approve(nullptr), m_imagelist_passed(nullptr), m_imagelist_rejected(nullptr)
{
    createSubPage(WAIT_FOR_APPROVE);
    createSubPage(APPROVE_PASSED);
    createSubPage(APPROVE_REFUSED);
    setCurrentPage(WAIT_FOR_APPROVE);
    //    updateInfo();
    connect(this, &ImageApprovalPage::sigTabBarClicked, this, &ImageApprovalPage::updatePageInfo);
}

void ImageApprovalPage::updateInfo(QString keyword)
{
    KLOG_INFO() << "ImageApprovalPage UpdateInfo";
    setCurrentPage(WAIT_FOR_APPROVE);
    m_imagelist_approve->updateInfo();
    //    m_imagelist_passed->updateInfo();
    //    m_imagelist_rejected->updateInfo();
}

void ImageApprovalPage::createSubPage(ImageApprovalType type)
{
    switch (type)
    {
    case WAIT_FOR_APPROVE:
    {
        m_imagelist_approve = new ImageListPage(this, true);
        //        m_imagelist_approve->setCheckBox(false);
        m_imagelist_approve->setDelRow(tr("Rejected"), tr("Passed"));
        //        m_imagelist_approve->setIsInitAuditButtons(true);
        m_imagelist_approve->setOperateAreaVisible(true);
        addTabPage(m_imagelist_approve, tr("Wait for Approve"));
        connect(m_imagelist_approve, &ImageListPage::sigUpdateAuditInfo, this, &ImageApprovalPage::updateApprovalInfo);
        connect(m_imagelist_approve, &ImageListPage::sigUpdateAPproveTipSums, this, &ImageApprovalPage::sigUpdateTipSumsProxy);
        break;
    }
    case APPROVE_PASSED:
    {
        m_imagelist_passed = new ImageListPage(this);
        m_imagelist_passed->setCheckBox(false);
        m_imagelist_passed->setDelRow(tr("Wait for Approve"), tr("Rejected"));
        m_imagelist_passed->setOperateAreaVisible(false);
        addTabPage(m_imagelist_passed, tr("Passed"));
        break;
    }
    case APPROVE_REFUSED:
    {
        m_imagelist_rejected = new ImageListPage(this);
        m_imagelist_rejected->setCheckBox(false);
        m_imagelist_rejected->setDelRow(tr("Wait for Approve"), tr("Passed"));
        m_imagelist_rejected->setOperateAreaVisible(false);
        addTabPage(m_imagelist_rejected, tr("Rejected"));
        break;
    }
    default:
        break;
    }
}

void ImageApprovalPage::updatePageInfo(int index)
{
    if (index == WAIT_FOR_APPROVE)
    {
        m_imagelist_approve->updateInfo();
    }
    else if (index == APPROVE_PASSED)
    {
        m_imagelist_passed->updateInfo();
    }
    else if (index == APPROVE_REFUSED)
    {
        m_imagelist_rejected->updateInfo();
    }
}

void ImageApprovalPage::updateApprovalInfo()
{
    updateInfo();
}
