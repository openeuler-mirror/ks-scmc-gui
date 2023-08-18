#include "user-configuration.h"
#include <cryptopp/base64.h>
#include <cryptopp/des.h>
#include <cryptopp/modes.h>
#include <kiran-log/qt5-log-i.h>
#include <QApplication>

using namespace CryptoPP;
static std::string m_ServerAddr;
UserConfiguration::UserConfiguration() : m_loginSettings(nullptr), m_serverSetting(nullptr)
{
    m_loginSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "kylinsec", "ks-scmc-user");
    m_serverSetting = new QSettings(QSettings::IniFormat, QSettings::UserScope, "kylinsec", "ks-scmc-server");
    QStringList args = qApp->arguments();
    if (args.size() > 1)
    {
        m_ServerAddr = args[1].toStdString();
    }
}

UserConfiguration::~UserConfiguration()
{
    if (m_loginSettings)
    {
        delete m_loginSettings;
        m_loginSettings = nullptr;
    }
    if (m_serverSetting)
    {
        delete m_serverSetting;
        m_serverSetting = nullptr;
    }
}

void UserConfiguration::readConfig(ConfigSettingType type, QString group, QString key, QString &value)
{
    QSettings *settings;
    bool isEncryptedPw = false;
    switch (type)
    {
    case CONFIG_SETTING_TYPE_LOGIN:
        settings = m_loginSettings;
        isEncryptedPw = true;
        break;
    case CONFIG_SETTING_TYPE_SERVER:
        settings = m_serverSetting;
        break;
    default:
        break;
    }
    value.clear();
    //读取配置信息
    value = settings->value(group + "/" + key).toString();
    if (isEncryptedPw)
    {
        if (!value.isEmpty())
        {
            std::string decryptedPw = desDecrypt(value.toStdString());
            value = QString::fromStdString(decryptedPw);
        }
    }
}

void UserConfiguration::writeConfig(ConfigSettingType type, QString group, QString key, QString value)
{
    QSettings *settings;
    switch (type)
    {
    case CONFIG_SETTING_TYPE_LOGIN:
        settings = m_loginSettings;
        break;
    case CONFIG_SETTING_TYPE_SERVER:
        settings = m_serverSetting;
        break;
    default:
        break;
    }
    QVariant variant;
    variant.setValue(value);
    //将信息写入配置文件
    settings->beginGroup(group);
    settings->setValue(key, variant);
    settings->endGroup();
}

std::string UserConfiguration::getServerAddr()
{
    return m_ServerAddr;
}

void UserConfiguration::setServerAddr(std::string addr)
{
    m_ServerAddr = addr;
}

std::string UserConfiguration::desEncrypt(const std::string &message, const std::string &key)
{
    try
    {
        std::string result;
        ECB_Mode<DES>::Encryption encoder;
        // 这里的key长度必须为8
        encoder.SetKey((const byte *)key.c_str(), key.length());
        StringSource(message, true, new StreamTransformationFilter(encoder, new Base64Encoder(new StringSink(result))));
        return result;
    }
    catch (const CryptoPP::Exception &e)
    {
        KLOG_WARNING("%s.", e.what());
        return std::string();
    }
}

std::string UserConfiguration::desDecrypt(const std::string &message, const std::string &key)
{
    try
    {
        std::string result;
        ECB_Mode<DES>::Decryption decoder;
        decoder.SetKey((const byte *)key.c_str(), key.length());
        StringSource(message, true, new Base64Decoder(new StreamTransformationFilter(decoder, new StringSink(result))));
        return result;
    }
    catch (const CryptoPP::Exception &e)
    {
        KLOG_WARNING("%s.", e.what());
        return std::string();
    }
}
