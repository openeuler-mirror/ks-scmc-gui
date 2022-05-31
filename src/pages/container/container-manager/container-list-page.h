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

    void getNetworkListResult(const QPair<grpc::Status, network::ListReply> &reply);

    void getContainerListResult(const QPair<grpc::Status, container::ListReply> &);
    void getContainerStartResult(const QPair<grpc::Status, container::StartReply> &);
    void getContainerStopResult(const QPair<grpc::Status, container::StopReply> &);
    void getContainerRestartResult(const QPair<grpc::Status, container::RestartReply> &);
    void getContainerRemoveResult(const QPair<grpc::Status, container::RemoveReply> &);

    void getListTemplateFinishResult(const QPair<grpc::Status, container::ListTemplateReply> &reply);

signals:
    void sigContainerNameClicked(QMap<QString, QVariant> infoMap);

private:
    void initButtons();
    void initTable();
    void initConnect();
    void getTemplateList();
    void getCheckedItemsId(std::map<int64_t, std::vector<std::string>> &ids);
    void getItemId(int row, std::map<int64_t, std::vector<std::string>> &ids);
    void timedRefresh(bool start);
    void getNetworkInfo(int64_t node_id);

private:
    QMenu *m_createMenu;
    QMenu *m_moreMenu;
    QAction *m_createFromTemplateAct;
    QMap<int, QPushButton *> m_batchOpBtnMap;
    ContainerSetting *m_createCTSetting;
    ContainerSetting *m_editCTSetting;
    MonitorDialog *m_monitor;
    QTimer *m_timer;
    QMap<QString, QPair<QString, QString>> m_statusMap;
    QMultiMap<int, QPair<int, QString>> m_templateMap;  //templateId nodeId,template name
    QMultiMap<int, QString> m_networksMap;
};

#endif  // CONTAINERLISTPAGE_H
