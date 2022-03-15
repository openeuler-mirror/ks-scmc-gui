#ifndef LOGIN_H
#define LOGIN_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QWidget>
#include "common/info-worker.h"

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

private:
    bool inspectLoginParam();

private slots:
    void onLogin();
    void onLogout();
    void getLoginResult(const QPair<grpc::Status, user::LoginReply> &);
    void getLogoutResult(const QPair<grpc::Status, user::LogoutReply> &);

private:
    Ui::Login *ui;
    MainWindow *m_mainWindow;
};

#endif  // LOGIN_H
