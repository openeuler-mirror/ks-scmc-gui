#ifndef SECURITYSWITCHPAGE_H
#define SECURITYSWITCHPAGE_H

#include <QWidget>
#include "info-worker.h"
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
