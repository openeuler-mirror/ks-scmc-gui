/**
 * @file          src/base/interface/network.h
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

class Network : public QObject
{
    Q_OBJECT
public:
    static Network &getInstance()
    {
        static Network s_Network;
        return s_Network;
    }

    static QString generateId(QObject *callObj);

    // network management
    void listNetwork(const QString objId, const int64_t);
    void connectNetwork(const QString objId, const network::ConnectRequest &);
    void disconnectNetwork(const QString objId, const int64_t node_id, std::string interface, std::string container_id);
    void listIPtables(const QString objId, const int64_t node_id, std::string container_id = "");
    void enableIPtables(const QString objId, const int64_t node_id, bool enable, std::string container_id = "");
    void createIPtables(const QString objId, const network::CreateIPtablesRequest &);
    void modifyIPtables(const QString objId, const network::ModifyIPtablesRequest &);
    void removeIPtables(const QString objId, const network::RemoveIPtablesRequest &);
    void createNic(const QString objId, const network::CreateNicRequest &);
    void removeNic(const QString objId, const int64_t nodeId, const std::string &name);
    void updateNic(const QString objId, const network::UpdateNicRequest &);

private:
    Network(QObject *parent = nullptr);
    ~Network();

    // network management
    static QPair<grpc::Status, network::ListReply> _listNetwork(const network::ListRequest &);
    static QPair<grpc::Status, network::ConnectReply> _connectNetwork(const network::ConnectRequest &);
    static QPair<grpc::Status, network::DisconnectReply> _disconnectNetwork(const network::DisconnectRequest &);
    static QPair<grpc::Status, network::ListIPtablesReply> _listIPtables(const network::ListIPtablesRequest &);
    static QPair<grpc::Status, network::EnableIPtablesReply> _enableIPtables(const network::EnableIPtablesRequest &);
    static QPair<grpc::Status, network::CreateIPtablesReply> _createIPtables(const network::CreateIPtablesRequest &);
    static QPair<grpc::Status, network::ModifyIPtablesReply> _modifyIPtables(const network::ModifyIPtablesRequest &);
    static QPair<grpc::Status, network::RemoveIPtablesReply> _removeIPtables(const network::RemoveIPtablesRequest &);
    static QPair<grpc::Status, network::CreateNicReply> _createNic(const network::CreateNicRequest &);
    static QPair<grpc::Status, network::RemoveNicReply> _removeNic(const network::RemoveNicRequest &);
    static QPair<grpc::Status, network::UpdateNicReply> _updateNic(const network::UpdateNicRequest &);

signals:
    // network management
    void listNetworkFinished(const QString objId, const QPair<grpc::Status, network::ListReply> &);
    void connectNetworkFinished(const QString objId, const QPair<grpc::Status, network::ConnectReply> &);
    void disconnectNetworkFinished(const QString objId, const QPair<grpc::Status, network::DisconnectReply> &);
    void listIPtablesFinished(const QString objId, const QPair<grpc::Status, network::ListIPtablesReply> &);
    void enableIPtablesFinished(const QString objId, const QPair<grpc::Status, network::EnableIPtablesReply> &);
    void createIPtablesFinished(const QString objId, const QPair<grpc::Status, network::CreateIPtablesReply> &);
    void modifyIPtablesFinished(const QString objId, const QPair<grpc::Status, network::ModifyIPtablesReply> &);
    void removeIPtablesFinished(const QString objId, const QPair<grpc::Status, network::RemoveIPtablesReply> &);
    void createNicFinished(const QString objId, const QPair<grpc::Status, network::CreateNicReply> &);
    void removeNicFinished(const QString objId, const QPair<grpc::Status, network::RemoveNicReply> &);
    void updateNicFinished(const QString objId, const QPair<grpc::Status, network::UpdateNicReply> &);

private:
    QMutex mutex;
};
