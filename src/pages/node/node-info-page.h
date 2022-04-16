#ifndef NODEINFOPAGE_H
#define NODEINFOPAGE_H

#include "tab-page.h"

enum NodeInfoSubPageType
{
    NODE_INFO_SUB_PAGE_TYPE_CONTAINER,
    NODE_INFO_SUB_PAGE_TYPE_MONITOR
};

class MonitorDialog;
class ContainerListPage;
class NodeInfoPage : public TabPage
{
    Q_OBJECT
public:
    explicit NodeInfoPage(QWidget *parent = nullptr);
    void setNodeId(qint64 nodeId);
    void updateInfo(QString keyword = "");  //刷新表格

private:
    void initUI();
    void createSubPage(NodeInfoSubPageType type);

private:
    qint64 m_nodeId = -1;
    MonitorDialog *m_monitor;
    ContainerListPage *m_containerListPage;
};

#endif  // NODEINFOPAGE_H
