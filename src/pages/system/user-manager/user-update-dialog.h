/**
 * @file          src/pages/system/user-manager/user-update-dialog.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#ifndef USERUPDATEDIALOG_H
#define USERUPDATEDIALOG_H

#include <kiran-titlebar-window.h>
#include <QWidget>

namespace Ui
{
class UserUpdateDialog;
}

enum DialogType
{
    DIALOG_TYPE_CREATE = 0,
    DIALOG_TYPE_EDIT
};

class UserUpdateDialog : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit UserUpdateDialog(QWidget *parent = nullptr);
    ~UserUpdateDialog();
    void setDialogType(DialogType type);
    void setUserInfo(int userID, const QString &userName);

private slots:
    void confirm();
    void checkPw();

signals:
    void checkPassed();

private:
    void initUI();
    bool checkUserName(const QString &userName);

private:
    Ui::UserUpdateDialog *ui;
    DialogType m_type = DIALOG_TYPE_CREATE;
    int m_userID = -1;
};

#endif  // USERUPDATEDIALOG_H
