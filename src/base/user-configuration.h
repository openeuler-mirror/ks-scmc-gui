/**
 * @file          /ks-scmc-gui/src/base/user-configuration.h
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef USERCONFIGURATION_H
#define USERCONFIGURATION_H

#include <QSettings>

#define GROUP_CURRENT_USER "CurrentUser"
#define SCMC_DES_KEY "vaksscms"
#define PASSWORD "password"
#define USERNAME "username"
#define USERROLE "userrole"

#define SERVER_GROUP "Server"
#define SERVER_ADDRESS "Address"

enum ConfigSettingType
{
    CONFIG_SETTING_TYPE_LOGIN,
    CONFIG_SETTING_TYPE_SERVER
};

class UserConfiguration : public QObject
{
    Q_OBJECT
public:
    static UserConfiguration &getInstance()
    {
        static UserConfiguration userCfg;
        return userCfg;
    }
    UserConfiguration(QObject *parent = nullptr);
    ~UserConfiguration();

    // 读写配置
    void readConfig(ConfigSettingType type, QString group, QString key, QString &value);
    void writeConfig(ConfigSettingType type, QString group, QString key, QString value);

    // 服务器配置
    static std::string getServerAddr();
    static void setServerAddr(const std::string &addr);

    // 密码处理
    static std::string desEncrypt(const std::string &message, const std::string &key = SCMC_DES_KEY);  // 加密
    static std::string desDecrypt(const std::string &message, const std::string &key = SCMC_DES_KEY);  // 解密
    QString getDecryptedPw(QString userName);                                                          // 从配置文件中读取用户加密后的密码，解密后返回

private:
    QSettings *m_loginSettings;
    QSettings *m_serverSetting;
};

#endif  // USERCONFIGURATION_H
