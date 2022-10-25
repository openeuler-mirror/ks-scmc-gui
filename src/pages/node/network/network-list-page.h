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

enum NetworkIfsOperateType
{
    OPERATE_TYPE_CREATE,
    OPERATE_TYPE_EDIT
};

class KiranTitlebarWindow;
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
    void removeVirtIfs(int row);
    void onEditVirtIfs(int row);
    void getListResult(const QString objId, const QPair<grpc::Status, network::ListReply> &);
    void getCreateResult(const QString objId, const QPair<grpc::Status, network::CreateNicReply> &);
    void getRemoveResult(const QString objId, const QPair<grpc::Status, network::RemoveNicReply> &);
    void getUpdateResult(const QString objId, const QPair<grpc::Status, network::UpdateNicReply> &);

private:
    void initRealIfsTable();
    void initVirtIfsTable();
    void initButtons();
    void initConnect();
    KiranTitlebarWindow *createOperateDialog(NetworkIfsOperateType type, QString name = "", QString subnet = "", QString realIfs = "");
    void showOperateDialog(NetworkIfsOperateType type, QString name = "", QString subnet = "", QString realIfs = "");

private:
    QString m_objId;
    NetworkIfsType m_type;
    int64_t m_nodeId;
    QStringList m_realIfs;
};

#endif  // NETWORKLISTPAGE_H
