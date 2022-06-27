/**
 * @file          /ks-scmc-gui/src/pages/container/security-configuration/security-list-tab.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef SECURITYLISTTAB_H
#define SECURITYLISTTAB_H

#include <info-worker.h>
#include <QLabel>
#include <QListWidget>
#include <QRadioButton>
#include <QTextBrowser>
#include <QWidget>
enum ProtectionType
{
    NONE = 0,
    EXEC_WHITELIST = 1,      // 进程白名单
    NET_EXEC_WHITELIST = 2,  // 网络进程白名单
    PROTECT_FILE_LIST        // 保护文件列表
};

class SecurityListItem;
class SecurityListTab : public QWidget
{
    Q_OBJECT
public:
    explicit SecurityListTab(ProtectionType type, QWidget *parent = nullptr);
    ~SecurityListTab();
    void setSecurityListInfo(container::SecurityConfig *securityCfg);
    bool getSecurityListInfo(container::SecurityConfig *securityCfg);

private:
    SecurityListItem *createItem(int index);
    void addItem();
    void deleteItem();
    void updateItemName();
    void popuoFuncDesc();

private:
    void initUI();

private:
    QListWidget *m_listWidget;
    QWidget *m_funcDesc;
    QRadioButton *m_btnOpen;
    QRadioButton *m_btnClose;
    ProtectionType m_type;
    bool m_isEnable;
};

#endif  // SECURITYLISTTAB_H
