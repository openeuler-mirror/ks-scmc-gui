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
    void listUser(const QString objId);
    void createUser(const QString objId, const user::CreateUserRequest &);
    void updateUser(const QString objId, const user::UpdateUserRequest &);
    void removeUser(const QString objId, const std::vector<int64_t> &userIds);

private:
    User(QObject *parent = nullptr);
    ~User();

    // user management
    static QPair<grpc::Status, user::LoginReply> _login(const user::LoginRequest &);
    static QPair<grpc::Status, user::LogoutReply> _logout(const user::LogoutRequest &);
    static QPair<grpc::Status, user::UpdatePasswordReply> _updatePassword(const user::UpdatePasswordRequest &);
    static QPair<grpc::Status, user::ListUserReply> _listUser(const user::ListUserRequest &);
    static QPair<grpc::Status, user::CreateUserReply> _createUser(const user::CreateUserRequest &);
    static QPair<grpc::Status, user::UpdateUserReply> _updateUser(const user::UpdateUserRequest &);
    static QPair<grpc::Status, user::RemoveUserReply> _removeUser(const user::RemoveUserRequest &);

signals:
    // user management
    void loginFinished(const QString objId, const QPair<grpc::Status, user::LoginReply> &);
    void logoutFinished(const QString objId, const QPair<grpc::Status, user::LogoutReply> &);
    void updatePasswordFinished(const QString objId, const QPair<grpc::Status, user::UpdatePasswordReply> &);
    void listUserFinished(const QString objId, const QPair<grpc::Status, user::ListUserReply> &);
    void createUserFinished(const QString objId, const QPair<grpc::Status, user::CreateUserReply> &);
    void updateUserFinished(const QString objId, const QPair<grpc::Status, user::UpdateUserReply> &);
    void removeUserFinished(const QString objId, const QPair<grpc::Status, user::RemoveUserReply> &);

private:
    QMutex mutex;
};
