/**
 * @file          src/common/authorization-dialog.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "authorization-dialog.h"
#include <QPaintEvent>
#include <QPainter>
#include "ui_authorization-dialog.h"
#include "user-configuration.h"

AuthorizationDialog::AuthorizationDialog(QWidget *parent) : QDialog(parent),
                                                            ui(new Ui::AuthorizationDialog)
{
    ui->setupUi(this);
    initUI();
}

AuthorizationDialog::~AuthorizationDialog()
{
    delete ui;
}

void AuthorizationDialog::setMsgTitle(const QString &msgTitle)
{
    ui->label_title->setText(msgTitle);
}

void AuthorizationDialog::setUserName(const QString &userName)
{
    ui->lineEdit_name->setText(userName);
}

QString AuthorizationDialog::userName()
{
    return ui->lineEdit_name->text();
}

QString AuthorizationDialog::password()
{
    return ui->lineEdit_password->text();
}

void AuthorizationDialog::initUI()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Authorization"));

    ui->lineEdit_name->setDisabled(true);
    ui->lineEdit_name->setTextMargins(10, 0, 0, 0);
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_password->setMaxLength(32);
    ui->lineEdit_password->setTextMargins(10, 0, 0, 0);

    connect(ui->btn_confirm, &QPushButton::clicked, [this]()
            { authenticate(); });
    connect(ui->btn_cancel, &QPushButton::clicked, this, &AuthorizationDialog::close);
}

void AuthorizationDialog::authenticate()
{
    QString realPassword;
    QString currentUser;
    UserConfiguration::getInstance().readConfig(CONFIG_SETTING_TYPE_LOGIN, GROUP_USER, CURRENT_USER, currentUser);
    UserConfiguration::getInstance().readConfig(CONFIG_SETTING_TYPE_LOGIN, currentUser, PASSWORD, realPassword);

    auto password = ui->lineEdit_password->text();
    if (!password.compare(realPassword))
    {
        done(QDialog::Accepted);
    }
    else
    {
        ui->label_tip->setText(tr("Password error!"));
    }
}