﻿#ifndef NETWORKCONFTAB_H
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
    void getNetworkInfo(container::CreateRequest *req);
    void updateNetworkInfo(int64_t node_id);

private:
    void getNetworkListResult(const QPair<grpc::Status, network::ListReply> &reply);

private:
    void initUI();
    void getNetworkInfo();

private:
    Ui::NetworkConfTab *ui;
};

#endif  // NETWORKCONFTAB_H