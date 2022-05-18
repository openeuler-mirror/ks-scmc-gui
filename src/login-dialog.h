#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <kylin-license/license-i.h>
#include <QLineEdit>
#include <QSettings>
#include <QWidget>
#include "info-worker.h"
#include "common/license.h"
#include "dbus/dbus-utils.h"
#include "activate-page.h"
#include "error-message-box.h"

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
    void initMessageBox();
    void initAbout();
    void loadConfig();
    bool inspectLoginParam();
    void getLicense(QString license_str);

private slots:
    void onMenuTrigger(QAction *act);
    void onLogin();
    void onLogout();
    void getLoginResult(const QPair<grpc::Status, user::LoginReply> &);
    void getLogoutResult(const QPair<grpc::Status, user::LogoutReply> &);
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
    KiranTitlebarWindow *m_about;
//    QAction *actionLicense;
};

#endif  // LOGIN_DIALOG_H
