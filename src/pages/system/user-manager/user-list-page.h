/**
 * @file          src/pages/user/user-manager/user-list-page.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#ifndef USERLISTPAGE_H
#define USERLISTPAGE_H

#include "base/interface/user.h"
#include "page.h"
#include "table-widgets/user-table.h"
#include "user-update-dialog.h"

namespace Ui
{
class UserListPage;
}

class UserUpdateDialog;
class UserListPage : public Page
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
    void getListUserFinished(const QString objId, const QPair<grpc::Status, user::ListUserReply> &);
    void getCreateUserFinished(const QString objId, const QPair<grpc::Status, user::CreateUserReply> &);
    void getUpdateUserFinished(const QString objId, const QPair<grpc::Status, user::UpdateUserReply> &);
    void getRemoveUserFinished(const QString objId, const QPair<grpc::Status, user::RemoveUserReply> &);

private:
    void initButtons();
    void initTable();
    void initConnect();
    UserUpdateDialog *popupDialog(DialogType type, const QString &title);
    void getCheckedItemsId(QList<qint64> &ids);
    bool isSystemUser(const QString &userRole);

private:
    QString m_objID;
    UserUpdateDialog *m_createDialog;
    UserUpdateDialog *m_editDialog;
};

#endif  // USERLISTPAGE_H
