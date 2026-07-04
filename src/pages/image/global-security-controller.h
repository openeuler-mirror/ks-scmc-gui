#ifndef GLOBALSECURITYCONTROLLER_H
#define GLOBALSECURITYCONTROLLER_H

#include <QWidget>
#include "page.h"

class KiranSwitchButton;
class GlobalSecurityController : public Page
{
    Q_OBJECT
public:
    explicit GlobalSecurityController(QWidget *parent = nullptr);
    void updateInfo(QString keyword = "");

private slots:
    void setApprovable(bool);

private:
    void initUI();
    void setStatus(bool);

signals:

private:
    KiranSwitchButton *m_checkbox;
};

#endif  // GLOBALSECURITYCONTROLLER_H
