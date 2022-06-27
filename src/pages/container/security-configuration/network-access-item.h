/**
 * @file          /ks-scmc-gui/src/pages/container/security-configuration/network-access-item.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef NETWORKACCESSITEM_H
#define NETWORKACCESSITEM_H

#include <QWidget>

namespace Ui
{
class NetworkAccessItem;
}

class NetworkAccessItem : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkAccessItem(QWidget *parent = nullptr);
    ~NetworkAccessItem();
    void setDeleteBtnVisible(bool visible);
    void getInfo(QStringList &protocols, QString &addr, int &port);
    void setInfo(QStringList protocols, QString addr, int port);

private slots:
    void ondelete();
    void onAdd();

signals:
    void sigDelete();
    void sigAdd();

private:
    void initUI();

private:
    Ui::NetworkAccessItem *ui;
};

#endif  // NETWORKACCESSITEM_H
