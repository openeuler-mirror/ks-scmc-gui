#ifndef CONTAINERLISTPAGE_H
#define CONTAINERLISTPAGE_H

#include <QMenu>
#include <QStandardItemModel>
#include <QWidget>
#include "info-worker.h"
#include "table-page.h"

class ContainerSetting;
class MonitorDialog;
class ContainerListPage : public TablePage
{
    Q_OBJECT

public:
    explicit ContainerListPage(QWidget *parent = nullptr);
    ~ContainerListPage();
    void updateInfo(QString keyword = "");  //刷新表格
    void getContainerList(qint64 nodeId = -1);
    void operateAreaVisible(bool visible);

private slots:
    void
    onBtnRun();
    void onBtnRun(QModelIndex index);
    void onBtnStop();
    void onBtnStop(QModelIndex index);
    void onBtnRestart();
    void onBtnRestart(QModelIndex index);
    void onBtnDelete();
    void onActCreate();
    void onActCopyConfig();
    void onActBatchUpdate();
    void onActBatchEdit();
    void onActBackup();

    void onMonitor(int row);
    void onEdit(int row);
    void onTerminal(int row);

    void onItemClicked(const QModelIndex &index);
    void onItemEntered(const QModelIndex &index);

    void getContainerListResult(const QPair<grpc::Status, container::ListReply> &);
    void getContainerStartResult(const QPair<grpc::Status, container::StartReply> &);
    void getContainerStopResult(const QPair<grpc::Status, container::StopReply> &);
    void getContainerRestartResult(const QPair<grpc::Status, container::RestartReply> &);
    void getContainerRemoveResult(const QPair<grpc::Status, container::RemoveReply> &);

signals:
    void sigContainerNameClicked(QMap<QString, QVariant> infoMap);

private:
    void initButtons();
    void initTable();
    void initConnect();
    void getCheckedItemsId(std::map<int64_t, std::vector<std::string>> &ids);
    void getItemId(int row, std::map<int64_t, std::vector<std::string>> &ids);
    void timedRefresh(bool start);

private:
    QMenu *m_createMenu;
    QMenu *m_moreMenu;
    QMap<int, QPushButton *> m_batchOpBtnMap;
    ContainerSetting *m_createCTSetting;
    ContainerSetting *m_editCTSetting;
    MonitorDialog *m_monitor;
    std::vector<int64_t> m_vecNodeId;
    QTimer *m_timer;
    QMap<QString, QPair<QString, QString>> m_statusMap;
};

#endif  // CONTAINERLISTPAGE_H
