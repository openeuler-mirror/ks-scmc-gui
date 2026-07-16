/**
 * @file          src/pages/user/user-manager/user-list-page.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#ifndef USERLISTPAGE_H
#define USERLISTPAGE_H

#include "base/interface/user.h"
#include "table-page.h"
#include "user-update-dialog.h"

class UserUpdateDialog;
class UserListPage : public TablePage
{
    Q_OBJECT
public:
    explicit UserListPage(QWidget *parent = nullptr);
    ~UserListPage();
    void updateInfo(QString keyword = "");  // 刷新表格

private slots:
    void createUser();
    void deleteUsers();
    void deleteUser(int row);
    void editUser(int row);

private:
    void initButtons();
    void initTable();
    void initConnect();
    UserUpdateDialog *popupDialog(DialogType type, const QString &title);
};

#endif  // USERLISTPAGE_H
