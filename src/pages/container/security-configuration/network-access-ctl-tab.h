#ifndef NETWORKACCESSCTLTAB_H
#define NETWORKACCESSCTLTAB_H

#include <QListWidget>
#include <QRadioButton>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>
#include "info-worker.h"

class NetworkAccessItem;
class NetworkAccessCtlTab : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkAccessCtlTab(QWidget *parent = nullptr);
    ~NetworkAccessCtlTab();
    void setNetworkAccessInfo(container::SecurityConfig *securityCfg);
    void getNetworkAccessInfo(container::SecurityConfig *securityCfg);

private slots:
    NetworkAccessItem *createItem(int index = -1);
    void addItem();
    void deleteItem();
    void popuoFuncDesc();

private:
    void initUI();

private:
    QListWidget *m_listWidget;
    QWidget *m_funcDesc;
    QRadioButton *m_btnOpen;
    QRadioButton *m_btnClose;
    bool m_isEnable;
};

#endif  // NETWORKACCESSCTLTAB_H
