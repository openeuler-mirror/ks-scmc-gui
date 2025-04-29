/**
 * @file          src/base/interface/system.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma once

#include <QMutex>
#include <QObject>

#include <grpcpp/grpcpp.h>

#include "rpc.h"

class System : public QObject
{
    Q_OBJECT
public:
    static System &getInstance()
    {
        static System s_System;
        return s_System;
    }

    static QString generateId(QObject *callObj);

    // system management
    void setSecuritySwitch(const QString objId, const bool &isOn);
    void getSecuritySwitch(const QString objId);
    void dashboard(const QString objId);

private:
    System(QObject *parent = nullptr);
    ~System();

    // system management
    static QPair<grpc::Status, sys::SetSecuritySwitchReply> _setSecuritySwitch(const sys::SetSecuritySwitchRequest &);
    static QPair<grpc::Status, sys::GetSecuritySwitchReply> _getSecuritySwitch(const sys::GetSecuritySwitchRequest &);
    static QPair<grpc::Status, sys::DashboardReply> _dashboard(const sys::DashboardRequest &);

signals:
    // system management
    void setSecuritySwitchFinished(const QString objId, const QPair<grpc::Status, sys::SetSecuritySwitchReply> &);
    void getSecuritySwitchFinished(const QString objId, const QPair<grpc::Status, sys::GetSecuritySwitchReply> &);
    void dashboardFinished(const QString objId, const QPair<grpc::Status, sys::DashboardReply> &);

private:
    QMutex mutex;
};
