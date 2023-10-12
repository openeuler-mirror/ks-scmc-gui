#ifndef NODELISTPAGE_H
#define NODELISTPAGE_H

#include <QWidget>
#include "info-worker.h"
#include "table-page.h"

struct nodeInfo_s
{
    nodeInfo_s(std::string _name, std::string _addr, std::string _comment) : name(_name), address(_addr), comment(_comment) {}
    std::string name;
    std::string address;
    std::string comment;
};

class NodeOperateDialog;
class NodeListPage : public TablePage
{
    Q_OBJECT
public:
    explicit NodeListPage(QWidget *parent = nullptr);
    ~NodeListPage();
    void updateInfo(QString keyword = "");  //刷新表格

private slots:
    void onCreateNode();
    void onRemoveNode();
    void onEdit(int row);
    void onSaveSlot(QMap<QString, QString> Info);
    void getListResult(const QPair<grpc::Status, node::ListReply> &);
    void getCreateResult(const QPair<grpc::Status, node::CreateReply> &);
    void getRemoveResult(const QPair<grpc::Status, node::RemoveReply> &);
    void getUpdateResult(const QPair<grpc::Status, node::UpdateReply> &);
    void onItemClicked(const QModelIndex &index);
    void onItemEntered(const QModelIndex &index);

signals:
    void sigNodeNameClicked(qint64 nodeId);

private:
    void initButtons();
    void initTable();
    void initNodeConnect();
    void getNodeList();
    void timedRefresh(bool start);

private:
    NodeOperateDialog *m_nodeAddition;
    NodeOperateDialog *m_nodeUpdate;
    QMap<int64_t, QPair<QString, QString>> m_mapStatus;
    QTimer *m_timer;
};

#endif  // NODELISTPAGE_H
