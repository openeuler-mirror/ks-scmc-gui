/**
 * @file          src/base/interface/network.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "network.h"
#include "common.h"

#include <kiran-log/qt5-log-i.h>

#include <QMutexLocker>
#include <QUuid>

Network::Network(QObject *parent) : QObject(parent)
{
    QMutexLocker locker(&mutex);
}

Network::~Network()
{
}

QString Network::generateId(QObject *callObj)
{
    QString uid = QUuid::createUuid().toString();
    return QString("%1_%2").arg(callObj->metaObject()->className()).arg(uid);
}

void Network::listNetwork(const QString objId, const int64_t node_id)
{
    network::ListRequest req;
    req.set_node_id(node_id);
    RPC_ASYNC(network::ListReply, _listNetwork, listNetworkFinished, objId, req);
}

void Network::connectNetwork(const QString objId, const network::ConnectRequest &req)
{
    RPC_ASYNC(network::ConnectReply, _connectNetwork, connectNetworkFinished, objId, req);
}

void Network::disconnectNetwork(const QString objId, const int64_t node_id, std::string interface, std::string container_id)
{
    network::DisconnectRequest req;
    req.set_node_id(node_id);
    req.set_interface(interface);
    req.set_container_id(container_id);
    RPC_ASYNC(network::DisconnectReply, _disconnectNetwork, disconnectNetworkFinished, objId, req);
}

void Network::listIPtables(const QString objId, const int64_t node_id, std::string container_id)
{
    network::ListIPtablesRequest req;
    req.set_node_id(node_id);
    req.set_container_id(container_id);
    RPC_ASYNC(network::ListIPtablesReply, _listIPtables, listIPtablesFinished, objId, req);
}

void Network::enableIPtables(const QString objId, const int64_t node_id, bool enable, std::string container_id)
{
    network::EnableIPtablesRequest req;
    req.set_node_id(node_id);
    req.set_container_id(container_id);
    req.set_enable(enable);
    RPC_ASYNC(network::EnableIPtablesReply, _enableIPtables, enableIPtablesFinished, objId, req);
}

void Network::createIPtables(const QString objId, const network::CreateIPtablesRequest &req)
{
    RPC_ASYNC(network::CreateIPtablesReply, _createIPtables, createIPtablesFinished, objId, req);
}

void Network::modifyIPtables(const QString objId, const network::ModifyIPtablesRequest &req)
{
    RPC_ASYNC(network::ModifyIPtablesReply, _modifyIPtables, modifyIPtablesFinished, objId, req);
}

void Network::removeIPtables(const QString objId, const network::RemoveIPtablesRequest &req)
{
    RPC_ASYNC(network::RemoveIPtablesReply, _removeIPtables, removeIPtablesFinished, objId, req);
}

void Network::createNic(const QString objId, const network::CreateNicRequest &req)
{
    RPC_ASYNC(network::CreateNicReply, _createNic, createNicFinished, objId, req);
}

void Network::removeNic(const QString objId, const int64_t nodeId, const std::string &name)
{
    network::RemoveNicRequest req;
    req.set_node_id(nodeId);
    req.set_name(name);
    RPC_ASYNC(network::RemoveNicReply, _removeNic, removeNicFinished, objId, req);
}

void Network::updateNic(const QString objId, const network::UpdateNicRequest &req)
{
    RPC_ASYNC(network::UpdateNicReply, _updateNic, updateNicFinished, objId, req);
}

QPair<grpc::Status, network::ListReply> Network::_listNetwork(const network::ListRequest &req)
{
    RPC_IMPL(network::ListReply, network::Network::NewStub, List);
}

QPair<grpc::Status, network::ConnectReply> Network::_connectNetwork(const network::ConnectRequest &req)
{
    RPC_IMPL(network::ConnectReply, network::Network::NewStub, Connect);
}

QPair<grpc::Status, network::DisconnectReply> Network::_disconnectNetwork(const network::DisconnectRequest &req)
{
    RPC_IMPL(network::DisconnectReply, network::Network::NewStub, Disconnect);
}

QPair<grpc::Status, network::ListIPtablesReply> Network::_listIPtables(const network::ListIPtablesRequest &req)
{
    RPC_IMPL(network::ListIPtablesReply, network::Network::NewStub, ListIPtables);
}

QPair<grpc::Status, network::EnableIPtablesReply> Network::_enableIPtables(const network::EnableIPtablesRequest &req)
{
    RPC_IMPL(network::EnableIPtablesReply, network::Network::NewStub, EnableIPtables);
}

QPair<grpc::Status, network::CreateIPtablesReply> Network::_createIPtables(const network::CreateIPtablesRequest &req)
{
    RPC_IMPL(network::CreateIPtablesReply, network::Network::NewStub, CreateIPtables);
}

QPair<grpc::Status, network::ModifyIPtablesReply> Network::_modifyIPtables(const network::ModifyIPtablesRequest &req)
{
    RPC_IMPL(network::ModifyIPtablesReply, network::Network::NewStub, ModifyIPtables);
}

QPair<grpc::Status, network::RemoveIPtablesReply> Network::_removeIPtables(const network::RemoveIPtablesRequest &req)
{
    RPC_IMPL(network::RemoveIPtablesReply, network::Network::NewStub, RemoveIPtables);
}

QPair<grpc::Status, network::CreateNicReply> Network::_createNic(const network::CreateNicRequest &req)
{
    RPC_IMPL(network::CreateNicReply, network::Network::NewStub, CreateNic);
}

QPair<grpc::Status, network::RemoveNicReply> Network::_removeNic(const network::RemoveNicRequest &req)
{
    RPC_IMPL(network::RemoveNicReply, network::Network::NewStub, RemoveNic);
}

QPair<grpc::Status, network::UpdateNicReply> Network::_updateNic(const network::UpdateNicRequest &req)
{
    RPC_IMPL(network::UpdateNicReply, network::Network::NewStub, UpdateNic);
}