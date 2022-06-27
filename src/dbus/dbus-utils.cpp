/**
 * @file          /ks-scmc-gui/src/dbus/dbus-utils.cpp
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "dbus-utils.h"

#include <kiran-log/qt5-log-i.h>
#include <exception>

DBusUtils::DBusUtils(QObject *parent) : QObject(parent)
{
    creatObjectScmcName();
//    SCMC = new com::kylinsec::SCMC(SCMC_DBUS_NAME, SCMC_DBUS_OBJECT_PATH, QDBusConnection::systemBus(), parent);

//    connect(SCMC, SIGNAL(com::kylinsec::SCMC::standard_type()),this, SLOT(standardTypeChanged(uint)));
}

DBusUtils::~DBusUtils()
{
}

bool DBusUtils::creatObjectScmcName()
{
    QDBusMessage msgMethodCall = QDBusMessage::createMethodCall(LICENSE_MANAGER_DBUS_NAME,
                                                                LICENSE_MANAGER_OBJECT_PATH,
                                                                LICENSE_MANAGER_DBUS_NAME,
                                                                METHOD_GET_LICENSE_OBJECK);
    msgMethodCall << LICENSE_OBJECT_SCMC_NAME;

    QDBusMessage msgReply = QDBusConnection::systemBus().call(msgMethodCall,
                                                              QDBus::Block,
                                                              TIMEOUT_MS);
    QString errorMsg;
    if (msgReply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = msgReply.arguments();
        if (args.size() < 1)
        {
            errorMsg = "arguments size < 1";
            goto failed;
        }

        return true;
    }
failed:
    KLOG_WARNING() << LICENSE_DBUS_NAME << METHOD_GET_LICENSE_JSON
                   << msgReply.errorName() << msgReply.errorMessage() << errorMsg;
    return false;
}

static QString GetLicense()
{
    QDBusMessage msgMethodCall = QDBusMessage::createMethodCall(LICENSE_MANAGER_DBUS_NAME,
                                                                QString(LICENSE_OBJECT_OBJECT_PATH) + "/" + LICENSE_OBJECT_SCMC_NAME,
                                                                LICENSE_OBJECT_DBUS_NAME,
                                                                METHOD_GET_LICENSE);
    QDBusMessage msgReply = QDBusConnection::systemBus().call(msgMethodCall,
                                                              QDBus::Block,
                                                              TIMEOUT_MS);
    KLOG_DEBUG() << "msgReply " << msgReply;
    QString errorMsg;
    if (msgReply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = msgReply.arguments();
        if (args.size() < 1)
        {
            errorMsg = "arguments size < 1";
            goto failed;
        }
        QVariant firstArg = args.takeFirst();
        return firstArg.toString();
    }

failed:
    KLOG_WARNING() << LICENSE_DBUS_NAME << METHOD_GET_LICENSE_JSON
                   << msgReply.errorName() << msgReply.errorMessage() << errorMsg;
    return "";
}

static bool ActivateByActivationCode(QString activation_Code, QString &errorMsg)
{
    QDBusMessage msgMethodCall = QDBusMessage::createMethodCall(LICENSE_MANAGER_DBUS_NAME,
                                                                QString(LICENSE_OBJECT_OBJECT_PATH) + "/" + LICENSE_OBJECT_SCMC_NAME,
                                                                LICENSE_OBJECT_DBUS_NAME,
                                                                "ActivateByActivationCode");
    msgMethodCall << activation_Code;

    QDBusMessage msgReply = QDBusConnection::systemBus().call(msgMethodCall,
                                                              QDBus::Block,
                                                              TIMEOUT_MS);

    KLOG_DEBUG() << "msgReply " << msgReply;

    if (msgReply.type() == QDBusMessage::ErrorMessage)
    {
        errorMsg = msgReply.errorMessage();
        goto failed;
    }
    return true;

failed:
    KLOG_WARNING() << LICENSE_DBUS_NAME << METHOD_REGISTER_BY_LICENSE_CODE
                   << msgReply.errorName() << msgReply.errorMessage();
    return false;
}

QString DBusUtils::callInterface(DbusInterface num)
{
    QString ret;
    switch (num)
    {
    case GET_LICENSE:
        ret = GetLicense();
        KLOG_DEBUG() << ret;
        break;
    default:
        KLOG_WARNING() << "error interface!";
        break;
    }

    if(ret == QString("failed"))
    {
        callFailed();
    }
    return ret;
}

bool DBusUtils::callInterface(DbusInterface num, QString args)
{
    bool ret = false;
    QString errorMsg;
    switch (num)
    {
    case ACTIVATE_BYACTIVATIONCODE:
        ret = ActivateByActivationCode(args,errorMsg);
        emit LicenseChanged(ret);
        break;
    default:
        KLOG_WARNING() << "error interface!";
        break;
    }
    return ret;
}

void DBusUtils::callFailed()
{
    emit callDbusFailed();
}
