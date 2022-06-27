/**
 * @file          /ks-scmc-gui/src/pages/audit/audit-list/audit-list-page.h
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef AUDITLISTPAGE_H
#define AUDITLISTPAGE_H

#include "tab-page.h"
#include "image/image-list-page.h"

enum AuditListPageType
{
    WAIT_FOR_APPROVE,
    AUDIT_PASSED,
    AUDIT_REFUSED
};

class AuditListPage : public TabPage
{
    Q_OBJECT
public:
    explicit AuditListPage(QWidget *parent = nullptr);
    void setNodeId(qint64 nodeId);
    void updateInfo(QString keyword = "");  //刷新表格

private:
    void initUI();
    void createSubPage(AuditListPageType type);
    void updatePageInfo(int index);

private:
    void updateAuditInfo();

private:
    ImageListPage *m_imagelist_approve;
    ImageListPage *m_imagelist_passed;
    ImageListPage *m_imagelist_rejected;

signals:
    void sigUpdateTipSums();
};

#endif // AUDITLISTPAGE_H
