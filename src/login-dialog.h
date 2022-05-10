#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QLineEdit>
#include <QSettings>
#include <QWidget>
#include "info-worker.h"

namespace Ui
{
class LoginDialog;
}

class MainWindow;
class ServerConfigDialog;
class LoginDialog : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void initUI();
    void loadConfig();
    bool inspectLoginParam();

private slots:
    void onMenuTrigger(QAction *act);
    void onLogin();
    void onLogout();
    void getLoginResult(const QPair<grpc::Status, user::LoginReply> &);
    void getLogoutResult(const QPair<grpc::Status, user::LogoutReply> &);
    void sessionExpire();

private:
    Ui::LoginDialog *ui;
    MainWindow *m_mainWindow;
    QTimer *m_timer;
    ServerConfigDialog *m_serverCfgDlg;
    QString m_server;
};

#endif  // LOGIN_DIALOG_H
