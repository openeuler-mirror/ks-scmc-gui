/**
 * @file          src/pages/container/security-configuration/security-level-tab.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#ifndef SECURITYLEVELTAB_H
#define SECURITYLEVELTAB_H

#include <QMap>
#include <QWidget>

#include "def.h"

namespace Ui
{
class SecurityLevelTab;
}

class SecurityLevelTab : public QWidget
{
    Q_OBJECT

public:
    explicit SecurityLevelTab(QWidget *parent = nullptr);
    ~SecurityLevelTab();

    void setSecurityInfo(int level, bool networkAccessCtl);
    void getSecurityInfo(int &level, bool &networkAccessCtl);

signals:
    void levelChanged(int level);

private:
    void initUI();

private:
    Ui::SecurityLevelTab *ui;
    QMap<int, QPair<QString, QString>> m_securityLevelMap;
};

#endif  // SECURITYLEVELTAB_H
