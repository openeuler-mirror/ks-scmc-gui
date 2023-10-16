#ifndef USERCONFIGURATION_H
#define USERCONFIGURATION_H

#include <QSettings>

#define SCMC_DES_KEY "vaksscms"
#define PASSWORD "password"
#define USERNAME "username"

enum ConfigSettingType
{
    CONFIG_SETTING_TYPE_LOGIN,
    CONFIG_SETTING_TYPE_SERVER
};

class UserConfiguration
{
public:
    static UserConfiguration &getInstance()
    {
        static UserConfiguration userCfg;
        return userCfg;
    }
    UserConfiguration();
    ~UserConfiguration();
    void readConfig(ConfigSettingType type, QString group, QString key, QString &value);
    void writeConfig(ConfigSettingType type, QString group, QString key, QString value);
    QString getDecryptedPw(QString userName);  //从配置文件中读取用户加密后的密码，解密后返回
    static std::string getServerAddr();
    static void setServerAddr(std::string addr);
    static std::string desEncrypt(const std::string &message, const std::string &key = SCMC_DES_KEY);  //加密
    static std::string desDecrypt(const std::string &message, const std::string &key = SCMC_DES_KEY);  //解密

private:
    QSettings *m_loginSettings;
    QSettings *m_serverSetting;
};

#endif  // USERCONFIGURATION_H
