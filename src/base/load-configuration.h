/**
 * @file          /ks-scmc-gui/src/base/load-configuration.h
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef LOADCONFIGURATION_H
#define LOADCONFIGURATION_H

#include <QSettings>

class LoadConfiguration : public QObject
{
    Q_OBJECT
public:
    static LoadConfiguration &Instance();
    LoadConfiguration(QObject *parent = nullptr);
    ~LoadConfiguration();
    QString getTerminalConfig(QString nodeAddr, QString containerName, QString appexec = "");
    void getSslConfig(bool &enable, QString &ca, QString &cert, QString &key);

private:
    void initConfig();
    void setValue();
    void getTerminalCmd();
    QString _getTerminalConfig();
    void _getSslConfig();

private:
    QSettings *m_settings;
    bool m_enable;
    QString m_ca;
    QString m_cert;
    QString m_key;
    QString m_cmd;
    QString m_totalCmd;
};

#endif  // LOADCONFIGURATION_H
