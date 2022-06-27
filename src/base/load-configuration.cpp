/**
 * @file          /ks-scmc-gui/src/base/load-configuration.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "load-configuration.h"
#include <kiran-log/qt5-log-i.h>
#include <QSettings>
#include <QFileInfo>

LoadConfiguration::LoadConfiguration() :m_settings(nullptr)
{
    m_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "kylinsec", "ks-scmc-config");
    initConfig();
}

LoadConfiguration::~LoadConfiguration()
{
    if (m_settings)
    {
        delete m_settings;
        m_settings = nullptr;
    }
}


LoadConfiguration &LoadConfiguration::Instance()
{
    static LoadConfiguration loadCfg;
    return loadCfg;
}

void LoadConfiguration::initConfig()
{
    m_values.insert("TERMINAL_CMD", "mate-terminal -e");
    m_values.insert("BASHRC_FILE", "/etc/ks-scmc/graphic_rc");
    m_values.insert("TERMINAL_USAGE", "mate-terminal -e \"ssh -Xt root@${nodeAddr} CONTAINER_NAME=${containerName} bash --rcfile /etc/ks-scmc/graphic_rc\"");
    m_values.insert("SSL_ENABLE", "false");
    m_values.insert("SSL_CA", "/etc/ks-scmc/x509/ca.pem");
    m_values.insert("SSL_CERT", "/etc/ks-scmc/x509/client-cert.pem");
    m_values.insert("SSL_KEY", "/etc/ks-scmc/x509/client-key.pem");

    QFileInfo fileinfo(m_settings->fileName());
    fileinfo.isFile();
    if (!fileinfo.isFile())
    {
        m_settings->beginGroup("terminal");
        m_settings->setValue("cmd",  m_values["TERMINAL_CMD"]);
        m_settings->setValue("rcfile",  m_values["BASHRC_FILE"]);
        m_settings->setValue("total_cmd", m_values["TERMINAL_USAGE"]);
        m_settings->endGroup();

        m_settings->beginGroup("ssl");
        m_settings->setValue("enable", m_values["SSL_ENABLE"]);
        m_settings->setValue("ca", m_values["SSL_CA"]);
        m_settings->setValue("cert", m_values["SSL_CERT"]);
        m_settings->setValue("key", m_values["SSL_KEY"]);
        m_settings->endGroup();
        m_settings->sync();
    }
}

QString LoadConfiguration::readConfig(QString group, QString key)
{
    QString value = m_settings->value(group + "/" + key).toString();
    return  value;
}


QString LoadConfiguration::_getTerminalConfig(QString nodeAddr, QString containerName)
{
    const QString group = "terminal";
    QString cmd = readConfig(group, "total_cmd");
    if (cmd.isEmpty())
        cmd = m_values["TERMINAL_USAGE"];
    QString result = cmd.replace("${nodeAddr}", nodeAddr).replace("${containerName}", containerName);

//    QString execmd = readConfig(group, "cmd");
//    if (execmd.isEmpty())
//        execmd = m_values["TERMINAL_CMD"];
//    QString file = readConfig(group,  "rcfile");
//    if (file.isEmpty())
//        file = m_values["BASHRC_FILE"];

    return  result;
}

void LoadConfiguration::_getSSLConfig(bool &enable, QString &ca, QString &cert, QString &key)
{
    const QString group = "ssl";
    QString strEnable = readConfig(group, "enable");
    if (strEnable.isEmpty())
        strEnable = m_values["SSL_ENABLE"];
    if (strEnable == "true")
        enable = true;
    else
        enable = false;

    ca = readConfig(group, "ca");
    if (ca.isEmpty())
        ca = m_values["SSL_CA"];


    cert = readConfig(group,  "cert");
    if (cert.isEmpty())
        cert = m_values["SSL_CERT"];


    key = readConfig(group,  "key");
    if (key.isEmpty())
        key = m_values["SSL_KEY"];
}


