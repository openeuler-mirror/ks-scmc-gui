/**
 * @file          src/pages/user/user-manager/user-list-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "user-list-page.h"
#include <widget-property-helper.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QPushButton>

UserListPage::UserListPage(QWidget *parent) : TablePage(parent)
{
    initButtons();
    initTable();
    initConnect();
}

UserListPage::~UserListPage()
{
}

void UserListPage::updateInfo(QString keyword)
{
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
void UserListPage::createUser()
{
    auto dialog = popupDialog(DialogType::DIALOG_TYPE_CREATE, tr("Create User"));
    connect(dialog, &UserUpdateDialog::checkPassed, this, [=]() {  // 调用后台接口创建用户
    });
}

void UserListPage::deleteUsers()
{
}

void UserListPage::deleteUser(int row)
{
}

void UserListPage::editUser(int row)
{
    auto dialog = popupDialog(DialogType::DIALOG_TYPE_CREATE, tr("Edit User"));
    connect(dialog, &UserUpdateDialog::checkPassed, this, [=]() {  // 调用后台接口编辑用户
    });
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
    // TODO:只是测试，后续改为false
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
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
