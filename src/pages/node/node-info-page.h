/**
 * @file          /ks-scmc-gui/src/pages/node/node-info-page.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef NODEINFOPAGE_H
#define NODEINFOPAGE_H

#include "tab-page.h"

enum NodeInfoSubPageType
{
    NODE_INFO_SUB_PAGE_TYPE_CONTAINER,
    NODE_INFO_SUB_PAGE_TYPE_MONITOR,
    NODE_INFO_SUB_PAGE_TYPE_NETWORK,
    NODE_INFO_SUB_PAGE_TYPE_CONFIG
};

class MonitorContent;
class ContainerListPage;
class NetworkPageManager;
class NodeConfigPage;
class NodeInfoPage : public TabPage
{
    Q_OBJECT
public:
    explicit NodeInfoPage(QWidget *parent = nullptr);
    void setNodeId(qint64 nodeId);
    void updateInfo(QString keyword = "");  //刷新表格

private:
    void createSubPage(NodeInfoSubPageType type);
    void updatePageInfo(int index);

private:
    qint64 m_nodeId = -1;
    ContainerListPage *m_containerListPage;
    MonitorContent *m_monitor;
    NetworkPageManager *m_networkPage;
    NodeConfigPage *m_configPage;
};

#endif  // NODEINFOPAGE_H
