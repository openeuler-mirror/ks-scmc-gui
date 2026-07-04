#include "container-info-page.h"
#include <kiran-log/qt5-log-i.h>
#include "container-backup-page.h"
#include "monitor-content.h"
ContainerInfoPage::ContainerInfoPage(QWidget *parent) : TabPage(parent), m_monitor(nullptr), m_containerBackup(nullptr)
{
    createSubPage(CONTAINER_INFO_SUB_PAGE_TYPE_MONITOR);
    createSubPage(CONTAINER_INFO_SUB_PAGE_TYPE_BACKUP);
}

void ContainerInfoPage::setContainerInfo(QMap<QString, QVariant> infoMap)
{
    m_infoMap = infoMap;
}

void ContainerInfoPage::updateInfo(QString keyword)
{
    KLOG_INFO() << "ContainerInfoPage UpdateInfo";
    m_monitor->updateMonitorInfo(m_infoMap.value(NODE_ID, -1).toInt(), m_infoMap.value(CONTAINER_ID, "").toString().toStdString());
    m_containerBackup->updateBackupList(m_infoMap.value(NODE_ID, -1).toInt(),
                                        m_infoMap.value(CONTAINER_ID, "").toString().toStdString(),
                                        m_infoMap.value(CONTAINER_STATUS, "").toString());
}

void ContainerInfoPage::createSubPage(ContainerInfoSubPageType type)
{
    switch (type)
    {
    case CONTAINER_INFO_SUB_PAGE_TYPE_MONITOR:
        m_monitor = new MonitorContent(this);
        addTabPage(m_monitor, tr("Monitor"));
        break;
    case CONTAINER_INFO_SUB_PAGE_TYPE_BACKUP:
        m_containerBackup = new ContainerBackupPage(this);
        addTabPage(m_containerBackup, tr("Backup"));
        break;
    default:
        break;
    }
}

void ContainerInfoPage::updatePageInfo(int index)
{
    if (index == CONTAINER_INFO_SUB_PAGE_TYPE_MONITOR)
        m_monitor->updateMonitorInfo(m_infoMap.value(NODE_ID).toInt(), m_infoMap.value(CONTAINER_ID).toString().toStdString());
}
