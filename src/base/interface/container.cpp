/**
 * @file          src/base/interface/container.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "container.h"
#include "common.h"

#include <kiran-log/qt5-log-i.h>
#include <QMutexLocker>
#include <QUuid>

Container::Container(QObject *parent) : QObject(parent)
{
    QMutexLocker locker(&mutex);
}

Container::~Container()
{
}

QString Container::generateId(QObject *callObj)
{
    QString uid = QUuid::createUuid().toString();
    return QString("%1_%2").arg(callObj->metaObject()->className()).arg(uid);
}

void Container::listContainer(const QString objId, const std::vector<int64_t> &node_ids, const bool all)
{
    container::ListRequest req;
    req.set_list_all(all);
    for (auto &id : node_ids)
    {
        req.add_node_ids(id);
    }

    RPC_ASYNC(container::ListReply, _listContainer, listContainerFinished, objId, req);
}

void Container::createContainer(const QString objId, const container::CreateRequest &req)
{
    RPC_ASYNC(container::CreateReply, _createContainer, createContainerFinished, objId, req);
}

// void Container::containerStatus(const QString objId, const int64_t node_id)
//{
//     container::StatusRequest req;
//     req.set_node_id(node_id);
//     RPC_ASYNC(container::StatusReply, _containerStatus, containerStatusFinished, objId, req);
// }

void Container::containerInspect(const QString objId, const int64_t node_id, const std::string &container_id)
{
    container::InspectRequest req;
    req.set_node_id(node_id);
    req.set_container_id(container_id);
    RPC_ASYNC(container::InspectReply, _containerInspect, containerInspectFinished, objId, req);
}

void Container::startContainer(const QString objId, QMap<int64_t, QStringList> &ids)
{
    container::StartRequest req;
    auto i = ids.constBegin();
    while (i != ids.constEnd())
    {
        auto pId = req.add_ids();
        pId->set_node_id(i.key());
        for (QString container_id : i.value())
        {
            pId->add_container_ids(container_id.toStdString());
        }
        ++i;
    }

    RPC_ASYNC(container::StartReply, _startContainer, startContainerFinished, objId, req);
}

void Container::stopContainer(const QString objId, QMap<int64_t, QStringList> &ids)
{
    container::StopRequest req;
    auto i = ids.constBegin();
    while (i != ids.constEnd())
    {
        auto pId = req.add_ids();
        pId->set_node_id(i.key());
        for (QString container_id : i.value())
        {
            pId->add_container_ids(container_id.toStdString());
        }
        ++i;
    }

    RPC_ASYNC(container::StopReply, _stopContainer, stopContainerFinished, objId, req);
}

void Container::restartContainer(const QString objId, QMap<int64_t, QStringList> &ids)
{
    container::RestartRequest req;
    auto i = ids.constBegin();
    while (i != ids.constEnd())
    {
        auto pId = req.add_ids();
        pId->set_node_id(i.key());
        for (QString container_id : i.value())
        {
            pId->add_container_ids(container_id.toStdString());
        }
        ++i;
    }
    RPC_ASYNC(container::RestartReply, _restartContainer, restartContainerFinished, objId, req);
}

void Container::removeContainer(const QString objId, QMap<int64_t, QStringList> &ids)
{
    container::RemoveRequest req;
    auto i = ids.constBegin();
    while (i != ids.constEnd())
    {
        auto pId = req.add_ids();
        pId->set_node_id(i.key());
        for (QString container_id : i.value())
        {
            pId->add_container_ids(container_id.toStdString());
        }
        ++i;
    }
    RPC_ASYNC(container::RemoveReply, _removeContainer, removeContainerFinished, objId, req);
}

void Container::updateContainer(const QString objId, const container::UpdateRequest &req)
{
    RPC_ASYNC(container::UpdateReply, _updateContainer, updateContainerFinished, objId, req);
}

void Container::monitorHistory(const QString objId, int node_id, int start_time, int end_time, uint32_t interval, std::string container_id)
{
    container::MonitorHistoryRequest req;
    req.set_node_id(node_id);
    req.set_start_time(start_time);
    req.set_end_time(end_time);
    req.set_interval(interval);
    req.set_container_id(container_id);
    RPC_ASYNC(container::MonitorHistoryReply, _monitorHistory, monitorHistoryFinished, objId, req);
}

void Container::listTemplate(const QString objId, const int perPage, const int nextPage, const std::string &sort, const std::string &likeSearch)
{
    container::ListTemplateRequest req;
    req.set_per_page(perPage);
    req.set_next_page(nextPage);
    req.set_sort(sort);
    req.set_like_search(likeSearch);
    RPC_ASYNC(container::ListTemplateReply, _listTemplate, listTemplateFinished, objId, req);
}

void Container::listTemplate(const QString objId)
{
    container::ListTemplateRequest req;
    req.set_per_page(-1);  // list all template
    RPC_ASYNC(container::ListTemplateReply, _listTemplate, listTemplateFinished, objId, req);
}

void Container::inspectTemplate(const QString objId, int64_t id)
{
    container::InspectTemplateRequest req;
    req.set_id(id);
    RPC_ASYNC(container::InspectTemplateReply, _inspectTemplate, inspectTemplateFinished, objId, req);
}

void Container::createTemplate(const QString objId, const container::CreateTemplateRequest &req)
{
    RPC_ASYNC(container::CreateTemplateReply, _createTemplate, createTemplateFinished, objId, req);
}

void Container::updateTemplate(const QString objId, const container::UpdateTemplateRequest &req)
{
    RPC_ASYNC(container::UpdateTemplateReply, _updateTemplate, updateTemplateFinished, objId, req);
}

void Container::removeTemplate(const QString objId, QList<int64_t> ids)
{
    container::RemoveTemplateRequest req;
    foreach (int64_t id, ids)
    {
        req.add_ids(id);
    }
    RPC_ASYNC(container::RemoveTemplateReply, _removeTemplate, removeTemplateFinished, objId, req);
}

void Container::listBackup(const QString objId, int nodeId, std::string containerId)
{
    container::ListBackupRequest req;
    req.set_node_id(nodeId);
    req.set_container_id(containerId);
    RPC_ASYNC(container::ListBackupReply, _listBackup, listBackupFinished, objId, req);
}

void Container::updateBackup(const QString objId, int nodeId, int id, std::string backupDesc)
{
    container::UpdateBackupRequest req;
    req.set_node_id(nodeId);
    req.set_id(id);
    req.set_backup_desc(backupDesc);
    RPC_ASYNC(container::UpdateBackupReply, _updateBackup, updateBackupFinished, objId, req);
}

void Container::createBackup(const QString objId, int nodeId, std::string containerId, std::string backupDesc, std::string backupName)
{
    container::CreateBackupRequest req;
    req.set_node_id(nodeId);
    req.set_container_id(containerId);
    req.set_backup_desc(backupDesc);
    req.set_backup_name(backupName);
    RPC_ASYNC(container::CreateBackupReply, _createBackup, createBackupFinished, objId, req);
}

void Container::removeBackup(const QString objId, int nodeId, int64_t id)
{
    container::RemoveBackupRequest req;
    req.set_node_id(nodeId);
    req.set_id(id);
    RPC_ASYNC(container::RemoveBackupReply, _removeBackup, removeBackupFinished, objId, req);
}

void Container::exportBackup(const QString objId, const container::ExportBackupRequest &req, const QString path)
{
    RPC_ASYNC(QString, _exportBackup, exportBackupFinished, objId, req, path);
}

void Container::listAppEntry(const QString objId, const int nodeId, const std::string &containerId)
{
    container::ListAppEntryRequest req;
    req.set_node_id(nodeId);
    req.set_container_id(containerId);
    RPC_ASYNC(container::ListAppEntryReply, _listAppEntry, listAppEntryFinished, objId, req);
}

void Container::addAppEntry(const QString objId, const container::AddAppEntryRequest &req)
{
    RPC_ASYNC(container::AddAppEntryReply, _addAppEntry, addAppEntryFinished, objId, req);
}

void Container::updateAppEntry(const QString objId, const container::UpdateAppEntryRequest &req)
{
    RPC_ASYNC(container::UpdateAppEntryReply, _updateAppEntry, updateAppEntryFinished, objId, req);
}

void Container::removeAppEntry(const QString objId, const int nodeId, const std::string &containerId, const QList<qint64> appIds)
{
    container::RemoveAppEntryRequest req;
    req.set_node_id(nodeId);
    req.set_container_id(containerId);
    foreach (auto id, appIds)
    {
        req.add_app_ids(id);
    }
    RPC_ASYNC(container::RemoveAppEntryReply, _removeAppEntry, removeAppEntryFinished, objId, req);
}

void Container::runAppEntry(const QString objId, const int nodeId, const std::string &containerId, const int appId)
{
    container::RunAppEntryRequest req;
    req.set_node_id(nodeId);
    req.set_container_id(containerId);
    req.set_app_id(appId);
    RPC_ASYNC(container::RunAppEntryReply, _runAppEntry, runAppEntryFinished, objId, req);
}

void Container::killAppEntry(const QString objId, const int nodeId, const std::string &containerId, const int appId)
{
    container::KillAppEntryRequest req;
    req.set_node_id(nodeId);
    req.set_container_id(containerId);
    req.set_app_id(appId);
    RPC_ASYNC(container::KillAppEntryReply, _killAppEntry, killAppEntryFinished, objId, req);
}

void Container::updateContainerVersion(const QString objId, const int nodeId, const std::string &containerId, const std::string &imageName, const std::string &imageVersion)
{
    container::UpdateContainerVersionRequest req;
    req.set_node_id(nodeId);
    req.set_container_id(containerId);
    req.set_img_name(imageName);
    req.set_img_version(imageVersion);

    RPC_ASYNC(container::UpdateContainerVersionReply, _updateContainerVersion, updateContainerVersionFinished, objId, req);
}

void Container::resumeBackup(const QString objId, int nodeId, std::string containerId, int backupId)
{
    container::ResumeBackupRequest req;
    req.set_node_id(nodeId);
    req.set_container_id(containerId);
    req.set_backup_id(backupId);
    RPC_ASYNC(container::ResumeBackupReply, _resumeBackup, resumeBackupFinished, objId, req);
}

QPair<grpc::Status, container::ListReply> Container::_listContainer(const container::ListRequest &req)
{
    RPC_IMPL(container::ListReply, container::Container::NewStub, List);
}

QPair<grpc::Status, container::CreateReply> Container::_createContainer(const container::CreateRequest &req)
{
    RPC_IMPL(container::CreateReply, container::Container::NewStub, Create);
}

// QPair<grpc::Status, container::StatusReply> Container::_containerStatus(const container::StatusRequest &req)
//{
//     RPC_IMPL(container::StatusReply, container::Container::NewStub, Status);
// }

QPair<grpc::Status, container::InspectReply> Container::_containerInspect(const container::InspectRequest &req)
{
    RPC_IMPL(container::InspectReply, container::Container::NewStub, Inspect);
}

QPair<grpc::Status, container::StartReply> Container::_startContainer(const container::StartRequest &req)
{
    RPC_IMPL(container::StartReply, container::Container::NewStub, Start);
}

QPair<grpc::Status, container::StopReply> Container::_stopContainer(const container::StopRequest &req)
{
    RPC_IMPL(container::StopReply, container::Container::NewStub, Stop);
}

// QPair<grpc::Status, container::KillReply> Container::_killContainer(const container::KillRequest &req)
//{
//     RPC_IMPL(container::KillReply, container::Container::NewStub, Kill);
// }

QPair<grpc::Status, container::RestartReply> Container::_restartContainer(const container::RestartRequest &req)
{
    RPC_IMPL(container::RestartReply, container::Container::NewStub, Restart);
}

QPair<grpc::Status, container::UpdateReply> Container::_updateContainer(const container::UpdateRequest &req)
{
    RPC_IMPL(container::UpdateReply, container::Container::NewStub, Update);
}

QPair<grpc::Status, container::RemoveReply> Container::_removeContainer(const container::RemoveRequest &req)
{
    RPC_IMPL(container::RemoveReply, container::Container::NewStub, Remove);
}

QPair<grpc::Status, container::MonitorHistoryReply> Container::_monitorHistory(const container::MonitorHistoryRequest &req)
{
    RPC_IMPL(container::MonitorHistoryReply, container::Container::NewStub, MonitorHistory);
}

QPair<grpc::Status, container::ListTemplateReply> Container::_listTemplate(const container::ListTemplateRequest &req)
{
    RPC_IMPL(container::ListTemplateReply, container::Container::NewStub, ListTemplate);
}

QPair<grpc::Status, container::InspectTemplateReply> Container::_inspectTemplate(const container::InspectTemplateRequest &req)
{
    RPC_IMPL(container::InspectTemplateReply, container::Container::NewStub, InspectTemplate);
}

QPair<grpc::Status, container::CreateTemplateReply> Container::_createTemplate(const container::CreateTemplateRequest &req)
{
    RPC_IMPL(container::CreateTemplateReply, container::Container::NewStub, CreateTemplate);
}

QPair<grpc::Status, container::UpdateTemplateReply> Container::_updateTemplate(const container::UpdateTemplateRequest &req)
{
    RPC_IMPL(container::UpdateTemplateReply, container::Container::NewStub, UpdateTemplate);
}

QPair<grpc::Status, container::RemoveTemplateReply> Container::_removeTemplate(const container::RemoveTemplateRequest &req)
{
    RPC_IMPL(container::RemoveTemplateReply, container::Container::NewStub, RemoveTemplate);
}

QPair<grpc::Status, container::ListBackupReply> Container::_listBackup(const container::ListBackupRequest &req)
{
    RPC_IMPL(container::ListBackupReply, container::Container::NewStub, ListBackup);
}

QPair<grpc::Status, container::UpdateBackupReply> Container::_updateBackup(const container::UpdateBackupRequest &req)
{
    RPC_IMPL(container::UpdateBackupReply, container::Container::NewStub, UpdateBackup);
}

QPair<grpc::Status, container::CreateBackupReply> Container::_createBackup(const container::CreateBackupRequest &req)
{
    RPC_IMPL(container::CreateBackupReply, container::Container::NewStub, CreateBackup);
}

QPair<grpc::Status, container::ResumeBackupReply> Container::_resumeBackup(const container::ResumeBackupRequest &req)
{
    RPC_IMPL(container::ResumeBackupReply, container::Container::NewStub, ResumeBackup);
}

QPair<grpc::Status, container::RemoveBackupReply> Container::_removeBackup(const container::RemoveBackupRequest &req)
{
    RPC_IMPL(container::RemoveBackupReply, container::Container::NewStub, RemoveBackup);
}

QPair<grpc::Status, QString> Container::_exportBackup(const container::ExportBackupRequest &req, const QString path)
{
    QPair<grpc::Status, QString> r;
    bool isDownload = req.is_download();

    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());
    if (!chan)
    {
        KLOG_WARNING() << "Export backup failed to get connection";
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        return r;
    }

    container::ExportBackupReply reply;
    grpc::ClientContext context;
    if (Common::getInstance().getAuthKey().size() > 0)
        context.AddMetadata("authorization", Common::getInstance().getAuthKey());

    auto stream = container::Container::NewStub(chan)->ExportBackup(&context, req);
    // Read流数据会耗时一会
    bool ret = stream->Read(&reply);
    if (!ret)
    {
        KLOG_WARNING() << "Recv backup param err";
        r.first = stream->Finish();
        return r;
    }

    // 导出到镜像仓库，不处理data 流数据
    if (!isDownload)
    {
        r.first = stream->Finish();
        if (grpc::StatusCode(ErrUnauthenticated) == r.first.error_code())
            emit Common::getInstance().sessionExpire();
        else if (grpc::StatusCode::DEADLINE_EXCEEDED == r.first.error_code())
        {
            r.first = grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED,
                                   QObject::tr("Response timeout").toStdString());
        }
        return r;
    }

    const auto name = QString::fromStdString(reply.img_name());
    const auto version = QString::fromStdString(reply.img_version());
    const auto size = reply.img_size();
    QString exportName = name + "-" + version;
    KLOG_DEBUG() << "export name:" << name << "export version:" << version << "export size:" << size;
    if (Container::getInstance().m_exportList.contains(exportName))
    {
        r.first = grpc::Status(grpc::StatusCode::ALREADY_EXISTS, tr("The %1 backup is exporting,please wait a minute.").arg(exportName).toStdString());
        return r;
    }
    else
        Container::getInstance().m_exportList.append(exportName);

    QString filePath = QString("%1%2_%3.tar").arg(path).arg(name).arg(version);
    QFile imgFile(filePath);
    if (!imgFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        KLOG_WARNING() << "Failed to open " << filePath;
        r.first = grpc::Status(grpc::StatusCode::INTERNAL,
                               QObject::tr("Failed to open %1").arg(filePath).toStdString());
        Container::getInstance().m_exportList.removeAll(exportName);
        return r;
    }

    int64_t trans(0), progress(0);
    while (stream->Read(&reply))
    {
        if (Common::getInstance().isTransferStoped(name, version))
        {
            imgFile.close();
            r.first = grpc::Status(grpc::StatusCode::ABORTED, QObject::tr("Cancel").toStdString());
            Container::getInstance().m_exportList.removeAll(exportName);
            return r;
        }

        imgFile.write(reply.data().data(), qint64(reply.data().size()));
        trans += reply.data().size();
        int tmp = int(floor(trans * 100.0 / size));
        if (progress != tmp)
        {
            progress = tmp > 100 ? 100 : tmp;  // steam数据包含镜像的信息，会比实际镜像大，所有进度>100后显示100
            emit Common::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_DOWNLOADING, name, version, progress);
        }
    }
    if (progress != 100)
    {
        r.first = grpc::Status(grpc::StatusCode::ABORTED, tr("Export interruption!").toStdString());
        emit Common::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_DOWNLOADING_FAILED, name, version, progress);
    }
    else
    {
        r.first = grpc::Status(grpc::StatusCode::OK, tr("Ok").toStdString());
        r.second = filePath;
        emit Common::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_DOWNLOADING_SUCCESSFUL, name, version, progress);
    }
    imgFile.close();
    Container::getInstance().m_exportList.removeAll(exportName);
    return r;
}

QPair<grpc::Status, container::ListAppEntryReply> Container::_listAppEntry(const container::ListAppEntryRequest &req)
{
    RPC_IMPL(container::ListAppEntryReply, container::Container::NewStub, ListAppEntry);
}

QPair<grpc::Status, container::AddAppEntryReply> Container::_addAppEntry(const container::AddAppEntryRequest &req)
{
    RPC_IMPL(container::AddAppEntryReply, container::Container::NewStub, AddAppEntry);
}

QPair<grpc::Status, container::UpdateAppEntryReply> Container::_updateAppEntry(const container::UpdateAppEntryRequest &req)
{
    RPC_IMPL(container::UpdateAppEntryReply, container::Container::NewStub, UpdateAppEntry);
}

QPair<grpc::Status, container::RemoveAppEntryReply> Container::_removeAppEntry(const container::RemoveAppEntryRequest &req)
{
    RPC_IMPL(container::RemoveAppEntryReply, container::Container::NewStub, RemoveAppEntry);
}

QPair<grpc::Status, container::RunAppEntryReply> Container::_runAppEntry(const container::RunAppEntryRequest &req)
{
    RPC_IMPL(container::RunAppEntryReply, container::Container::NewStub, RunAppEntry);
}

QPair<grpc::Status, container::KillAppEntryReply> Container::_killAppEntry(const container::KillAppEntryRequest &req)
{
    RPC_IMPL(container::KillAppEntryReply, container::Container::NewStub, KillAppEntry);
}

QPair<grpc::Status, container::UpdateContainerVersionReply> Container::_updateContainerVersion(const container::UpdateContainerVersionRequest &req)
{
    RPC_IMPL(container::UpdateContainerVersionReply, container::Container::NewStub, UpdateContainerVersion);
}
