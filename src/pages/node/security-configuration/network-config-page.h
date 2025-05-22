/**
 * @file          src/pages/node/security-configuration/network-access-ctl-page.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma once

#include <QAbstractButton>
#include <QListWidget>
#include "base/interface/node.h"
#include "page.h"

namespace Ui
{
class NetworkConfigPage;
}

enum ButtonType
{
    BUTTON_TYPE_ACCESS_OPEN,
    BUTTON_TYPE_ACCESS_CLOSE,
    BUTTON_TYPE_WHITE_LIST_OPEN,
    BUTTON_TYPE_WHITE_LIST_CLOSE
};

class NetworkAccessList;
class SecurityList;
class NetworkConfigPage : public Page
{
    Q_OBJECT

public:
    explicit NetworkConfigPage(QWidget *parent = nullptr);
    ~NetworkConfigPage();
    void updateInfo(QString keyword = "") override;
    void setNodeId(qint64 nodeID);

private slots:
    void save();
    void setConfigEnabled(int id);

    // 网络安全配置
    void getNetworkRuleFinished(const QString objId, const QPair<grpc::Status, node::GetNetworkRuleReply> &);
    void updateNetworkRuleFinished(const QString objId, const QPair<grpc::Status, node::UpdateNetworkRuleReply> &);

    void getNetworkProcessWhiteListFinished(const QString objId, const QPair<grpc::Status, node::GetNetworkProcessWhiteListReply> &);
    void updateNetworkProcessWhiteListFinished(const QString objId, const QPair<grpc::Status, node::UpdateNetworkProcessWhiteListReply> &);

private:
    void initUI();
    void initConnect();
    void getDefaultPorts();
    void saveNetworkRules();
    void saveNetworkProcessWhiteList();

private:
    Ui::NetworkConfigPage *ui;
    QString m_objId;
    qint64 m_nodeID;
    NetworkAccessList *m_accessList;
    SecurityList *m_processList;
    bool m_accessCtrlEnabled;
    bool m_whiteListEnabled;
    QList<int> m_defaultPorts;
    QMap<int, bool> m_btnMap;
};