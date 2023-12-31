/**
 * @file          /ks-scmc-gui/src/login-dialog.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <kylin-license/license-i.h>
#include <QLineEdit>
#include <QSettings>
#include <QWidget>
#include "activate-page.h"
#include "common/about-page.h"
#include "common/license.h"
#include "dbus/dbus-utils.h"
#include "error-message-box.h"
#include "info-worker.h"

namespace Ui
{
class LoginDialog;
}

class MainWindow;
class ServerConfigDialog;
class SubscribeThread;
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
    void initMessageBox();
    void createSubscribThread();
    void loadConfig();
    bool inspectLoginParam();
    void getLicense(QString license_str);

private slots:
    void onMenuTrigger(QAction *act);
    void onLogin();
    void onLogout();
    void getLoginResult(const QString objID, const QPair<grpc::Status, user::LoginReply> &);
    void getLogoutResult(const QString objID, const QPair<grpc::Status, user::LogoutReply> &);
    void sessionExpire();
    void updateLicense(bool);
    void activation(QString);
    void showActivatePage();
    void showErrorBox();
    void actionAboutClicked();

signals:
    void sigLicenseChange();

private:
    Ui::LoginDialog *ui;
    QString m_objID;
    MainWindow *m_mainWindow;
    QTimer *m_timer;
    ServerConfigDialog *m_serverCfgDlg;
    QString m_server;
    ActivatePage *m_activate_page;
    License *m_license;
    DBusUtils *m_dbusutil;
    ErrorMessageBox *m_errorMessageBox;
    KiranMessageBox *m_activate_page_box;
    KiranMessageBox *m_dbusErrorBox;
    QLabel *activate_label;
    AboutPage *m_about;
    QMutex m_sessionMutex;
    QThread *m_thread;
    SubscribeThread *m_subscribeThread;
};

#endif  // LOGIN_DIALOG_H
