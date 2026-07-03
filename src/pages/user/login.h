#ifndef LOGIN_H
#define LOGIN_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QLineEdit>
#include <QSettings>
#include <QWidget>
#include "common/info-worker.h"

#define SCMC_DES_KEY "vaksscms"

namespace Ui
{
class Login;
}

class MainWindow;
class ServerConfiguration;
class Login : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void initUI();
    void loadConfig();
    void readConfig(QString key, QString &value);
    void writeConfig(QString key, QString value);
    bool inspectLoginParam();
    static std::string desEncrypt(const std::string &message, const std::string &key = SCMC_DES_KEY);
    static std::string desDecrypt(const std::string &message, const std::string &key = SCMC_DES_KEY);

private slots:
    void getPasswd();
    void onBtnRemember(bool checked);
    void onMenuTrigger(QAction *act);
    void onLogin();
    void onLogout();
    void getLoginResult(const QPair<grpc::Status, user::LoginReply> &);
    void getLogoutResult(const QPair<grpc::Status, user::LogoutReply> &);

private:
    Ui::Login *ui;
    MainWindow *m_mainWindow;
    bool m_isRemember = false;
    QTimer *m_timer;
    QSettings *m_loginSettings;
    ServerConfiguration *m_serverConfig;
    QString m_server;
};

class ServerConfiguration : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit ServerConfiguration(QWidget *parent = nullptr);
    ~ServerConfiguration();
    QString getServerInfo();
    void setServerInfo();

private:
    void initUI();
    void readConfig(QString key, QString &value);
    void writeConfig(QString key, QString value);

private:
    QLineEdit *m_ipLineEdit;
    QLineEdit *m_portLineEdit;
    QSettings *m_serverSetting;
    QString m_ip;
    QString m_port;
};

#endif  // LOGIN_H
