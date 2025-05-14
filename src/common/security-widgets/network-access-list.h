/**
 * @file          src/common/security-widgets/network-access-list.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma once

#include <QListWidget>
#include <QObject>
#include <QVBoxLayout>

class NetworkAccessItem;
class NetworkAccessList : public QWidget
{
public:
    struct NetworkAccessInfo
    {
        QStringList protocols;
        QString addr;
        int port = 0;
        bool defaultPort = false;
    };

    Q_OBJECT
public:
    explicit NetworkAccessList(QWidget *parent = nullptr);
    ~NetworkAccessList();

    void setNeteworkInfos(QList<NetworkAccessList::NetworkAccessInfo> infos);
    QList<NetworkAccessList::NetworkAccessInfo> getNeteworkInfos();

    void clearItems();
    void setItemsEnable(bool enable);

private:
    void initUI();
    void createItem(int index);
    void addItem();
    void deleteItem();
    void deleteOneItem(QListWidgetItem *listItem);

private:
    QListWidget *m_listWidget;
};