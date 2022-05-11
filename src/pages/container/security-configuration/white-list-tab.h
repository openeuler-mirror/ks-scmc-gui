#ifndef WHITELISTTAB_H
#define WHITELISTTAB_H

#include <info-worker.h>
#include <QListWidget>
#include <QWidget>

enum PROC_PROTECTION
{
    NONE = 0,
    EXEC_WHITELIST = 1,  // 进程白名单
    NET_WHITELIST = 2    // 网络进程白名单
};

class WhiteListItem;
class WhiteListTab : public QWidget
{
    Q_OBJECT
public:
    explicit WhiteListTab(PROC_PROTECTION type, QWidget *parent = nullptr);
    ~WhiteListTab();
    void setWhiteListInfo();
    void getWhiteListInfo();

private:
    WhiteListItem *createItem(int index = -1);
    void addItem();
    void deleteItem();
    void popuoFuncDesc();

private:
    void initUI();

private:
    QListWidget *m_listWidget;
    QWidget *m_funcDesc;
    PROC_PROTECTION m_type;
};

#endif  // WHITELISTTAB_H
