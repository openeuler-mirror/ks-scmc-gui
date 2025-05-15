/**
 * @file          src/base/interface/node.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma once

#include <QMutex>
#include <QObject>
#include <QPair>

#include <grpcpp/grpcpp.h>

#include "rpc.h"

class Node : public QObject
{
    Q_OBJECT
public:
    static Node &getInstance()
    {
        static Node s_Node;
        return s_Node;
    }

    static QString generateId(QObject *callObj);

    // node management
    void listNode(const QString objId);
    void createNode(const QString objId, const node::CreateRequest &);
    void removeNode(const QString objId, const std::vector<int64_t> &node_ids);
    void nodeStatus(const QString objId, const std::vector<int64_t> &node_ids);
    void updateNode(const QString objId, const node::UpdateRequest &);

    // 文件保护
    void getFileProtect(const QString objId, qint64 nodeID);
    void updateFileProtect(const QString objId, const node::UpdateFileProtectRequest &);

    // 网络安全配置
    QPair<grpc::Status, node::GetNodeDefaultPortReply> getNodeDefaultPort(qint64 nodeID);
    void getNetworkRule(const QString objId, qint64 nodeID);
    void updateNetworkRule(const QString objId, const node::UpdateNetworkRuleRequest &);

    void getNetworkProcessWhiteList(const QString objId, qint64 nodeID);
    void updateNetworkProcessWhiteList(const QString objId, const node::UpdateNetworkProcessWhiteListRequest &);

private:
    Node(QObject *parent = nullptr);
    ~Node();

    // node management
    static QPair<grpc::Status, node::ListReply> _listNode(const node::ListRequest &);
    static QPair<grpc::Status, node::CreateReply> _createNode(const node::CreateRequest &);
    static QPair<grpc::Status, node::RemoveReply> _removeNode(const node::RemoveRequest &);
    static QPair<grpc::Status, node::StatusReply> _nodeStatus(const node::StatusRequest &);
    static QPair<grpc::Status, node::UpdateReply> _updateNode(const node::UpdateRequest &);

    // 文件保护
    static QPair<grpc::Status, node::GetFileProtectReply> _getFileProtect(const node::GetFileProtectRequest &);
    static QPair<grpc::Status, node::UpdateFileProtectReply> _updateFileProtect(const node::UpdateFileProtectRequest &);

    // 网络安全配置
    static QPair<grpc::Status, node::GetNetworkRuleReply> _getNetworkRule(const node::GetNetworkRuleRequest &);
    static QPair<grpc::Status, node::UpdateNetworkRuleReply> _updateNetworkRule(const node::UpdateNetworkRuleRequest &);

signals:
    // node management
    void listNodeFinished(QString objId, const QPair<grpc::Status, node::ListReply> &);
    void createNodeFinished(const QString objId, const QPair<grpc::Status, node::CreateReply> &);
    void removeNodeFinished(const QString objId, const QPair<grpc::Status, node::RemoveReply> &);
    void statusNodeFinished(const QString objId, const QPair<grpc::Status, node::StatusReply> &);
    void updateNodeFinished(const QString objId, const QPair<grpc::Status, node::UpdateReply> &);

    // 文件保护
    void getFileProtectFinished(const QString objId, const QPair<grpc::Status, node::GetFileProtectReply> &);
    void updateFileProtectFinished(const QString objId, const QPair<grpc::Status, node::UpdateFileProtectReply> &);

    // 网络安全配置
    void getNetworkRuleFinished(const QString objId, const QPair<grpc::Status, node::GetNetworkRuleReply> &);
    void updateNetworkRuleFinished(const QString objId, const QPair<grpc::Status, node::UpdateNetworkRuleReply> &);

private:
    QMutex mutex;
};
