#ifndef SERVERCONFIGDIALOG_H
#define SERVERCONFIGDIALOG_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QLineEdit>
#include <QObject>

class ServerConfigDialog : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit ServerConfigDialog(QWidget *parent = nullptr);
    ~ServerConfigDialog();
    QString getServerInfo();
    void setServerInfo();

private:
    void initUI();
    void readConfig(QString key, QString &value);
    void writeConfig(QString key, QString value);

private:
    QLineEdit *m_ipLineEdit;
    QLineEdit *m_portLineEdit;
    QString m_ip;
    QString m_port;
};

#endif  // SERVERCONFIGDIALOG_H
