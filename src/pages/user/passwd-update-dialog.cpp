#include "passwd-update-dialog.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QTimer>
#include "message-dialog.h"
#include "ui_passwd-update-dialog.h"
#include "user-configuration.h"

#define TIMEOUT 300

PasswdUpdateDialog::PasswdUpdateDialog(QString userName, QWidget *parent) : KiranTitlebarWindow(parent),
                                                                            ui(new Ui::PasswdUpdateDialog),
                                                                            m_userName(userName),
                                                                            m_oldTimer(nullptr),
                                                                            m_newTimer(nullptr),
                                                                            m_confirmTimer(nullptr)
{
    ui->setupUi(getWindowContentWidget());
    m_oldTimer = new QTimer(this);
    connect(m_oldTimer, &QTimer::timeout,
            [this] {
                checkPassword(PASSWORF_TYPE_OLD, ui->lineEdit_old_pw->text());
                m_oldTimer->stop();
            });
    m_newTimer = new QTimer(this);
    connect(m_newTimer, &QTimer::timeout,
            [this] {
                checkPassword(PASSWORF_TYPE_NEW, ui->lineEdit_new_pw->text());
                m_newTimer->stop();
            });
    m_confirmTimer = new QTimer(this);
    connect(m_confirmTimer, &QTimer::timeout,
            [this] {
                checkPassword(PASSWORF_TYPE_CONFIRM, ui->lineEdit_confirm_pw->text());
                m_confirmTimer->stop();
            });
    ui->lab_user_name->setText(userName);
    initUI();
    connect(&InfoWorker::getInstance(), &InfoWorker::updatePasswordFinished, this, &PasswdUpdateDialog::getUpdatePasswordResult);
}

PasswdUpdateDialog::~PasswdUpdateDialog()
{
    delete ui;
    if (m_oldTimer)
    {
        delete m_oldTimer;
        m_oldTimer = nullptr;
    }
    if (m_newTimer)
    {
        delete m_newTimer;
        m_newTimer = nullptr;
    }
    if (m_confirmTimer)
    {
        delete m_confirmTimer;
        m_confirmTimer = nullptr;
    }
}

bool PasswdUpdateDialog::checkPassword(PasswordType type, QString inputPw)
{
    QString errorMsg;
    switch (type)
    {
    case PASSWORF_TYPE_OLD:
    {
        QString oldPw;
        UserConfiguration::getInstance().readConfig(CONFIG_SETTING_TYPE_LOGIN, m_userName, PASSWORD, oldPw);
        if (!oldPw.isEmpty())
        {
            if (0 == QString::compare(inputPw, oldPw, Qt::CaseSensitive))  //same
            {
                KLOG_INFO() << "old password input ok!";
                ui->lab_old_pw_tips->clear();
                ui->lab_old_pw_tips->hide();
                return true;
            }
            else
            {
                errorMsg = tr("old password input error");
                KLOG_INFO() << errorMsg;
                ui->lab_old_pw_tips->show();
                ui->lab_old_pw_tips->setText(errorMsg);
            }
        }
        else
            KLOG_INFO() << "There is no password in ks-scmc-user.ini";
        break;
    }
    case PASSWORF_TYPE_NEW:
    {
        QRegExp rx1(QString("^(?=.*[0-9])(?=.*[a-zA-Z])(.{8,})$"));
        if (inputPw.length() < 8)
            errorMsg = tr("Please input at least 8 characters");
        else if (inputPw.contains(QRegExp("[\\x4e00-\\x9fa5]+")))
            errorMsg = tr("Can't input Chinese");
        else if ((!rx1.exactMatch(inputPw)))  //没有包含数字与字符
        {
            errorMsg = tr("Please input at least a combination of password and number");
        }
        else
        {
            KLOG_INFO() << "new password input ok!";
            ui->lab_new_pw_tips->clear();
            ui->lab_new_pw_tips->hide();
            return true;
        }
        ui->lab_new_pw_tips->show();
        ui->lab_new_pw_tips->setText(errorMsg);
        KLOG_INFO() << errorMsg;
        break;
    }

    case PASSWORF_TYPE_CONFIRM:
    {
        QString newPw = ui->lineEdit_new_pw->text();
        if (0 == QString::compare(newPw, inputPw))
        {
            KLOG_INFO() << "confirm password input ok!";
            ui->lab_confirm_pw_tips->clear();
            ui->lab_confirm_pw_tips->hide();
            return true;
        }
        else
        {
            errorMsg = tr("The confirmation password is not match with the new password");
            KLOG_INFO() << errorMsg;
            ui->lab_confirm_pw_tips->show();
            ui->lab_confirm_pw_tips->setText(errorMsg);
        }
        break;
    }
    default:
        break;
    }
    return false;
}

