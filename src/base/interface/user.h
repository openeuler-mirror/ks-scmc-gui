/**
 * @file          src/base/interface/user.h
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
#include "user-configuration.h"

class User : public QObject
{
    Q_OBJECT
public:
    static User &getInstance()
    {
        static User s_User;
        return s_User;
    }

    static QString generateId(QObject *callObj);

    // user management
    void login(const QString objId, const std::string &username, const std::string &password);
    void logout(const QString objId);
    void updatePassword(const QString objId, const std::string &oldPassword, const std::string &newPassword);

private:
    User(QObject *parent = nullptr);
    ~User();

    // user management
    static QPair<grpc::Status, user::LoginReply> _login(const user::LoginRequest &);
    static QPair<grpc::Status, user::LogoutReply> _logout(const user::LogoutRequest &);
    static QPair<grpc::Status, user::UpdatePasswordReply> _updatePassword(const user::UpdatePasswordRequest &);

signals:
    // user management
    void loginFinished(const QString objId, const QPair<grpc::Status, user::LoginReply> &);
    void logoutFinished(const QString objId, const QPair<grpc::Status, user::LogoutReply> &);
    void updatePasswordFinished(const QString objId, const QPair<grpc::Status, user::UpdatePasswordReply> &);

private:
    QMutex mutex;
};
