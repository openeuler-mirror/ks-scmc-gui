#ifndef NETWORKACCESSCTLTAB_H
#define NETWORKACCESSCTLTAB_H

#include <QListWidget>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>
class NetworkAccessItem;
class NetworkAccessCtlTab : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkAccessCtlTab(QWidget *parent = nullptr);
    ~NetworkAccessCtlTab();
    void setNetworkAccessInfo();
    void getNetworkAccessInfo();

private slots:
    NetworkAccessItem *createItem(int index = -1);
    void addItem();
    void deleteItem();
    void popuoFuncDesc();

private:
    void initUI();

private:
    QVBoxLayout *m_itemLayout;
    QListWidget *m_listWidget;
    QWidget *m_funcDesc;
};

#endif  // NETWORKACCESSCTLTAB_H
