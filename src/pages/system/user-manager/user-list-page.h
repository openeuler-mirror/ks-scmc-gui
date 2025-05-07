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

protected:
    virtual void timerEvent(QTimerEvent *event);

private slots:
    void search();
    void refresh();
    void createUser();
    void deleteUsers();
    void deleteUser(qint64 userID);
    void editUser(qint64 userID, QString userName, qint64 roleID);
    void getListUserFinished(const QString objId, const QPair<grpc::Status, user::ListUserReply> &);
    void getCreateUserFinished(const QString objId, const QPair<grpc::Status, user::CreateUserReply> &);
    void getUpdateUserFinished(const QString objId, const QPair<grpc::Status, user::UpdateUserReply> &);
    void getRemoveUserFinished(const QString objId, const QPair<grpc::Status, user::RemoveUserReply> &);
    void getListRoleFinished(const QString objId, const QPair<grpc::Status, user::ListRoleReply> &);

private:
    void initUI();
    void initConnect();
    void clearText();
    void setTips(const QString &tips);
    void getRoleList();
    UserUpdateDialog *popupDialog(DialogType type, const QString &title);
    bool isSystemUser(const QString &userRole);
    void updateDeleteBtnState(Qt::CheckState checkState);

private:
    Ui::UserListPage *ui;
    QString m_objID;
    int m_timerID;
    UserUpdateDialog *m_createDialog;
    UserUpdateDialog *m_editDialog;
    QMap<QString, qint64> m_roles;
};

#endif  // USERLISTPAGE_H
