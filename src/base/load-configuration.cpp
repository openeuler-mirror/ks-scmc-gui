/**
 * @file          /ks-scmc-gui/src/base/load-configuration.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "load-configuration.h"
#include <kiran-log/qt5-log-i.h>
#include <QFileInfo>
#include <QProcess>
#include <QSettings>

#define VERSION_NAME "version"
#define VERSION_NUMBER "1.1.1"
#define SSL_GROUP_NAME "ssl"
#define SSL_ENABLE "enable"
#define SSL_CA "ca"
#define SSL_CERT "cert"
#define SSL_KEY "key"
#define TERMINAL_GROUP_NAME "terminal"
#define TERMINAL_CMD "cmd"
#define TERMINAL_RCFILE "rcfile"
#define TERMINAL_TOTAL_CMD "total_cmd"

LoadConfiguration::LoadConfiguration(QObject *parent) : m_settings(nullptr)
{
    m_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "kylinsec", "ks-scmc-config");
    initConfig();
    _getSslConfig();
}

LoadConfiguration::~LoadConfiguration()
{
    if (m_settings)
    {
        delete m_settings;
        m_settings = nullptr;
    }
}

QString LoadConfiguration::getTerminalConfig(QString nodeAddr, QString containerName, QString appexec)
{
    QString cmd = _getTerminalConfig();
    QString result;
    if (appexec.isEmpty())
    {
        result = cmd.replace("${nodeAddr}", nodeAddr).replace("${containerName}", containerName).replace("${appexec}", "");
    }
    else
    {
        result = cmd.replace("${nodeAddr}", nodeAddr).replace("${containerName}", containerName).replace("${appexec}", appexec);
    }
    return result;
}

void LoadConfiguration::getSslConfig(bool &enable, QString &ca, QString &cert, QString &key)
{
    enable = m_enable;
    ca = m_ca;
    cert = m_cert;
    key = m_key;
}

LoadConfiguration &LoadConfiguration::Instance()
{
    static LoadConfiguration loadCfg;
    return loadCfg;
}

void LoadConfiguration::initConfig()
{
    getTerminalCmd();
    QFileInfo fileinfo(m_settings->fileName());
    if (!fileinfo.isFile())
    {
        KLOG_DEBUG() << "Create config file";
        setValue();
    }
    else
    {
        KLOG_DEBUG() << "Config version:" << QString(m_settings->value(VERSION_NAME).toString()) << VERSION_NUMBER;
        if (QString(m_settings->value(VERSION_NAME).toString()) != VERSION_NUMBER)
        {
            KLOG_DEBUG() << "Update config file";
            m_settings->clear();
            setValue();
        }
    }
}

void LoadConfiguration::setValue()
{
    m_settings->setValue(VERSION_NAME, VERSION_NUMBER);
    m_settings->beginGroup(TERMINAL_GROUP_NAME);
    m_settings->setValue(TERMINAL_CMD, m_cmd);
    m_settings->setValue(TERMINAL_RCFILE, "/etc/ks-scmc/graphic_rc");
    m_settings->setValue(TERMINAL_TOTAL_CMD, m_totalCmd);
    m_settings->endGroup();
    m_settings->beginGroup(SSL_GROUP_NAME);
    m_settings->setValue(SSL_ENABLE, "false");
    m_settings->setValue(SSL_CA, "/etc/ks-scmc/x509/ca.pem");
    m_settings->setValue(SSL_CERT, "/etc/ks-scmc/x509/client-cert.pem");
    m_settings->setValue(SSL_KEY, "/etc/ks-scmc/x509/client-key.pem");
    m_settings->endGroup();
    m_settings->sync();
}

void LoadConfiguration::getTerminalCmd()
{
    const QString sshCmd = "ssh -Xt -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null ks-scmc@${nodeAddr} sudo /etc/ks-scmc/access-container-gui ${containerName} ${appexec}";
    if (0 == QProcess::execute("which mate-terminal"))
    {
        m_cmd = "mate-terminal --disable-factory -e";
        m_totalCmd = m_cmd + " \"" + sshCmd + "\"";
    }
    else
    {
        if (0 == QProcess::execute("which konsole"))
        {
            m_cmd = "konsole --nofork -e";
            m_totalCmd = m_cmd + " " + sshCmd;
        }
    }
}

QString LoadConfiguration::_getTerminalConfig()
{
    QString cmd = m_settings->value(QString(TERMINAL_GROUP_NAME) + "/" + QString(TERMINAL_TOTAL_CMD)).toString();
    if (cmd.isEmpty())
    {
        cmd = m_totalCmd;
    }

    return cmd;
}

void LoadConfiguration::_getSslConfig()
{
    m_settings->beginGroup(SSL_GROUP_NAME);
    QString enableVal = m_settings->value(SSL_ENABLE).toString();
    QString caVal = m_settings->value(SSL_CA).toString();
    QString certVal = m_settings->value(SSL_CERT).toString();
    QString keyVal = m_settings->value(SSL_KEY).toString();
    m_settings->endGroup();

    m_enable = enableVal == "true" ? true : false;
    if (!m_enable)
    {
        return;
    }

    if (!caVal.isEmpty())
    {
        QFile file(caVal);
        if (file.open(QIODevice::ReadOnly))
        {
            KLOG_DEBUG() << "ca file size:" << file.size();
            m_ca = file.readAll();
            file.close();
        }
        else
        {
            KLOG_WARNING() << "ca:" << caVal << file.errorString();
        }
    }

    if (!certVal.isEmpty())
    {
        m_cert = "error";
        QFile file(certVal);
        if (file.open(QIODevice::ReadOnly))
        {
            KLOG_DEBUG() << "cert file size:" << file.size();
            if (0 != file.size())
            {
                m_cert = file.readAll();
            }
            file.close();
        }
        else
        {
            KLOG_WARNING() << "cert:" << certVal << file.errorString();
        }
    }

    if (!keyVal.isEmpty())
    {
        m_key = "error";
        QFile file(keyVal);
        if (file.open(QIODevice::ReadOnly))
        {
            KLOG_DEBUG() << "key file size:" << file.size();
            if (0 != file.size())
            {
                m_key = file.readAll();
            }
            file.close();
        }
        else
        {
            KLOG_WARNING() << "key:" << keyVal << file.errorString();
        }
    }
}
