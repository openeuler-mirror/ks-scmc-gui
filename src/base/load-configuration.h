/**
 * @file          /ks-scmc-gui/src/base/load-configuration.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef LOADCONFIGURATION_H
#define LOADCONFIGURATION_H

#include <QSettings>

class LoadConfiguration
{
public:
    LoadConfiguration();
    ~LoadConfiguration();
    static QString getTerminalConfig(QString nodeAddr, QString containerName, QString appexec = "")
    { return LoadConfiguration::Instance()._getTerminalConfig(nodeAddr, containerName, appexec); }
    static void getSSLConfig(bool &enable, QString &ca, QString &cert, QString &key)
    { LoadConfiguration::Instance()._getSSLConfig(enable, ca, cert, key); }

private:
    static LoadConfiguration &Instance();
    void initConfig();
    QString readConfig(QString group, QString key);
    QString _getTerminalConfig(QString nodeAddr, QString containerName, QString appexec);
    void _getSSLConfig(bool &enable, QString &ca, QString &cert, QString &key);

private:
    QSettings *m_settings;
    QMap<QString, QString> m_values;
};

#endif // LOADCONFIGURATION_H
