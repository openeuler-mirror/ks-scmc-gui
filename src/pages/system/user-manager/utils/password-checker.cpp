/**
 * @file          src/pages/system/user-manager/password-checker.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "password-checker.h"
#include <kiran-log/qt5-log-i.h>
#include <QRegExp>
#include "base/user-configuration.h"
bool PasswordChecker::checkOldPassword(const QString& userName, const QString& password, QString& errorMessage)
{
    QString oldPw;
    UserConfiguration::getInstance().readConfig(CONFIG_SETTING_TYPE_LOGIN, userName, PASSWORD, oldPw);
    if (!oldPw.isEmpty())
    {
        if (0 == QString::compare(password, oldPw, Qt::CaseSensitive))  // same
        {
            KLOG_DEBUG() << "Old password input correct!";
            return true;
        }
        else
        {
            errorMessage = tr("Old password input error!");
        }
    }
    else
        errorMessage = tr("There is no password in ks-scmc-user.ini");
    return false;
}

bool PasswordChecker::checkNewPassword(const QString& password, QString& errorMessage)
{
    if (password.length() < 8)
        errorMessage = tr("Please input at least 8 characters");
    else if (password.contains(QRegExp("[\\x4e00-\\x9fa5]+")))
        errorMessage = tr("Can't input Chinese");
    else
    {
        QStringList list;
        list << "[A-Z]"
             << "[a-z]"
             << "[0-9]"
             << "[^0-9A-Za-z]";
        int complex = 0;
        for (auto rx : list)
        {
            complex = password.contains(QRegExp(rx)) ? complex + 1 : complex;
            if (complex >= 3)
            {
                KLOG_DEBUG() << "New password input correct!";
                return true;
            }
        }
        // 请输入包含大写字母、小写字母、数字和特殊字符中至少三种组合
        errorMessage = tr("Please input at least three combinations of\n uppercase letter, lowercase letter, number and special character");
    }
    return false;
}

bool PasswordChecker::checkConfirmPassword(const QString& newPassword, const QString& confirmPassword, QString& errorMessage)
{
    if (0 == QString::compare(newPassword, confirmPassword))
    {
        KLOG_DEBUG() << "Confirm password input correct!";
        return true;
    }
    else
    {
        errorMessage = tr("The confirmation password is not match with the new password");
    }
    return false;
}
