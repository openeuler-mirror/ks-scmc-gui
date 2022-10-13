#ifndef NETWORKLISTPAGE_H
#define NETWORKLISTPAGE_H

#include <QWidget>
#include "info-worker.h"
#include "table-page.h"
enum NetworkIfsType
{
    NETWORK_IFS_TYPE_REAL,
    NETWORK_IFS_TYPE_VIRT
};

class NetworkListPage : public TablePage
{
    Q_OBJECT
public:
    explicit NetworkListPage(NetworkIfsType type, QWidget *parent = nullptr);
    void updateInfo(QString keyword = "");
    void setNodeId(int64_t nodeId);

private slots:
    void onCreateVirtIfs();
    void onRemoveVirtIfs();
    void getListResult(const QString objId, const QPair<grpc::Status, network::ListReply> &);
    void getCreateResult(const QString objId, const QPair<grpc::Status, network::CreateNicReply> &);
    void getRemoveResult(const QString objId, const QPair<grpc::Status, network::RemoveNicReply> &);
    void getUpdateResult(const QString objId, const QPair<grpc::Status, network::UpdateNicReply> &);

private:
    void initRealIfsTable();
    void initVirtIfsTable();
    void initButtons();
    void initConnect();

private:
    QString m_objId;
    NetworkIfsType m_type;
    int64_t m_nodeId;
};

#endif  // NETWORKLISTPAGE_H
