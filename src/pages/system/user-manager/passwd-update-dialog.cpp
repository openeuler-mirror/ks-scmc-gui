/**
 * @file          /ks-scmc-gui/src/pages/user/passwd-update-dialog.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "passwd-update-dialog.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QTimer>
#include "message-dialog.h"
#include "ui_passwd-update-dialog.h"
#include "utils/password-checker.h"

#define TIMEOUT 300

PasswdUpdateDialog::PasswdUpdateDialog(QString userName, QWidget *parent) : KiranTitlebarWindow(parent),
                                                                            ui(new Ui::PasswdUpdateDialog),
                                                                            m_userName(userName),
                                                                            m_oldTimer(nullptr),
                                                                            m_newTimer(nullptr),
                                                                            m_confirmTimer(nullptr)
{
    ui->setupUi(getWindowContentWidget());
    m_objId = User::generateId(this);
    m_oldTimer = new QTimer(this);
    connect(m_oldTimer, &QTimer::timeout,
            [this]
            {
                QString errorMsg = "";
                auto isValid = PasswordChecker::checkOldPassword(m_userName, ui->lineEdit_old_pw->text(), errorMsg);
                ui->lab_old_pw_tips->setVisible(!isValid);
                ui->lab_old_pw_tips->setText(errorMsg);
                m_oldTimer->stop();
            });
    m_newTimer = new QTimer(this);
    connect(m_newTimer, &QTimer::timeout,
            [this]
            {
                QString errorMsg = "";
                auto isValid = PasswordChecker::checkNewPassword(ui->lineEdit_new_pw->text(), errorMsg);
                ui->lab_new_pw_tips->setVisible(!isValid);
                ui->lab_new_pw_tips->setText(errorMsg);
                m_newTimer->stop();
            });
    m_confirmTimer = new QTimer(this);
    connect(m_confirmTimer, &QTimer::timeout,
            [this]
            {
                QString errorMsg = "";
                auto isValid = PasswordChecker::checkConfirmPassword(ui->lineEdit_new_pw->text(), ui->lineEdit_confirm_pw->text(), errorMsg);
                ui->lab_confirm_pw_tips->setVisible(!isValid);
                ui->lab_confirm_pw_tips->setText(errorMsg);
                m_confirmTimer->stop();
            });
    ui->lab_user_name->setText(userName);
    initUI();
    connect(&User::getInstance(), &User::updatePasswordFinished, this, &PasswdUpdateDialog::getUpdatePasswordResult);
}

PasswdUpdateDialog::~PasswdUpdateDialog()
{
    delete ui;
    if (m_oldTimer)
    {
        m_oldTimer->stop();
        delete m_oldTimer;
        m_oldTimer = nullptr;
    }
    if (m_newTimer)
    {
        m_newTimer->stop();
        delete m_newTimer;
        m_newTimer = nullptr;
    }
    if (m_confirmTimer)
    {
        m_confirmTimer->stop();
        delete m_confirmTimer;
        m_confirmTimer = nullptr;
    }
}

void PasswdUpdateDialog::onConfirm()
{
    QString errorMsg;
    if (PasswordChecker::checkOldPassword(m_userName, ui->lineEdit_old_pw->text(), errorMsg) &&
        PasswordChecker::checkNewPassword(ui->lineEdit_new_pw->text(), errorMsg) &&
        PasswordChecker::checkConfirmPassword(ui->lineEdit_new_pw->text(), ui->lineEdit_confirm_pw->text(), errorMsg))
    {
        updatePassword(ui->lineEdit_old_pw->text(), ui->lineEdit_new_pw->text());
    }
    else
        KLOG_WARNING() << "The password has error";
}

void PasswdUpdateDialog::getUpdatePasswordResult(const QString objId, const QPair<grpc::Status, user::UpdatePasswordReply> reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        KLOG_INFO() << "Update password successful!";
        UserConfiguration::getInstance().writeConfig(CONFIG_SETTING_TYPE_LOGIN, m_userName, PASSWORD, ui->lineEdit_new_pw->text());
        emit sigUpdatePasswdSuccessful();
        close();
    }
    else
    {
        MessageDialog::message(tr("Update Password"),
                               tr("Update password failed!"),
                               tr("error: %1").arg(reply.first.error_message().data()),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void PasswdUpdateDialog::updatePassword(QString oldPw, QString newPw)
{
    User::getInstance().updatePassword(m_objId, oldPw.toStdString(), newPw.toStdString());
}

void PasswdUpdateDialog::initUI()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setIcon(QIcon(":/images/logo.png"));
    setTitle(tr("Change Password"));
    setWindowModality(Qt::ApplicationModal);
    setButtonHints(TitlebarCloseButtonHint);
    setFixedSize(620, 450);
    setResizeable(false);

    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_confirm, Kiran::BUTTON_Default);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_cancel, Kiran::BUTTON_Normal);

    ui->btn_tips->setIcon(QIcon(":/images/tips.svg"));
    ui->btn_tips->setStyleSheet("QToolTip{"
                                "background-color: rgb(255,255,255);"
                                "color:#000000;"
                                "border:0px solid rgb(0,0,0);"
                                "border-radius: 6px;"
                                "outline:none; "
                                "min-height:30px;"
                                "}"
                                "QToolButton{border:none;}");

    // 密码长度8-32位，包含大写字母、小写字母、数字和特殊字符中至少三种组合
    ui->btn_tips->setToolTip(tr("The password length 8-32 bits, should contain\n at least three combinations of uppercase letter,\n lowercase letter, number and special character"));

    ui->lab_old_pw_tips->hide();
    ui->lab_new_pw_tips->hide();
    ui->lab_confirm_pw_tips->hide();

    ui->lineEdit_old_pw->setEchoMode(QLineEdit::Password);
    ui->lineEdit_old_pw->setMaxLength(32);
    ui->lineEdit_new_pw->setEchoMode(QLineEdit::Password);
    ui->lineEdit_new_pw->setMaxLength(32);
    ui->lineEdit_confirm_pw->setEchoMode(QLineEdit::Password);
    ui->lineEdit_confirm_pw->setMaxLength(32);

    connect(ui->lineEdit_old_pw, &QLineEdit::textChanged,
            [=](QString inputStr)
            {
                m_oldTimer->start(TIMEOUT);
            });
    connect(ui->lineEdit_new_pw, &QLineEdit::textChanged,
            [=](QString inputStr)
            {
                m_newTimer->start(TIMEOUT);
            });
    connect(ui->lineEdit_confirm_pw, &QLineEdit::textChanged,
            [=](QString inputStr)
            {
                m_confirmTimer->start(TIMEOUT);
            });

    connect(ui->btn_cancel, &QPushButton::clicked, this, &PasswdUpdateDialog::close);
    connect(ui->btn_confirm, &QPushButton::clicked, this, &PasswdUpdateDialog::onConfirm);
}
