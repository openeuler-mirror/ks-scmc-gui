#ifndef PASSWDUPDATEDIALOG_H
#define PASSWDUPDATEDIALOG_H

#include <kiran-titlebar-window.h>
#include <QWidget>
#include "info-worker.h"

enum PasswordType
{
    PASSWORF_TYPE_OLD,
    PASSWORF_TYPE_NEW,
    PASSWORF_TYPE_CONFIRM
};

namespace Ui
{
class PasswdUpdateDialog;
}

class PasswdUpdateDialog : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit PasswdUpdateDialog(QString userName, QWidget *parent = nullptr);
    ~PasswdUpdateDialog();

private:
    void updatePassword(QString oldPw, QString newPw);

private slots:
    bool checkPassword(PasswordType type, QString inputPw);
    void onConfirm();
    void getUpdatePasswordResult(const QPair<grpc::Status, user::UpdatePasswordReply> reply);

signals:
    void sigUpdatePasswdSuccessful();

private:
    void initUI();

private:
    Ui::PasswdUpdateDialog *ui;
    QString m_userName;
    QTimer *m_oldTimer;
    QTimer *m_newTimer;
    QTimer *m_confirmTimer;
};

#endif  // PASSWDUPDATEDIALOG_H
