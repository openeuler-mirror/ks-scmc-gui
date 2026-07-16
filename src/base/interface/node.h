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
    void UpdateFileProtect(const QString objId, const node::UpdateFileProtectRequest &);

private:
    Node(QObject *parent = nullptr);
    ~Node();

    // node management
    static QPair<grpc::Status, node::ListReply> _listNode(const node::ListRequest &);
    static QPair<grpc::Status, node::CreateReply> _createNode(const node::CreateRequest &);
    static QPair<grpc::Status, node::RemoveReply> _removeNode(const node::RemoveRequest &);
    static QPair<grpc::Status, node::StatusReply> _nodeStatus(const node::StatusRequest &);
    static QPair<grpc::Status, node::UpdateReply> _updateNode(const node::UpdateRequest &);
    static QPair<grpc::Status, node::UpdateFileProtectReply> _updateFileProtect(const node::UpdateFileProtectRequest &);

signals:
    // node management
    void listNodeFinished(QString objId, const QPair<grpc::Status, node::ListReply> &);
    void createNodeFinished(const QString objId, const QPair<grpc::Status, node::CreateReply> &);
    void removeNodeFinished(const QString objId, const QPair<grpc::Status, node::RemoveReply> &);
    void statusNodeFinished(const QString objId, const QPair<grpc::Status, node::StatusReply> &);
    void updateNodeFinished(const QString objId, const QPair<grpc::Status, node::UpdateReply> &);
    void updateFileProtectFinished(const QString objId, const QPair<grpc::Status, node::UpdateFileProtectReply> &);

private:
    QMutex mutex;
};
