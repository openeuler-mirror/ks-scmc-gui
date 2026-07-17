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

class NetworkAccessList;
class SecurityList;
class NetworkConfigPage : public Page
{
    Q_OBJECT
public:
    explicit NetworkConfigPage(QWidget *parent = nullptr);
    void updateInfo(QString keyword = "") override;
    void setNodeId(qint64 nodeID);

private slots:
    void save();
    void setConfigEnabled(QAbstractButton *btn);

    // 网络安全配置
    void getNetworkRuleFinished(const QString objId, const QPair<grpc::Status, node::GetNetworkRuleReply> &);
    void updateNetworkRuleFinished(const QString objId, const QPair<grpc::Status, node::UpdateNetworkRuleReply> &);

private:
    void initUI();
    void initConnect();
    void getDefaultPorts();

private:
    Ui::NetworkConfigPage *ui;
    QString m_objId;
    qint64 m_nodeID;
    NetworkAccessList *m_accessList;
    SecurityList *m_processList;
    bool m_accessCtrlEnabled;
    bool m_whiteListEnabled;
    QList<int> m_defaultPorts;
};