/**
 * @file          src/common/authorization-dialog.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#ifndef AUTHORIZATIONDIALOG_H
#define AUTHORIZATIONDIALOG_H

#include <QDialog>

namespace Ui
{
class AuthorizationDialog;
}

class AuthorizationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AuthorizationDialog(QWidget *parent = nullptr);
    ~AuthorizationDialog();

    void setMsgTitle(const QString &msgTitle);

    void setUserName(const QString &userName);
    QString userName();

    QString password();

private:
    void initUI();
    void authenticate();

private:
    Ui::AuthorizationDialog *ui;
};

#endif  // AUTHORIZATIONDIALOG_H
