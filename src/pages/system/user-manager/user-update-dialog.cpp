/**
 * @file          src/pages/system/user-manager/user-update-dialog.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "user-update-dialog.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QLineEdit>
#include <QRegularExpression>
#include "ui_user-update-dialog.h"
#include "utils/password-checker.h"

UserUpdateDialog::UserUpdateDialog(QWidget *parent) : KiranTitlebarWindow(parent),
                                                      ui(new Ui::UserUpdateDialog)
{
    ui->setupUi(getWindowContentWidget());
    initUI();
}

UserUpdateDialog::~UserUpdateDialog()
{
    delete ui;
}

void UserUpdateDialog::setDialogType(DialogType type)
{
    m_type = type;
    ui->stackedWidget->setCurrentIndex(m_type);
}

void UserUpdateDialog::setUserInfo(UserUpdateDialog::UserInfo info)
{
    m_userID = info.userID;
    m_roleID = info.roleID;
    m_userName = info.userName;
    m_password = info.password;
    ui->lineEdit_new_name->setText(info.userName);
}

UserUpdateDialog::UserInfo UserUpdateDialog::getUserInfo()
{
    UserUpdateDialog::UserInfo info;

    info.userID = m_userID;
    info.roleID = m_roleID;
    info.userName = m_userName;
    info.password = m_password;

    return info;
}

void UserUpdateDialog::checkPw()
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());
    QString errMsg;
    if (lineEdit == ui->lineEdit_pw)
    {
        PasswordChecker::checkNewPassword(ui->lineEdit_pw->text(), errMsg);
        ui->tip_pw->setText(errMsg);
    }
    else if (lineEdit == ui->lineEdit_confirm_pw)
    {
        PasswordChecker::checkConfirmPassword(ui->lineEdit_pw->text(), ui->lineEdit_confirm_pw->text(), errMsg);
        ui->tip_confirm_pw->setText(errMsg);
    }
    else if (lineEdit == ui->lineEdit_new_pw)
    {
        PasswordChecker::checkNewPassword(ui->lineEdit_new_pw->text(), errMsg);
        ui->tip_new_pw->setText(errMsg);
    }
    else if (lineEdit == ui->lineEdit_confirm_new_pw)
    {
        PasswordChecker::checkConfirmPassword(ui->lineEdit_new_pw->text(), ui->lineEdit_confirm_new_pw->text(), errMsg);
        ui->tip_confirm_new_pw->setText(errMsg);
    }
}

void UserUpdateDialog::confirm()
{
    QString errMsg;
    switch (m_type)
    {
    case DIALOG_TYPE_CREATE:
    {
        if (!checkUserName(ui->lineEdit_name->text()))
        {
            ui->tip_name->setText(tr("The username supports combinations of letters and numbers."));
            return;
        }
        ui->tip_name->clear();

        if (!PasswordChecker::checkNewPassword(ui->lineEdit_pw->text(), errMsg) ||
            !PasswordChecker::checkConfirmPassword(ui->lineEdit_pw->text(), ui->lineEdit_confirm_pw->text(), errMsg))
        {
            KLOG_DEBUG() << "Invalid password of" << ui->lineEdit_name->text();
            return;
        }
        emit checkPassed();
        break;
    }
    case DIALOG_TYPE_EDIT:
    {
        if (!PasswordChecker::checkNewPassword(ui->lineEdit_new_pw->text(), errMsg) ||
            !PasswordChecker::checkConfirmPassword(ui->lineEdit_new_pw->text(), ui->lineEdit_confirm_new_pw->text(), errMsg))
        {
            KLOG_DEBUG() << "Invalid password of" << ui->lineEdit_new_name->text();
            return;
        }
        emit checkPassed();
        break;
    }
    default:
        break;
    }
}

void UserUpdateDialog::initUI()
{
    setWindowModality(Qt::ApplicationModal);
    setButtonHints(TitlebarCloseButtonHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(QSize(600, 400));

    ui->stackedWidget->setCurrentIndex(m_type);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_confirm, Kiran::BUTTON_Default);

    // 创建界面
    ui->lineEdit_name->setMaxLength(18);
    ui->lineEdit_name->setPlaceholderText(tr("Please input 2 to 18 characters"));
    ui->btn_tip_name->setToolTip(tr("The username supports combinations of letters and numbers."));
    ui->btn_tip_name->setIcon(QIcon(":/images/tips.svg"));

    ui->lineEdit_confirm_pw->setMaxLength(32);
    ui->lineEdit_confirm_pw->setEchoMode(QLineEdit::Password);
    ui->lineEdit_confirm_pw->setPlaceholderText(tr("Please input 8 to 32 characters"));

    ui->lineEdit_pw->setMaxLength(32);
    ui->lineEdit_pw->setPlaceholderText(tr("Please input 8 to 32 characters"));
    ui->lineEdit_pw->setEchoMode(QLineEdit::Password);
    ui->btn_tip_pw->setToolTip(tr("The password length 8-32 bits, should contain\n at least three combinations of uppercase letter,\n lowercase letter, number and special character"));
    ui->btn_tip_pw->setIcon(QIcon(":/images/tips.svg"));

    // 编辑界面
    ui->lineEdit_confirm_new_pw->setMaxLength(32);
    ui->lineEdit_confirm_new_pw->setEchoMode(QLineEdit::Password);
    ui->lineEdit_confirm_new_pw->setPlaceholderText(tr("Please input 8 to 32 characters"));

    ui->lineEdit_new_pw->setMaxLength(32);
    ui->lineEdit_new_pw->setPlaceholderText(tr("Please input 8 to 32 characters"));
    ui->lineEdit_new_pw->setEchoMode(QLineEdit::Password);
    ui->btn_tip_new_pw->setToolTip(tr("The password length 8-32 bits, should contain\n at least three combinations of uppercase letter,\n lowercase letter, number and special character"));
    ui->btn_tip_new_pw->setIcon(QIcon(":/images/tips.svg"));

    connect(ui->btn_cancel, &QPushButton::clicked, this, &UserUpdateDialog::close);
    connect(ui->btn_confirm, &QPushButton::clicked, this, &UserUpdateDialog::confirm);
    connect(ui->lineEdit_pw, &QLineEdit::editingFinished, this, &UserUpdateDialog::checkPw);
    connect(ui->lineEdit_confirm_pw, &QLineEdit::editingFinished, this, &UserUpdateDialog::checkPw);
    connect(ui->lineEdit_new_pw, &QLineEdit::editingFinished, this, &UserUpdateDialog::checkPw);
    connect(ui->lineEdit_confirm_new_pw, &QLineEdit::editingFinished, this, &UserUpdateDialog::checkPw);
}

bool UserUpdateDialog::checkUserName(const QString &userName)
{
    QRegularExpression regex("^[A-Za-z0-9]{2,18}$");
    return regex.match(userName).hasMatch();
}
