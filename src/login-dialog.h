/**
 * @file          /ks-scmc-gui/src/login-dialog.h
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <ks-license/license-i.h>
#include <QLabel>
#include <QSharedPointer>
#include <QWidget>
#include "common/about-page.h"
#include "info-worker.h"
#include "license/license-activation.h"
#include "license/license-proxy.h"

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
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void initUI();
    void initActivation();
    bool inspectLoginParam();

private slots:
    void onMenuTrigger(QAction *act);
    void onLogin();
    void onLogout();
    void getLoginResult(const QString objID, const QPair<grpc::Status, user::LoginReply> &);
    void getLogoutResult(const QString objID, const QPair<grpc::Status, user::LogoutReply> &);
    void sessionExpire();
    void showActivatePage();
    void showAboutPage();
    void updateActivation();

private:
    Ui::LoginDialog *ui;
    QString m_objID;
    MainWindow *m_mainWindow;
    QTimer *m_timer;
    ServerConfigDialog *m_serverCfgDlg;
    QString m_server;
    LicenseActivation *m_activateDialog;
    QSharedPointer<LicenseProxy> m_licenseProxy;
    QLabel *m_activate_label;
    AboutPage *m_about;
    QMutex m_sessionMutex;
    bool m_isLogin;
    bool m_isSessionExpired;
};

#endif  // LOGIN_DIALOG_H
