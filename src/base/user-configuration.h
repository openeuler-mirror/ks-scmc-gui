#ifndef USERCONFIGURATION_H
#define USERCONFIGURATION_H

#include <QSettings>

#define SCMC_DES_KEY "vaksscms"

enum ConfigSettingType
{
    CONFIG_SETTING_TYPE_LOGIN,
    CONFIG_SETTING_TYPE_SERVER
};

class UserConfiguration
{
public:
    UserConfiguration();
    ~UserConfiguration();
    void readConfig(ConfigSettingType type, QString group, QString key, QString &value);
    void writeConfig(ConfigSettingType type, QString group, QString key, QString value);
    static std::string getServerAddr();
    static void setServerAddr(std::string addr);
    static std::string desEncrypt(const std::string &message, const std::string &key = SCMC_DES_KEY);
    static std::string desDecrypt(const std::string &message, const std::string &key = SCMC_DES_KEY);

private:
    QSettings *m_serverSetting;
    QSettings *m_loginSettings;
};

#endif  // USERCONFIGURATION_H
