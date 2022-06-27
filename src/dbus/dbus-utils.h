/**
 * @file          /ks-scmc-gui/src/dbus/dbus-utils.h
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef DBUSUTILS_H
#define DBUSUTILS_H

#include <kylin-license/license-i.h>
#include <QObject>
#include <QString>
#include <QWidget>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QtDBus>

//#include "SCMCInterface.h"

#define LICENSE_OBJECT_SCMC_NAME "KSSCMC"
//#define LICENSE_OBJECT_OS_NAME "KylinSecOS"
#define TIMEOUT_MS 5000
#define KIRAN_LICENSE_INTERFACE "com.kylinsec.Kiran.LicenseHelper"
#define LICENSE_OBJECT_DBUS_NAME "com.kylinsec.Kiran.LicenseObject"
#define METHOD_GET_LICENSE "GetLicense"
#define METHOD_GET_LICENSE_JSON "GetLicenseJson"
#define METHOD_GET_LICENSE_OBJECK "GetLicenseObject"
#define METHOD_REGISTER_BY_LICENSE_CODE "RegisterByLicenseCode"

enum DbusInterface
{
    ACTIVATE_BYACTIVATIONCODE,
    GET_LICENSE
};

class DBusUtils : public QObject
{
    Q_OBJECT
public:
    DBusUtils(QObject* parent = 0);
    ~DBusUtils();

    QString callInterface(DbusInterface num);
    bool callInterface(DbusInterface num, QString args);

signals:
    void standardChanged(uint type);
    void callDbusFailed();
    void LicenseChanged(bool);

private slots:
    void callFailed();

private:
    bool creatObjectScmcName();
    //    com::kylinsec::SCMC* SCMC;
};

#endif  // DBUSUTILS_H
