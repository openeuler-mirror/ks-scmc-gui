#ifndef LOGIN_H
#define LOGIN_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QSettings>
#include <QWidget>
#include "common/info-worker.h"

#define SCMC_DES_KEY "vaksscms"

namespace Ui
{
class Login;
}

class MainWindow;
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
};

#endif  // LOGIN_H
