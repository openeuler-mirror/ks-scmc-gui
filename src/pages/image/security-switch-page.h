/**
 * @file          src/pages/image/security-switch-page.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#ifndef SECURITYSWITCHPAGE_H
#define SECURITYSWITCHPAGE_H

#include <QWidget>
#include "base/interface/system.h"
#include "page.h"
class KiranSwitchButton;
class SecuritySwitchPage : public Page
{
    Q_OBJECT
public:
    explicit SecuritySwitchPage(QWidget *parent = nullptr);
    void updateInfo(QString keyword = "");

private slots:
    void getSecuritySwitchResult(const QString objId, const QPair<grpc::Status, sys::GetSecuritySwitchReply> &reply);
    void setSecuritySwitchResult(const QString objId, const QPair<grpc::Status, sys::SetSecuritySwitchReply> &reply);

private:
    void initUI();
    void setStatus(bool);

signals:

private:
    KiranSwitchButton *m_checkbox;
    QString m_objId;
};

#endif  // SECURITYSWITCHPAGE_H
