/**
 * @file          src/base/interface/user.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "user.h"

#include <kiran-log/qt5-log-i.h>
#include <QMutexLocker>
#include <QUuid>
#include "common.h"

User::User(QObject *parent) : QObject(parent)
{
    QMutexLocker locker(&mutex);
}

User::~User()
{
}

QString User::generateId(QObject *callObj)
{
    QString uid = QUuid::createUuid().toString();
    return QString("%1_%2").arg(callObj->metaObject()->className()).arg(uid);
}

void User::login(const QString objId, const std::string &username, const std::string &password)
{
    user::LoginRequest req;
    req.set_username(username);
    req.set_password(password);
    RPC_ASYNC(user::LoginReply, _login, loginFinished, objId, req);
}

void User::logout(const QString objId)
{
    user::LogoutRequest req;
    RPC_ASYNC(user::LogoutReply, _logout, logoutFinished, objId, req);
}

void User::updatePassword(const QString objId, const std::string &oldPassword, const std::string &newPassword)
{
    user::UpdatePasswordRequest req;
    req.set_old_password(oldPassword);
    req.set_new_password(newPassword);
    RPC_ASYNC(user::UpdatePasswordReply, _updatePassword, updatePasswordFinished, objId, req);
}

QPair<grpc::Status, user::LoginReply> User::_login(const user::LoginRequest &req)
{
    QPair<grpc::Status, user::LoginReply> r;
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());
    if (!chan)
    {
        KLOG_WARNING("user::User::NewStub Login failed to get connection");
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        return r;
    }
    grpc::ClientContext ctx;
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(DEADLINE);
    ctx.set_deadline(deadline);
    r.first = user::User::NewStub(chan)->Login(&ctx, req, &r.second);
    if (r.first.ok())
        // s_authKey = r.second.auth_key();
        Common::getInstance().setAuthKey(r.second.auth_key());
    if (grpc::StatusCode::DEADLINE_EXCEEDED == r.first.error_code())
    {
        r.first = grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED,
                               QObject::tr("Response timeout").toStdString());
    }
    return r;
}

QPair<grpc::Status, user::LogoutReply> User::_logout(const user::LogoutRequest &req)
{
    QPair<grpc::Status, user::LogoutReply> r;
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());
    if (!chan)
    {
        KLOG_WARNING("user::User::NewStub Logout failed to get connection");
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        return r;
    }
    grpc::ClientContext ctx;
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(DEADLINE);
    ctx.set_deadline(deadline);
    if (Common::getInstance().getAuthKey().size() > 0)
        ctx.AddMetadata("authorization", Common::getInstance().getAuthKey());
    r.first = user::User::NewStub(chan)->Logout(&ctx, req, &r.second);
    if (r.first.ok())
        // s_authKey = "";
        Common::getInstance().setAuthKey("");
    if (grpc::StatusCode::DEADLINE_EXCEEDED == r.first.error_code())
    {
        r.first = grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED,
                               QObject::tr("Response timeout").toStdString());
    }
    return r;
}

QPair<grpc::Status, user::UpdatePasswordReply> User::_updatePassword(const user::UpdatePasswordRequest &req)
{
    RPC_IMPL(user::UpdatePasswordReply, user::User::NewStub, UpdatePassword);
}
