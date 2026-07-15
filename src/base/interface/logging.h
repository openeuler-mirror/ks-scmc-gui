/**
 * @file          src/base/interface/logging.h
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

class Logging : public QObject
{
    Q_OBJECT
public:
    static Logging &getInstance()
    {
        static Logging s_Logging;
        return s_Logging;
    }

    static QString generateId(QObject *callObj);

    // logging management
    void listRuntimeLogging(const QString objId, const logging::ListRuntimeRequest &);
    void listWarnLogging(const QString objId, const logging::ListWarnRequest &);
    void readWarnLogging(const QString objId, QList<int64_t> ids);
    void setLog(const QString objId, int days);
    void getLog(const QString objId);

private:
    Logging(QObject *parent = nullptr);
    ~Logging();

    // logging management
    static QPair<grpc::Status, logging::ListRuntimeReply> _listRuntimeLogging(const logging::ListRuntimeRequest &);
    static QPair<grpc::Status, logging::ListWarnReply> _listWarnLogging(const logging::ListWarnRequest &);
    static QPair<grpc::Status, logging::ReadWarnReply> _listReadWarnLogging(const logging::ReadWarnRequest &);
    static QPair<grpc::Status, logging::GetLogReply> _getLog(const logging::GetLogRequest &);
    static QPair<grpc::Status, logging::SetLogReply> _setLog(const logging::SetLogRequest &);

signals:
    // logging management
    void loggingRuntimeFinished(const QString objId, const QPair<grpc::Status, logging::ListRuntimeReply> &);
    void loggingListWarnFinished(const QString objId, const QPair<grpc::Status, logging::ListWarnReply> &);
    void loggingReadWarnFinished(const QString objId, const QPair<grpc::Status, logging::ReadWarnReply> &);
    void loggingSetLogFinished(const QString objId, const QPair<grpc::Status, logging::SetLogReply> &);
    void loggingGetLogFinished(const QString objId, const QPair<grpc::Status, logging::GetLogReply> &);

private:
    QMutex mutex;
};
