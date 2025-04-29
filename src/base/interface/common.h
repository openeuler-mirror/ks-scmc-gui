/**
 * @file          src/base/interface/common.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma once

#include <kiran-log/qt5-log-i.h>
#include <QFuture>
#include <QFutureWatcher>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QtConcurrent/QtConcurrent>
#include "def.h"

#include <grpcpp/grpcpp.h>
#include "rpc.h"
#include "user-configuration.h"

static std::string s_authKey = "";

#define DEADLINE 8000

#define RPC_ASYNC(REPLY_TYPE, WORKER, CALLBACK, OBJID, ...)       \
    typedef QPair<grpc::Status, REPLY_TYPE> T;                    \
    QFutureWatcher<T> *watcher = new QFutureWatcher<T>();         \
    watcher->setFuture(QtConcurrent::run(WORKER, ##__VA_ARGS__)); \
    connect(watcher, &QFutureWatcher<T>::finished, [this, watcher, OBJID] { \
        auto reply = watcher->result();                                     \
        emit CALLBACK(OBJID, reply);                                        \
        delete watcher; });

#define RPC_IMPL(REPLY_TYPE, STUB, RPC_NAME)                                                \
    QPair<grpc::Status, REPLY_TYPE> r;                                                      \
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());                        \
    if (!chan)                                                                              \
    {                                                                                       \
        KLOG_WARNING("%s %s failed to get connection", #STUB, #RPC_NAME);                   \
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,                                   \
                               QObject::tr("Network Error").toStdString());                 \
        return r;                                                                           \
    }                                                                                       \
    grpc::ClientContext ctx;                                                                \
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(DEADLINE); \
    ctx.set_deadline(deadline);                                                             \
    if (Common::getInstance().getAuthKey().size() > 0)                                      \
        ctx.AddMetadata("authorization", Common::getInstance().getAuthKey());               \
    r.first = STUB(chan)->RPC_NAME(&ctx, req, &r.second);                                   \
    if (grpc::StatusCode(ErrUnauthenticated) == r.first.error_code())                       \
        emit Common::getInstance().sessionExpire();                                         \
    else if (grpc::StatusCode::DEADLINE_EXCEEDED == r.first.error_code())                   \
    {                                                                                       \
        r.first = grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED,                         \
                               QObject::tr("Response timeout").toStdString());              \
    }                                                                                       \
    return r;

enum ErrCode
{
    ErrOK = 0,
    ErrCanceled = 1,            // "请求取消"
    ErrUnknown = 2,             // "未知错误"
    ErrInvalidArgument = 3,     // "参数错误"
    ErrDeadlineExceeded = 4,    // "请求超时"
    ErrNotFound = 5,            // "资源不存在"
    ErrAlreadyExists = 6,       // "资源冲突"
    ErrPermissionDenied = 7,    // "拒绝请求"
    ErrResourceExhausted = 8,   // ResourceExhausted framework error
    ErrFailedPrecondition = 9,  // FailedPrecondition
    ErrAborted = 10,            // Aborted
    ErrOutOfRange = 11,         // OutOfRange
    ErrUnimplemented = 12,      // Unimplemented framework error
    ErrInternal = 13,           // Internal, "内部错误" framework error | rpc failure
    ErrUnavailable = 14,        // Unavailable  // framework error
    ErrDataLoss = 15,           // DataLoss
    ErrUnauthenticated = 16,    // Unauthenticated, "用户请求未认证"
    ErrWrongPassword = 17,      // Errno_WrongPassword, "密码错误"

    ErrUserNotExist = -1,
    ErrStatus = -2,
};

class Common : public QObject
{
    Q_OBJECT
public:
    static Common &getInstance()
    {
        static Common s_Common;
        return s_Common;
    }

    static std::string getAuthKey();
    static void setAuthKey(std::string str);

    void stopTransfer(const QString &name, const QString &version, bool isStop);
    bool isTransferStoped(const QString &name, const QString &version);

private:
    Common(QObject *parent = nullptr);
    ~Common();

signals:
    void sessionExpire();
    void transferImageFinished(QString, QString);
    void transferImageStatus(ImageTransmissionStatus, QString, QString, int);

private:
    QMutex mutex;
    QMap<QString, bool> m_transferStatusMap;
};
