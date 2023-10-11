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
};

#endif  // CONTAINERINFOPAGE_H