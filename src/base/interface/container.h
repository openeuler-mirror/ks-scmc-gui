/**
 * @file          src/base/interface/container.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma once

#include <QMutex>
#include <QObject>
#include <QPair>

#include <grpcpp/grpcpp.h>

#include "def.h"
#include "rpc.h"
#include "user-configuration.h"

class Container : public QObject
{
    Q_OBJECT
public:
    static Container &getInstance()
    {
        static Container s_Container;
        return s_Container;
    }

    static QString generateId(QObject *callObj);

    // container management
    void listContainer(const QString objId, const std::vector<int64_t> &node_ids, const bool all);
    void createContainer(const QString objId, const container::CreateRequest &);
    // void containerStatus(const QString objId, const int64_t node_id);
    void containerInspect(const QString objId, const int64_t node_id, const std::string &container_id);
    void startContainer(const QString objId, QMap<int64_t, QStringList> &ids);
    void stopContainer(const QString objId, QMap<int64_t, QStringList> &ids);
    void restartContainer(const QString objId, QMap<int64_t, QStringList> &ids);
    void removeContainer(const QString objId, QMap<int64_t, QStringList> &ids);
    void updateContainer(const QString objId, const container::UpdateRequest &);
    void monitorHistory(const QString objId, int node_id, int start_time, int end_time, uint32_t interval, std::string container_id = "");

    // container template
    void listTemplate(const QString objId, const int perPage, const int nextPage, const std::string &sort, const std::string &likeSearch);
    void listTemplate(const QString objId);
    void inspectTemplate(const QString objId, int64_t id);
    void createTemplate(const QString objId, const container::CreateTemplateRequest &);
    void updateTemplate(const QString objId, const container::UpdateTemplateRequest &);
    void removeTemplate(const QString objId, QList<int64_t> ids);

    // container backup
    void listBackup(const QString objId, int nodeId, std::string containerId);
    void updateBackup(const QString objId, int nodeId, int id, std::string backupDesc);
    void createBackup(const QString objId, int nodeId, std::string containerId, std::string backupDesc, std::string backupName);
    void resumeBackup(const QString objId, int nodeId, std::string containerId, int backupId);
    void removeBackup(const QString objId, int nodeId, int64_t ids);
    void exportBackup(const QString objId, const container::ExportBackupRequest &req, const QString path);

    // container app
    void listAppEntry(const QString objId, const int nodeId, const std::string &containerId);
    void addAppEntry(const QString objId, const container::AddAppEntryRequest &);
    void updateAppEntry(const QString objId, const container::UpdateAppEntryRequest &);
    void removeAppEntry(const QString objId, const int nodeId, const std::string &containerId, const QList<qint64> appIds);
    void runAppEntry(const QString objId, const int nodeId, const std::string &containerId, const int appId);
    void killAppEntry(const QString objId, const int nodeId, const std::string &containerId, const int appId);

    // container version
    void updateContainerVersion(const QString objId, const int nodeId, const std::string &containerId, const std::string &imageName, const std::string &imageVersion);

private:
    Container(QObject *parent = nullptr);
    ~Container();

    // container management
    static QPair<grpc::Status, container::ListReply> _listContainer(const container::ListRequest &);
    static QPair<grpc::Status, container::CreateReply> _createContainer(const container::CreateRequest &);
    // static QPair<grpc::Status, container::StatusReply> _containerStatus(const container::StatusRequest &);
    static QPair<grpc::Status, container::InspectReply> _containerInspect(const container::InspectRequest &);
    static QPair<grpc::Status, container::StartReply> _startContainer(const container::StartRequest &);
    static QPair<grpc::Status, container::StopReply> _stopContainer(const container::StopRequest &);
    // static QPair<grpc::Status, container::KillReply> _killContainer(const container::KillRequest &);
    static QPair<grpc::Status, container::RestartReply> _restartContainer(const container::RestartRequest &);
    static QPair<grpc::Status, container::UpdateReply> _updateContainer(const container::UpdateRequest &);
    static QPair<grpc::Status, container::RemoveReply> _removeContainer(const container::RemoveRequest &);
    static QPair<grpc::Status, container::MonitorHistoryReply> _monitorHistory(const container::MonitorHistoryRequest &);

    // container template
    static QPair<grpc::Status, container::ListTemplateReply> _listTemplate(const container::ListTemplateRequest &);
    static QPair<grpc::Status, container::InspectTemplateReply> _inspectTemplate(const container::InspectTemplateRequest &);
    static QPair<grpc::Status, container::CreateTemplateReply> _createTemplate(const container::CreateTemplateRequest &);
    static QPair<grpc::Status, container::UpdateTemplateReply> _updateTemplate(const container::UpdateTemplateRequest &);
    static QPair<grpc::Status, container::RemoveTemplateReply> _removeTemplate(const container::RemoveTemplateRequest &);

    // container backup
    static QPair<grpc::Status, container::ListBackupReply> _listBackup(const container::ListBackupRequest &);
    static QPair<grpc::Status, container::UpdateBackupReply> _updateBackup(const container::UpdateBackupRequest &);
    static QPair<grpc::Status, container::CreateBackupReply> _createBackup(const container::CreateBackupRequest &);
    static QPair<grpc::Status, container::ResumeBackupReply> _resumeBackup(const container::ResumeBackupRequest &);
    static QPair<grpc::Status, container::RemoveBackupReply> _removeBackup(const container::RemoveBackupRequest &);
    static QPair<grpc::Status, QString> _exportBackup(const container::ExportBackupRequest &, const QString path);

    // container app
    static QPair<grpc::Status, container::ListAppEntryReply> _listAppEntry(const container::ListAppEntryRequest &);
    static QPair<grpc::Status, container::AddAppEntryReply> _addAppEntry(const container::AddAppEntryRequest &);
    static QPair<grpc::Status, container::UpdateAppEntryReply> _updateAppEntry(const container::UpdateAppEntryRequest &);
    static QPair<grpc::Status, container::RemoveAppEntryReply> _removeAppEntry(const container::RemoveAppEntryRequest &);
    static QPair<grpc::Status, container::RunAppEntryReply> _runAppEntry(const container::RunAppEntryRequest &);
    static QPair<grpc::Status, container::KillAppEntryReply> _killAppEntry(const container::KillAppEntryRequest &);

    // container version
    static QPair<grpc::Status, container::UpdateContainerVersionReply> _updateContainerVersion(const container::UpdateContainerVersionRequest &);

signals:
    // container management
    void listContainerFinished(const QString objId, const QPair<grpc::Status, container::ListReply> &);
    void createContainerFinished(const QString objId, const QPair<grpc::Status, container::CreateReply> &);
    // void containerStatusFinished(const QString objId, const QPair<grpc::Status, container::StatusReply> &);
    void containerInspectFinished(const QString objId, const QPair<grpc::Status, container::InspectReply> &);
    void startContainerFinished(const QString objId, const QPair<grpc::Status, container::StartReply> &);
    void stopContainerFinished(const QString objId, const QPair<grpc::Status, container::StopReply> &);
    // void killContainerFinished(const QString objId, const QPair<grpc::Status, container::KillReply> &);
    void restartContainerFinished(const QString objId, const QPair<grpc::Status, container::RestartReply> &);
    void updateContainerFinished(const QString objId, const QPair<grpc::Status, container::UpdateReply> &);
    void removeContainerFinished(const QString objId, const QPair<grpc::Status, container::RemoveReply> &);
    void monitorHistoryFinished(const QString objId, const QPair<grpc::Status, container::MonitorHistoryReply> &);

    // container template
    void listTemplateFinished(const QString objId, const QPair<grpc::Status, container::ListTemplateReply> &);
    void inspectTemplateFinished(const QString objId, const QPair<grpc::Status, container::InspectTemplateReply> &);
    void createTemplateFinished(const QString objId, const QPair<grpc::Status, container::CreateTemplateReply> &);
    void updateTemplateFinished(const QString objId, const QPair<grpc::Status, container::UpdateTemplateReply> &);
    void removeTemplateFinished(const QString objId, const QPair<grpc::Status, container::RemoveTemplateReply> &);

    // container backup
    void listBackupFinished(const QString objId, const QPair<grpc::Status, container::ListBackupReply> &);
    void updateBackupFinished(const QString objId, const QPair<grpc::Status, container::UpdateBackupReply> &);
    void createBackupFinished(const QString objId, const QPair<grpc::Status, container::CreateBackupReply> &);
    void resumeBackupFinished(const QString objId, const QPair<grpc::Status, container::ResumeBackupReply> &);
    void removeBackupFinished(const QString objId, const QPair<grpc::Status, container::RemoveBackupReply> &);
    void exportBackupFinished(const QString objId, const QPair<grpc::Status, QString> &);

    // container app
    void listAppEntryFinished(const QString objId, const QPair<grpc::Status, container::ListAppEntryReply> &);
    void addAppEntryFinished(const QString objId, const QPair<grpc::Status, container::AddAppEntryReply> &);
    void updateAppEntryFinished(const QString objId, const QPair<grpc::Status, container::UpdateAppEntryReply> &);
    void removeAppEntryFinished(const QString objId, const QPair<grpc::Status, container::RemoveAppEntryReply> &);
    void runAppEntryFinished(const QString objId, const QPair<grpc::Status, container::RunAppEntryReply> &);
    void killAppEntryFinished(const QString objId, const QPair<grpc::Status, container::KillAppEntryReply> &);

private:
    QMutex mutex;
    QStringList m_exportList;
};
