/**
 * @file          src/pages/system/user-manager/password-checker.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma once

#include <QObject>

class PasswordChecker : public QObject
{
    Q_OBJECT
public:
    static bool checkOldPassword(const QString& userName, const QString& password, QString& errorMessage);
    static bool checkNewPassword(const QString& password, QString& errorMessage);
    static bool checkConfirmPassword(const QString& newPassword, const QString& confirmPassword, QString& errorMessage);
};