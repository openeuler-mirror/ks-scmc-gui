/**
 * @file          src/base/interface/ipc.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "ipc.h"
#include "common.h"

#include <kiran-log/qt5-log-i.h>

#include <QMutexLocker>
#include <QUuid>

IPC::IPC(QObject* parent) : QObject(parent)
{
    QMutexLocker locker(&mutex);
}

IPC::~IPC()
{
}

QString IPC::generateId(QObject* callObj)
{
    QString uid = QUuid::createUuid().toString();
    return QString("%1_%2").arg(callObj->metaObject()->className()).arg(uid);
}

void IPC::listIPC(const QString objId, qint64 nodeID)
{
    ipcbase::IpcListRequest req;
    req.set_node_id(nodeID);
    RPC_ASYNC(ipcbase::IpcListReply, _listIPC, listIPCFinished, objId, req);
}

void IPC::createIPC(const QString objId, qint64 nodeID, const std::string& ipcName)
{
    ipcbase::IpcCreateRequest req;
    req.set_node_id(nodeID);
    req.set_name(ipcName);
    RPC_ASYNC(ipcbase::IpcCreateReply, _createIPC, createIPCFinished, objId, req);
}

void IPC::removeIPC(const QString objId, qint64 nodeID, const std::vector<std::string>& ipcNames)
{
    ipcbase::IpcRemoveRequest req;
    req.set_node_id(nodeID);
    for (auto ipcName : ipcNames)
    {
        req.add_ipc_name(ipcName);
    }
    RPC_ASYNC(ipcbase::IpcRemoveReply, _removeIPC, removeIPCFinished, objId, req);
}

QPair<grpc::Status, ipcbase::IpcListReply> IPC::_listIPC(const ipcbase::IpcListRequest& req)
{
    RPC_IMPL(ipcbase::IpcListReply, ipcbase::IpcBase::NewStub, List);
}

QPair<grpc::Status, ipcbase::IpcCreateReply> IPC::_createIPC(const ipcbase::IpcCreateRequest& req)
{
    RPC_IMPL(ipcbase::IpcCreateReply, ipcbase::IpcBase::NewStub, Create);
}

QPair<grpc::Status, ipcbase::IpcRemoveReply> IPC::_removeIPC(const ipcbase::IpcRemoveRequest& req)
{
    RPC_IMPL(ipcbase::IpcRemoveReply, ipcbase::IpcBase::NewStub, Remove);
}
