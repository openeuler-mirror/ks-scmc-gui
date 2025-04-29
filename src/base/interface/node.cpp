/**
 * @file          src/base/interface/node.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "node.h"
#include <QMutexLocker>
#include <QUuid>
#include "common.h"

Node::Node(QObject *parent) : QObject(parent)
{
    QMutexLocker locker(&mutex);
}

Node::~Node()
{
}

QString Node::generateId(QObject *callObj)
{
    QString uid = QUuid::createUuid().toString();
    return QString("%1_%2").arg(callObj->metaObject()->className()).arg(uid);
}

void Node::listNode(const QString objId)
{
    node::ListRequest req;
    RPC_ASYNC(node::ListReply, _listNode, listNodeFinished, objId, req);
}

void Node::createNode(const QString objId, const node::CreateRequest &req)
{
    RPC_ASYNC(node::CreateReply, _createNode, createNodeFinished, objId, req);
}

void Node::removeNode(const QString objId, const std::vector<int64_t> &node_ids)
{
    node::RemoveRequest req;
    for (auto &id : node_ids)
    {
        req.add_ids(id);
    }

    RPC_ASYNC(node::RemoveReply, _removeNode, removeNodeFinished, objId, req);
}

void Node::nodeStatus(const QString objId, const std::vector<int64_t> &node_ids)
{
    node::StatusRequest req;
    for (auto &id : node_ids)
    {
        req.add_node_ids(id);
    }

    RPC_ASYNC(node::StatusReply, _nodeStatus, statusNodeFinished, objId, req);
}

void Node::updateNode(const QString objId, const node::UpdateRequest &req)
{
    RPC_ASYNC(node::UpdateReply, _updateNode, updateNodeFinished, objId, req);
}

QPair<grpc::Status, node::ListReply> Node::_listNode(const node::ListRequest &req)
{
    RPC_IMPL(node::ListReply, node::Node::NewStub, List);
}

QPair<grpc::Status, node::CreateReply> Node::_createNode(const node::CreateRequest &req)
{
    RPC_IMPL(node::CreateReply, node::Node::NewStub, Create);
}

QPair<grpc::Status, node::RemoveReply> Node::_removeNode(const node::RemoveRequest &req)
{
    RPC_IMPL(node::RemoveReply, node::Node::NewStub, Remove);
}

QPair<grpc::Status, node::StatusReply> Node::_nodeStatus(const node::StatusRequest &req)
{
    RPC_IMPL(node::StatusReply, node::Node::NewStub, Status);
}

QPair<grpc::Status, node::UpdateReply> Node::_updateNode(const node::UpdateRequest &req)
{
    RPC_IMPL(node::UpdateReply, node::Node::NewStub, Update);
}