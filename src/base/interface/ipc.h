/**
 * @file          src/base/interface/ipc.h
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

class IPC : public QObject
{
    Q_OBJECT
public:
    static IPC &getInstance()
    {
        static IPC s_IPC;
        return s_IPC;
    }

    static QString generateId(QObject *callObj);

    void listIPC(const QString objId, qint64 nodeID);
    QPair<grpc::Status, ipcbase::IpcListReply> syncListIPC(qint64 nodeID);
    void createIPC(const QString objId, qint64 nodeID, const std::string &ipcName);
    void removeIPC(const QString objId, qint64 nodeID, const std::vector<std::string> &ipcNames);

private:
    IPC(QObject *parent = nullptr);
    ~IPC();

    static QPair<grpc::Status, ipcbase::IpcListReply> _listIPC(const ipcbase::IpcListRequest &);
    static QPair<grpc::Status, ipcbase::IpcCreateReply> _createIPC(const ipcbase::IpcCreateRequest &);
    static QPair<grpc::Status, ipcbase::IpcRemoveReply> _removeIPC(const ipcbase::IpcRemoveRequest &);

signals:
    // IPC management
    void listIPCFinished(QString objId, const QPair<grpc::Status, ipcbase::IpcListReply> &);
    void createIPCFinished(const QString objId, const QPair<grpc::Status, ipcbase::IpcCreateReply> &);
    void removeIPCFinished(const QString objId, const QPair<grpc::Status, ipcbase::IpcRemoveReply> &);

private:
    QMutex mutex;
};