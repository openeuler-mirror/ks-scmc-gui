/**
 * @file          src/pages/user/user-manager/user-list-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "user-list-page.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QPushButton>
#include <QStandardItem>

#include "message-dialog.h"

#define USER_ID "user id"
#define USER_ROLE_ID "role id"
#define USER_LOGIN_NAME "user login name"
#define USER_ROLE_NAME "user role name"

#define STDADM_ROLE_ID 4
UserListPage::UserListPage(QWidget *parent) : TablePage(parent),
                                              m_createDialog(nullptr),
                                              m_editDialog(nullptr)
{
    m_objID = User::generateId(this);
    initButtons();
    initTable();
    initConnect();
}

UserListPage::~UserListPage()
{
}

void UserListPage::updateInfo(QString keyword)
{
    clearText();
    User::getInstance().listUser(m_objID);
}

UserUpdateDialog *UserListPage::popupDialog(DialogType type, const QString &title)
{
    UserUpdateDialog *dialog = new UserUpdateDialog(this);
    dialog->setDialogType(type);
    dialog->setTitle(title);
    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    dialog->move(screenGeometry.x() + (screenGeometry.width() - dialog->width()) / 2,
                 screenGeometry.y() + (screenGeometry.height() - dialog->height()) / 2);
    dialog->show();
    return dialog;
}
void UserListPage::getCheckedItemsId(QList<qint64> &ids)
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);

    foreach (auto idMap, info)
    {
        ids.append(idMap.value(USER_ID).toInt());
    }
}
bool UserListPage::isSystemUser(const QString &userRole)
{
    if (0 == QString::compare(userRole, USER_ROLE_SYSADM) ||
        0 == QString::compare(userRole, USER_ROLE_SECADM) ||
        0 == QString::compare(userRole, USER_ROLE_AUDADM))
    {
        return true;
    }
    return false;
}
void UserListPage::createUser()
{
    m_createDialog = popupDialog(DialogType::DIALOG_TYPE_CREATE, tr("Create User"));
    connect(m_createDialog, &UserUpdateDialog::checkPassed, this, [=]()
            {  
        // 调用后台接口创建用户
        auto info = m_createDialog->getUserInfo();

        user::CreateUserRequest req;
        auto userinfo = req.mutable_user_info();
        userinfo->set_login_name(info.userName.toStdString());
        userinfo->set_password(info.password.toStdString());
        userinfo->set_is_active(true);
        userinfo->set_is_editable(true);
        userinfo->set_role_id(STDADM_ROLE_ID);

        User::getInstance().createUser(m_objID, req); });
}

void UserListPage::deleteUsers()
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);
    std::vector<int64_t> userIDs;
    foreach (auto &idMap, info)
    {
        KLOG_INFO() << "Remove user:" << idMap.value(USER_ID).toInt();
        userIDs.push_back(idMap.value(USER_ID).toInt());
    }

    if (!userIDs.empty())
    {
        MessageDialog::StandardButton ret = MessageDialog::message(tr("Remove User"),
                                                                   tr("Are you sure you want to remove the user?"),
                                                                   tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                                                   ":/images/warning.svg",
                                                                   MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
        if (ret == MessageDialog::StandardButton::Yes)
        {
            User::getInstance().removeUser(m_objID, userIDs);
        }
    }
}

void UserListPage::deleteUser(int row)
{
    int64_t id = -1;

    auto item = getItem(row, 1);
    QMap<QString, QVariant> idMap = item->data().value<QMap<QString, QVariant>>();
    id = idMap.value(USER_ID).toInt();

    if (id > 0)
    {
        auto ret = MessageDialog::message(tr("Remove User"),
                                          tr("Are you sure you want to remove the user?"),
                                          tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                          ":/images/warning.svg",
                                          MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
        if (ret == MessageDialog::StandardButton::Yes)
        {
            std::vector<int64_t> ids;
            ids.push_back(id);

            User::getInstance().removeUser(m_objID, ids);
        }
    }
}

void UserListPage::editUser(int row)
{
    auto item = getItem(row, 1);
    QMap<QString, QVariant> idMap = item->data().value<QMap<QString, QVariant>>();
    auto id = idMap.value(USER_ID).toInt();
    auto roleID = idMap.value(USER_ROLE_ID).toInt();
    auto name = idMap.value(USER_LOGIN_NAME).toString();

    UserUpdateDialog::UserInfo info;
    info.userID = id;
    info.userName = name;
    info.roleID = roleID;

    m_editDialog = popupDialog(DialogType::DIALOG_TYPE_EDIT, tr("Edit User"));
    m_editDialog->setUserInfo(info);

    connect(m_editDialog, &UserUpdateDialog::checkPassed, this, [=]() {  // 调用后台接口编辑用户
        auto userInfo = m_editDialog->getUserInfo();

        user::UpdateUserRequest req;
        auto info = req.mutable_user_info();
        info->set_id(userInfo.userID);
        info->set_login_name(userInfo.userName.toStdString());
        info->set_password(userInfo.password.toStdString());
        info->set_role_id(userInfo.roleID);
        User::getInstance().updateUser(m_objID, req);
    });
}

void UserListPage::getListUserFinished(const QString objId, const QPair<grpc::Status, user::ListUserReply> &reply)
{
    if (m_objID != objId)
        return;

    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
    setHeaderCheckable(false);

    if (!reply.first.ok())
    {
        if (reply.first.error_code() == grpc::StatusCode::PERMISSION_DENIED)
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
        else
        {
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
            if (reply.first.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED)
            {
                setTips(tr("Response timeout!"));
            }
        }
        setTableDefaultContent("-");
        return;
    }

    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);

    // 保存选中状态
    QList<qint64> ids;
    getCheckedItemsId(ids);

    clearTable();
    int size = reply.second.users_size();
    if (size <= 0)
    {
        setTableDefaultContent("-");
        return;
    }

    // 添加表格值
    setHeaderCheckable(true);
    int row = 0;
    QMap<QString, QVariant> idMap;
    for (auto user : reply.second.users())
    {
        KLOG_DEBUG() << "Get user list. "
                     << "user id:" << user.id() << "user login name:" << user.login_name().data();

        qint64 id = user.id();
        qint64 roleID = user.role_id();
        QString userRole = user.role_info().name().data();
        QString loginName = user.login_name().data();
        idMap.insert(USER_ID, id);
        idMap.insert(USER_ROLE_ID, roleID);
        idMap.insert(USER_LOGIN_NAME, loginName);
        idMap.insert(USER_ROLE_NAME, userRole);

        auto itemCheck = new QStandardItem();
        itemCheck->setCheckable(false);
        if (!isSystemUser(userRole))
        {
            itemCheck->setCheckable(true);
            // 恢复选中状态
            if (-1 != ids.indexOf(id))
            {
                itemCheck->setCheckState(Qt::Checked);
            }
        }

        auto itemName = new QStandardItem(loginName);
        itemName->setData(QVariant::fromValue(idMap));

        auto itemID = new QStandardItem(QString("%1").arg(id));

        auto itemRole = new QStandardItem(userRole);

        QString createTime = "-";
        if (user.created_at() != 0)
        {
            auto dt = QDateTime::fromSecsSinceEpoch(user.created_at());
            createTime = dt.toString("yyyy/MM/dd hh:mm:ss");
        }
        auto itemTime = new QStandardItem(createTime);

        setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemID << itemRole << itemTime);
        row++;
    }
}

void UserListPage::getCreateUserFinished(const QString objId, const QPair<grpc::Status, user::CreateUserReply> &reply)
{
    if (m_objID != objId)
        return;

    if (reply.first.ok())
    {
        m_createDialog->close();
        updateInfo();
    }
    else
    {
        MessageDialog::message(tr("Create User"),
                               tr("Create user failed!"),
                               tr("Error: ") + reply.first.error_message().data(),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void UserListPage::getUpdateUserFinished(const QString objId, const QPair<grpc::Status, user::UpdateUserReply> &reply)
{
    if (m_objID != objId)
        return;

    if (reply.first.ok())
    {
        m_editDialog->close();
        updateInfo();
    }
    else
    {
        MessageDialog::message(tr("Update User"),
                               tr("Update user failed!"),
                               tr("Error: ") + reply.first.error_message().data(),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void UserListPage::getRemoveUserFinished(const QString objId, const QPair<grpc::Status, user::RemoveUserReply> &reply)
{
    if (m_objID != objId)
        return;

    if (reply.first.ok())
    {
        updateInfo();
    }
    else
    {
        MessageDialog::message(tr("Remove User"),
                               tr("Remove user failed!"),
                               tr("Error: ") + reply.first.error_message().data(),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void UserListPage::initButtons()
{
    auto btnCreate = new QPushButton(this);
    btnCreate->setText(tr("Create"));
    btnCreate->setObjectName("btnCreate");
    btnCreate->setFixedSize(QSize(78, 32));
    Kiran::WidgetPropertyHelper::setButtonType(btnCreate, Kiran::BUTTON_Default);
    connect(btnCreate, &QPushButton::clicked, this, &UserListPage::createUser);

    auto btnDelete = new QPushButton(this);
    btnDelete->setText(tr("Delete"));
    btnDelete->setObjectName("btnDelete");
    btnDelete->setStyleSheet("#btnDelete{background-color:#ff4b4b;"
                             "border:none;"
                             "border-radius: 4px;"
                             "color:#ffffff;}"
                             "#btnDelete:hover{ background-color:#ff6c6c;}"
                             "#btnDelete:focus{outline:none;}"
                             "#btnDelete:disabled{color:#919191;background:#393939;}");
    btnDelete->setFixedSize(QSize(78, 32));
    connect(btnDelete, &QPushButton::clicked, this, &UserListPage::deleteUsers);

    addSingleOperationButton(btnCreate);
    addBatchOperationButtons(QList<QPushButton *>() << btnDelete);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void UserListPage::initTable()
{
    QStringList tableHHeaderDate = {
        "",
        QString(tr("User Name")),
        QString(tr("User ID")),
        QString(tr("Role")),
        QString(tr("Create Time")),
        QString(tr("Quick Actions"))};
    setHeaderSections(tableHHeaderDate);

    setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>{{ACTION_BUTTON_TYPE_USER_EDIT, QPair<QString, QString>{tr("Edit"), tr("Edit")}},
                                                                                                   {ACTION_BUTTON_TYPE_USER_EDIT, QPair<QString, QString>{tr("Delete"), tr("Delete")}}});

    setTableDefaultContent("-");
    setHeaderCheckable(true);
    connect(this, &UserListPage::sigUserEdit, this, &UserListPage::editUser);
    connect(this, &UserListPage::sigUserDelete, this, &UserListPage::deleteUser);
}

void UserListPage::initConnect()
{
}
