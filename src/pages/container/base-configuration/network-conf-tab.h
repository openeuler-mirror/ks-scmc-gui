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
    void setNetworkInfo(container::NetworkConfig *networkCfg);
    void setVirtNetworkData(QString virtNetwork);
    void setVirtNetwork();
    void setVirtNetworkList(const QPair<grpc::Status, network::ListReply> &reply);

private:
    void initUI();

private:
    Ui::NetworkConfTab *ui;
    QString m_virtNetwork;
};

#endif  // NETWORKCONFTAB_H