void PasswdUpdateDialog::onConfirm()
{
    if (checkPassword(PASSWORF_TYPE_OLD, ui->lineEdit_old_pw->text()) &&
        checkPassword(PASSWORF_TYPE_NEW, ui->lineEdit_new_pw->text()) &&
        checkPassword(PASSWORF_TYPE_CONFIRM, ui->lineEdit_confirm_pw->text()))
    {
        updatePassword(ui->lineEdit_old_pw->text(), ui->lineEdit_new_pw->text());
    }
    else
        KLOG_INFO() << "password arg error";
}

void PasswdUpdateDialog::getUpdatePasswordResult(const QPair<grpc::Status, user::UpdatePasswordReply> reply)
{
    KLOG_INFO() << "getUpdatePasswordResult";
    if (reply.first.ok())
    {
        KLOG_INFO() << "Update password successful!";
        UserConfiguration::getInstance().writeConfig(CONFIG_SETTING_TYPE_LOGIN, m_userName, PASSWORD, ui->lineEdit_new_pw->text());
        emit sigUpdatePasswdSuccessful();
        close();
    }
    else
    {
        KLOG_INFO() << reply.first.error_message().data();
        MessageDialog::message(tr("Update Password"),
                               tr("Update password failed!"),
                               tr("error: %1").arg(reply.first.error_message().data()),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void PasswdUpdateDialog::updatePassword(QString oldPw, QString newPw)
{
    KLOG_INFO() << "updatePassword";
    InfoWorker::getInstance().updatePassword(oldPw.toStdString(), newPw.toStdString());
}

void PasswdUpdateDialog::initUI()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setIcon(QIcon(":/images/logo.png"));
    setTitle(tr("Change Password"));
    setWindowModality(Qt::ApplicationModal);
    setButtonHints(TitlebarCloseButtonHint);
    setFixedSize(640, 420);

    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_confirm, Kiran::BUTTON_Default);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_cancel, Kiran::BUTTON_Normal);

    //    ui->btn_tips->setToolTip();
    ui->btn_tips->setStyleSheet("QToolTip{"
                                "background-color: rgb(255,255,255);"
                                "color:#000000;"
                                "border:0px solid rgb(0,0,0);"
                                "border-radius: 6px;"
                                "outline:none; "
                                "min-height:30px;"
                                "}"
                                "QToolButton{border:none;}");
    ui->btn_tips->setToolTip(tr("The password should contain at least\n a combination of password and number,\n with a length range of 8-32 characters"));

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
            [=](QString inputStr) {
                m_oldTimer->start(TIMEOUT);
            });
    connect(ui->lineEdit_new_pw, &QLineEdit::textChanged,
            [=](QString inputStr) {
                m_newTimer->start(TIMEOUT);
            });
    connect(ui->lineEdit_confirm_pw, &QLineEdit::textChanged,
            [=](QString inputStr) {
                m_confirmTimer->start(TIMEOUT);
            });

    connect(ui->btn_cancel, &QPushButton::clicked,
            [this] { close(); });
    connect(ui->btn_confirm, &QPushButton::clicked, this, &PasswdUpdateDialog::onConfirm);
}
