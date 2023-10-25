/**
 * @file          /ks-scmc-gui/src/pages/container/container-manager/container-info-page.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef CONTAINERINFOPAGE_H
#define CONTAINERINFOPAGE_H
#include "tab-page.h"

enum ContainerInfoSubPageType
{
    CONTAINER_INFO_SUB_PAGE_TYPE_OUTLINE,
    CONTAINER_INFO_SUB_PAGE_TYPE_BACKUP,
    CONTAINER_INFO_SUB_PAGE_TYPE_MONITOR,
    CONTAINER_INFO_SUB_PAGE_TYPE_TASK
};

class MonitorContent;
class ContainerBackupPage;
class ContainerInfoPage : public TabPage
{
    Q_OBJECT
public:
    explicit ContainerInfoPage(QWidget *parent = nullptr);
    void setContainerInfo(QMap<QString, QVariant> infoMap);
    void updateInfo(QString keyword = "");  //刷新表格

private:
    void createSubPage(ContainerInfoSubPageType type);
    void updatePageInfo(int index);

private:
    QMap<QString, QVariant> m_infoMap;
    MonitorContent *m_monitor;
    ContainerBackupPage *m_containerBackup;
};

#endif  // CONTAINERINFOPAGE_H
