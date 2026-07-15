/**
 * @file          src/base/interface/logging.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "logging.h"
#include "common.h"

#include <kiran-log/qt5-log-i.h>

#include <QMutexLocker>
#include <QUuid>

Logging::Logging(QObject *parent) : QObject(parent)
{
    QMutexLocker locker(&mutex);
}

Logging::~Logging()
{
}

QString Logging::generateId(QObject *callObj)
{
    QString uid = QUuid::createUuid().toString();
    return QString("%1_%2").arg(callObj->metaObject()->className()).arg(uid);
}

void Logging::listRuntimeLogging(const QString objId, const logging::ListRuntimeRequest &req)
{
    RPC_ASYNC(logging::ListRuntimeReply, _listRuntimeLogging, loggingRuntimeFinished, objId, req);
}

void Logging::listWarnLogging(const QString objId, const logging::ListWarnRequest &req)
{
    RPC_ASYNC(logging::ListWarnReply, _listWarnLogging, loggingListWarnFinished, objId, req);
}

void Logging::readWarnLogging(const QString objId, QList<int64_t> ids)
{
    logging::ReadWarnRequest req;
    foreach (int64_t id, ids)
    {
        req.add_ids(id);
    }
    RPC_ASYNC(logging::ReadWarnReply, _listReadWarnLogging, loggingReadWarnFinished, objId, req);
}

void Logging::setLog(const QString objId, int days)
{
    logging::SetLogRequest req;
    req.set_save_log_days(days);
    RPC_ASYNC(logging::SetLogReply, _setLog, loggingSetLogFinished, objId, req);
}

void Logging::getLog(const QString objId)
{
    logging::GetLogRequest req;
    RPC_ASYNC(logging::GetLogReply, _getLog, loggingGetLogFinished, objId, req);
}

QPair<grpc::Status, logging::ListRuntimeReply> Logging::_listRuntimeLogging(const logging::ListRuntimeRequest &req)
{
    RPC_IMPL(logging::ListRuntimeReply, logging::Logging::NewStub, ListRuntime);
}

QPair<grpc::Status, logging::ListWarnReply> Logging::_listWarnLogging(const logging::ListWarnRequest &req)
{
    RPC_IMPL(logging::ListWarnReply, logging::Logging::NewStub, ListWarn);
}

QPair<grpc::Status, logging::ReadWarnReply> Logging::_listReadWarnLogging(const logging::ReadWarnRequest &req)
{
    RPC_IMPL(logging::ReadWarnReply, logging::Logging::NewStub, ReadWarn);
}

QPair<grpc::Status, logging::GetLogReply> Logging::_getLog(const logging::GetLogRequest &req)
{
    RPC_IMPL(logging::GetLogReply, logging::Logging::NewStub, GetLog);
}

QPair<grpc::Status, logging::SetLogReply> Logging::_setLog(const logging::SetLogRequest &req)
{
    RPC_IMPL(logging::SetLogReply, logging::Logging::NewStub, SetLog);
}
