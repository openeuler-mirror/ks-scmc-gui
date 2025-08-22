/**
 * @file          /ks-scmc-gui/src/pages/user/server-config-dialog.h
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef SERVERCONFIGDIALOG_H
#define SERVERCONFIGDIALOG_H

#include <kiran-titlebar-window.h>
#include <QObject>

namespace Ui
{
class ServerConfigDialog;
}

class ServerConfigDialog : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit ServerConfigDialog(QWidget *parent = nullptr);
    ~ServerConfigDialog();
    QString getServerInfo();
    void setServerInfo();

private slots:
    void confirm();
    void switchPage();

private:
    void initUI();
    void readConfig(QString key, QString &value);
    void writeConfig(QString key, QString value);
    bool handleIpConfig();
    bool handleDomainConfig();

private:
    Ui::ServerConfigDialog *ui;
    QString m_ip;
    QString m_domain;
    QString m_port;
};

#endif  // SERVERCONFIGDIALOG_H
