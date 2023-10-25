/**
 * @file          /ks-scmc-gui/src/pages/node/node-page-manager.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef NODEPAGEMANAGER_H
#define NODEPAGEMANAGER_H

#include "stacked-page.h"

enum NodePageType
{
    NODE_PAGE_TYPE_NODE_LIST,
    NODE_PAGE_TYPE_NODE_INFO
};

class NodeInfoPage;
class NodePageManager : public StackedPage
{
    Q_OBJECT
public:
    explicit NodePageManager(QWidget *parent = nullptr);
    void createStackedPage(NodePageType type, QString key);
    virtual void updateInfo(QString keyword = "");

private slots:
    void onNodeNameClicked(qint64 nodeId);

private:
    NodeInfoPage *m_nodeInfoPage;
};

#endif  // NODEPAGEMANAGER_H
