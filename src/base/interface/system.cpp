/**
 * @file          src/base/interface/system.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "system.h"
#include <kiran-log/qt5-log-i.h>
#include <QMutexLocker>
#include <QPair>
#include <QUuid>
#include "common.h"
System::System(QObject *parent) : QObject(parent)
{
    QMutexLocker locker(&mutex);
}

System::~System()
{
}

QString System::generateId(QObject *callObj)
{
    QString uid = QUuid::createUuid().toString();
    return QString("%1_%2").arg(callObj->metaObject()->className()).arg(uid);
}

void System::setSecuritySwitch(const QString objId, const bool &isOn)
{
    sys::SetSecuritySwitchRequest req;
    req.set_is_on(isOn);
    QPair<grpc::Status, sys::SetSecuritySwitchReply> reply = _setSecuritySwitch(req);
    emit setSecuritySwitchFinished(objId, reply);
}

void System::getSecuritySwitch(const QString objId)
{
    sys::GetSecuritySwitchRequest req;
    QPair<grpc::Status, sys::GetSecuritySwitchReply> reply = _getSecuritySwitch(req);
    emit getSecuritySwitchFinished(objId, reply);
}

void System::dashboard(const QString objId)
{
    sys::DashboardRequest req;
    RPC_ASYNC(sys::DashboardReply, _dashboard, dashboardFinished, objId, req);
}

QPair<grpc::Status, sys::SetSecuritySwitchReply> System::_setSecuritySwitch(const sys::SetSecuritySwitchRequest &req)
{
    RPC_IMPL(sys::SetSecuritySwitchReply, sys::System::NewStub, SetSecuritySwitch);
}

QPair<grpc::Status, sys::GetSecuritySwitchReply> System::_getSecuritySwitch(const sys::GetSecuritySwitchRequest &req)
{
    RPC_IMPL(sys::GetSecuritySwitchReply, sys::System::NewStub, GetSecuritySwitch);
}

QPair<grpc::Status, sys::DashboardReply> System::_dashboard(const sys::DashboardRequest &req)
{
    RPC_IMPL(sys::DashboardReply, sys::System::NewStub, Dashboard);
}
