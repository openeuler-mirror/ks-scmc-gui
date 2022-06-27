/**
 * @file          /ks-scmc-gui/src/pages/container/base-configuration/network-conf-tab.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef NETWORKCONFTAB_H
#define NETWORKCONFTAB_H

#include <QWidget>
#include "info-worker.h"
namespace Ui
{
class NetworkConfTab;
}

class NetworkConfTab : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkConfTab(QWidget *parent = nullptr);
    ~NetworkConfTab();
    //void getNetworkInfo(container::CreateRequest *req);
    void getNetworkInfo(container::ContainerConfigs *cntrCfg);
    void getNetworkInfo(container::UpdateRequest *req);
    void setNetworkInfo(container::NetworkConfig *networkCfg, QList<QString> networkList);
    void initVirtNetworkInfo(QList<QString> networks);

private:
    void setVirtNetwork(QString virtNetwork);

private:
    void initUI();

private:
    Ui::NetworkConfTab *ui;
    QString m_virtNetwork;
};

#endif  // NETWORKCONFTAB_H
