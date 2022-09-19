/**
 * @file          /ks-scmc-gui/src/pages/image/image-approval-page.h
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef IMAGE_APPROVE_PAGE_H
#define IMAGE_APPROVE_PAGE_H

#include "image/image-list-page.h"
#include "tab-page.h"

enum ImageApprovalType
{
    WAIT_FOR_APPROVE,
    APPROVE_PASSED,
    APPROVE_REFUSED
};

class ImageApprovalPage : public TabPage
{
    Q_OBJECT
public:
    explicit ImageApprovalPage(QWidget *parent = nullptr);
    void setNodeId(qint64 nodeId);
    void updateInfo(QString keyword = "");  //刷新表格

private:
    void initUI();
    void createSubPage(ImageApprovalType type);
    void updatePageInfo(int index);

private:
    void updateApprovalInfo();

private:
    ImageListPage *m_imagelist_approve;
    ImageListPage *m_imagelist_passed;
    ImageListPage *m_imagelist_rejected;

signals:
    void sigUpdateTipSumsProxy(int tips);
};

#endif  // IMAGE_APPROVE_PAGE_H
