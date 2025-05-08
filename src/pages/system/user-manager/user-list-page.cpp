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
#include <QHBoxLayout>
#include <QPushButton>
#include <QStandardItem>

#include "message-dialog.h"
#include "ui_user-list-page.h"

#define USER_ID "user id"
#define USER_ROLE_ID "role id"
#define USER_LOGIN_NAME "user login name"
#define USER_ROLE_NAME "user role name"

#define STDADM_ROLE_ID 4
UserListPage::UserListPage(QWidget *parent) : Page(parent),
                                              ui(new Ui::UserListPage),
                                              m_createDialog(nullptr),
                                              m_editDialog(nullptr)
{
    ui->setupUi(this);
    m_objID = User::generateId(this);
    getRoleList();
    initUI();
    initConnect();
}

UserListPage::~UserListPage()
{
}

void UserListPage::updateInfo(QString keyword)
{
    // 清除搜索框内字符串
    clearText();
    ui->tableView->searchTextChanged("");

    User::getInstance().listUser(m_objID);
}

void UserListPage::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerID)
    {
        ui->label_tips->clear();
        killTimer(m_timerID);
    }
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

void UserListPage::updateDeleteBtnState(Qt::CheckState checkState)
{
    if (Qt::Unchecked == checkState)
    {
        ui->btn_delete->setEnabled(false);
    }
    else
    {
        ui->btn_delete->setEnabled(true);
    }
}

void UserListPage::search()
{
    auto text = ui->lineEdit_search->text();
    if (text.isEmpty())
    {
        updateInfo();
    }
    else
    {
        ui->tableView->searchTextChanged(text);
    }
}

void UserListPage::refresh()
{
    updateInfo();
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

        if (m_roles.contains(USER_ROLE_STDADM))
        {
            userinfo->set_role_id(m_roles[USER_ROLE_STDADM]);
        }

        User::getInstance().createUser(m_objID, req); });
}

