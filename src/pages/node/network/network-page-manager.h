#ifndef NETWORKPAGEMANAGER_H
#define NETWORKPAGEMANAGER_H

#include <QStackedWidget>
#include <QWidget>
#include "page.h"

class TablePage;
class NetworkListPage;
class QComboBox;
class NetworkPageManager : public Page
{
    Q_OBJECT
public:
    NetworkPageManager(QWidget *parent = nullptr);
    void updateInfo(QString keyword = "");
    void setNodeId(int64_t nodeId);

private slots:
    void changeTable(int index);

private:
    void initUI();
    void initConnect();
    void createPhysicalNetTable();
    void createVirtNetTable();

private:
    QStackedWidget *m_stackedWidget;
    QComboBox *m_cbNetwork;
    NetworkListPage *m_physicalPage;
    NetworkListPage *m_virtPage;
    int64_t m_nodeId;
};

#endif  // NETWORKPAGEMANAGER_H
