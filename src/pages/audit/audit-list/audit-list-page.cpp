#include "audit-list-page.h"
#include <kiran-log/qt5-log-i.h>

AuditListPage::AuditListPage(QWidget *parent) : TabPage(parent) , m_imagelist_approve(nullptr)
{
    createSubPage(WAIT_FOR_APPROVE);
    createSubPage(AUDIT_PASSED);
    createSubPage(AUDIT_REFUSED);
    connect(this, &AuditListPage::sigTabBarClicked, this, &AuditListPage::updatePageInfo);
}

void AuditListPage::updateInfo(QString keyword)
{
    KLOG_INFO() << "NodeInfoPage UpdateInfo";
    m_imagelist_approve->updateInfo(keyword);
    m_imagelist_passed->updateInfo(keyword);
    m_imagelist_rejected->updateInfo(keyword);
}

void AuditListPage::createSubPage(AuditListPageType type)
{
    switch (type)
    {
    case WAIT_FOR_APPROVE:
    {
        m_imagelist_approve = new ImageListPage(this);
        m_imagelist_approve->setDelRow(tr("Rejected"),tr("Passed"));
        m_imagelist_approve->setOperateAreaVisible(false);
        addTabPage(m_imagelist_approve,tr("Wait for Approve"));
        break;
    }
    case AUDIT_PASSED:
    {
        m_imagelist_passed = new ImageListPage(this);
        m_imagelist_passed->setDelRow(tr("Wait for Approve"),tr("Rejected"));
        m_imagelist_passed->setOperateAreaVisible(false);
        addTabPage(m_imagelist_passed,tr("Passed"));
        break;
    }
    case AUDIT_REFUSED:
    {
        m_imagelist_rejected = new ImageListPage(this);
        m_imagelist_rejected->setDelRow(tr("Wait for Approve"),tr("Passed"));
        m_imagelist_rejected->setOperateAreaVisible(false);
        addTabPage(m_imagelist_rejected,tr("Rejected"));
        break;
    }
    default:
        break;
    }
}

void AuditListPage::updatePageInfo(int index)
{
    if (index == WAIT_FOR_APPROVE)
    {
        m_imagelist_approve->updateInfo();
    }
    else if (index == AUDIT_PASSED)
    {
        m_imagelist_passed->updateInfo();
    }
    else if (index == AUDIT_REFUSED)
    {
        m_imagelist_rejected->updateInfo();
    }
}