void UserListPage::deleteUsers()
{
    std::vector<int64_t> userIDs;

    // 获取表格中选中的用户信息
    auto selectedUsersInfo = ui->tableView->getSelectedUserInfos();
    if (selectedUsersInfo.size() == 0)
        return;

    foreach (auto info, selectedUsersInfo)
    {
        KLOG_INFO() << "Remove user:" << info.userName << "ID:" << info.userID;
        userIDs.push_back(info.userID);
    }

    if (!userIDs.empty())
    {
        MessageDialog::StandardButton ret = MessageDialog::message(tr("Remove User"),
                                                                   tr("Are you sure you want to remove the users?"),
                                                                   tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                                                   ":/images/warning.svg",
                                                                   MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
        if (ret == MessageDialog::StandardButton::Yes)
        {
            User::getInstance().removeUser(m_objID, userIDs);
        }
    }
}

void UserListPage::deleteUser(qint64 userID)
{
    std::vector<int64_t> ids;
    ids.push_back(userID);

    User::getInstance().removeUser(m_objID, ids);
}

void UserListPage::editUser(qint64 userID, QString userName, qint64 roleID)
{
    UserUpdateDialog::UserInfo info;
    info.userID = userID;
    info.userName = userName;
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

    // TODO:看后续是否需要添加在没有数据情况下无法点击标题栏接口
    // setHeaderCheckable(false);
    ui->tableView->clearTable();

    if (!reply.first.ok())
    {
        if (reply.first.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED)
        {
            setTips(tr("Response timeout!"));
        }
        return;
    }

    // 返回成功后使能创建按钮
    ui->btn_create->setEnabled(true);

    int size = reply.second.users_size();
    if (size <= 0)
    {
        return;
    }

    // TODO:看后续是否需要添加在没有数据情况下无法点击标题栏接口
    // setHeaderCheckable(true);

    QList<UserInfo> userInfos;
    for (auto user : reply.second.users())
    {
        KLOG_DEBUG() << "Get user list. "
                     << "user id:" << user.id() << "user login name:" << user.login_name().data();

        qint64 id = user.id();
        qint64 roleID = user.role_id();
        QString userRole = user.role_info().name().data();
        QString loginName = user.login_name().data();
        QString createTime = "-";
        if (user.created_at() != 0)
        {
            auto dt = QDateTime::fromSecsSinceEpoch(user.created_at());
            createTime = dt.toString("yyyy/MM/dd hh:mm:ss");
        }

        auto userInfo = UserInfo{.selected = false,
                                 .userName = loginName,
                                 .userID = id,
                                 .role = userRole,
                                 .roleID = roleID,
                                 .createTime = createTime};
        userInfos.push_back(userInfo);
    }
    ui->tableView->setUserInfos(userInfos);
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

void UserListPage::getListRoleFinished(const QString objId, const QPair<grpc::Status, user::ListRoleReply> &reply)
{
    if (m_objID != objId)
        return;

    if (reply.first.ok())
    {
        for (auto role : reply.second.roles())
        {
            KLOG_DEBUG() << "Get role list. "
                         << "role id:" << role.id() << "user login name:" << role.name().data();
            m_roles.insert(role.name().data(), role.id());
        }
    }
    else
    {
        KLOG_DEBUG() << "Failed to get role list!";
    }
}

void UserListPage::initUI()
{
    // 在搜索框右侧添加搜索按钮
    QHBoxLayout *layout = new QHBoxLayout(ui->lineEdit_search);
    layout->setMargin(0);
    layout->setContentsMargins(10, 0, 10, 0);

    QPushButton *btn_search = new QPushButton(ui->lineEdit_search);
    btn_search->setObjectName("btn_search");
    btn_search->setFixedSize(QSize(16, 16));
    btn_search->setIcon(QIcon(":/images/search.svg"));
    btn_search->setStyleSheet("#btn_search{background:transparent;border:none;}"
                              "#btn_search:focus{outline:none;}");
    btn_search->setCursor(Qt::PointingHandCursor);
    layout->addStretch();
    layout->addWidget(btn_search);
    ui->lineEdit_search->setTextMargins(0, 0, btn_search->width() + 10, 0);

    // 设置“创建”、“删除”按钮样式
    ui->btn_create->setDisabled(true);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_create, Kiran::BUTTON_Default);

    ui->btn_delete->setDisabled(true);
    ui->btn_delete->setObjectName("btnDelete");
    ui->btn_delete->setStyleSheet("#btnDelete{background-color:#ff4b4b;"
                                  "border:none;"
                                  "border-radius: 4px;"
                                  "color:#ffffff;}"
                                  "#btnDelete:hover{ background-color:#ff6c6c;}"
                                  "#btnDelete:focus{outline:none;}"
                                  "#btnDelete:disabled{color:#919191;background:#393939;}");

    connect(ui->btn_delete, &QPushButton::clicked, this, &UserListPage::deleteUsers);
    connect(ui->btn_create, &QPushButton::clicked, this, &UserListPage::createUser);
    connect(btn_search, &QPushButton::clicked, this, &UserListPage::search);
    connect(ui->lineEdit_search, &QLineEdit::returnPressed, this, &UserListPage::search);
    connect(ui->btn_refresh, &QToolButton::clicked, this, &UserListPage::refresh);

    connect(ui->tableView, &UserTable::userEdited, this, &UserListPage::editUser);
    connect(ui->tableView, &UserTable::userDeleted, this, &UserListPage::deleteUser);
    connect(ui->tableView, &UserTable::checkStateChanged, this, &UserListPage::updateDeleteBtnState);
}

void UserListPage::initConnect()
{
    connect(&User::getInstance(), &User::listUserFinished, this, &UserListPage::getListUserFinished);
    connect(&User::getInstance(), &User::createUserFinished, this, &UserListPage::getCreateUserFinished);
    connect(&User::getInstance(), &User::updateUserFinished, this, &UserListPage::getUpdateUserFinished);
    connect(&User::getInstance(), &User::removeUserFinished, this, &UserListPage::getRemoveUserFinished);

    connect(&User::getInstance(), &User::listRoleFinished, this, &UserListPage::getListRoleFinished);
}

void UserListPage::clearText()
{
    ui->lineEdit_search->clear();
}

void UserListPage::setTips(const QString &tips)
{
    ui->label_tips->setText(tips);
    m_timerID = this->startTimer(10000);  // 10秒后提示消失
}

void UserListPage::getRoleList()
{
    User::getInstance().listRole(m_objID);
}
