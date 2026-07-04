/**
 * @file          /ks-scmc-gui/src/pages/container/container-manager/container-list-page.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef CONTAINERLISTPAGE_H
#define CONTAINERLISTPAGE_H

#include <QMenu>
#include <QStandardItemModel>
#include <QWidget>
#include "container/container-setting.h"
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

private slots:
    void onBtnRun();
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
    void onActGenerateTemp(QModelIndex index);

    void onApp(int row);
    void onMonitor(int row);
    void onEdit(int row);
    void onTerminal(int row);

    void onItemClicked(const QModelIndex &index);
    void onItemEntered(const QModelIndex &index);

    void getNetworkListResult(const QString objId, const QPair<grpc::Status, network::ListReply> &reply);

    void getContainerListResult(const QString objId, const QPair<grpc::Status, container::ListReply> &);
    void getContainerStartResult(const QString objId, const QPair<grpc::Status, container::StartReply> &);
    void getContainerStopResult(const QString objId, const QPair<grpc::Status, container::StopReply> &);
    void getContainerRestartResult(const QString objId, const QPair<grpc::Status, container::RestartReply> &);
    void getContainerRemoveResult(const QString objId, const QPair<grpc::Status, container::RemoveReply> &);

    void getListTemplateFinishResult(const QString objId, const QPair<grpc::Status, container::ListTemplateReply> &reply);

signals:
    void sigContainerNameClicked(QMap<QString, QVariant> infoMap);

private:
    void initButtons();
    void initTable();
    void initConnect();
    void operateContainer(ContainerSettingType type, int row = -1);
    void getTemplateList();
    void getCheckedItemsId(std::map<int64_t, std::vector<std::string>> &ids);
    void getItemId(int row, std::map<int64_t, std::vector<std::string>> &ids);
    void timedRefresh(bool start);
    void getNetworkInfo(int64_t node_id);

private:
    QString m_objId;
    QMenu *m_createMenu;
    QMenu *m_moreMenu;
    QAction *m_createFromTemplateAct;
    QMap<int, QPushButton *> m_batchOpBtnMap;
    ContainerSetting *m_containerSetting;
    MonitorDialog *m_monitor;
    QTimer *m_timer;
    QMap<QString, QPair<QString, QString>> m_statusMap;
    QMultiMap<int, QPair<int, QString>> m_templateMap;  //templateId nodeId,template name
    QMultiMap<int, QString> m_networksMap;
};

#endif  // CONTAINERLISTPAGE_H
