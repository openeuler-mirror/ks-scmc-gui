/**
 * @file          /ks-scmc-gui/src/base/info-worker.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "info-worker.h"

#include <kiran-log/qt5-log-i.h>

#include <QMutexLocker>
#include <QUuid>
#include <QtConcurrent/QtConcurrent>
#define DEADLINE 8000

const int CHUNK_SIZE = 1024 * 1024;

#define RPC_ASYNC(REPLY_TYPE, WORKER, CALLBACK, OBJID, ...)                 \
    typedef QPair<grpc::Status, REPLY_TYPE> T;                              \
    QFutureWatcher<T> *watcher = new QFutureWatcher<T>();                   \
    watcher->setFuture(QtConcurrent::run(WORKER, ##__VA_ARGS__));           \
    connect(watcher, &QFutureWatcher<T>::finished, [this, watcher, OBJID] { \
        auto reply = watcher->result();                                     \
        emit CALLBACK(OBJID, reply);                                        \
        delete watcher;                                                     \
    });

#define RPC_IMPL(REPLY_TYPE, STUB, RPC_NAME)                                                \
    QPair<grpc::Status, REPLY_TYPE> r;                                                      \
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());                        \
    if (!chan)                                                                              \
    {                                                                                       \
        KLOG_INFO("%s %s failed to get connection", #STUB, #RPC_NAME);                      \
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,                                   \
                               QObject::tr("Network Error").toStdString());                 \
        return r;                                                                           \
    }                                                                                       \
    grpc::ClientContext ctx;                                                                \
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(DEADLINE); \
    ctx.set_deadline(deadline);                                                             \
    if (s_authKey.size() > 0)                                                               \
        ctx.AddMetadata("authorization", s_authKey);                                        \
    r.first = STUB(chan)->RPC_NAME(&ctx, req, &r.second);                                   \
    if (grpc::StatusCode(ErrUnauthenticated) == r.first.error_code())                       \
        emit InfoWorker::getInstance().sessinoExpire();                                     \
    else if (grpc::StatusCode::DEADLINE_EXCEEDED == r.first.error_code())                   \
    {                                                                                       \
        r.first = grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED,                         \
                               QObject::tr("Response timeout").toStdString());              \
    }                                                                                       \
    return r;

InfoWorker::InfoWorker(QObject *parent) : QObject(parent)
{
    QMutexLocker locker(&mutex);
    qRegisterMetaType<ImageTransmissionStatus>("ImageTransmissionStatus");
}

InfoWorker::~InfoWorker()
{
}

QString InfoWorker::generateId(QObject *callObj)
{
    QString uid = QUuid::createUuid().toString();
    return QString("%1_%2").arg(callObj->metaObject()->className()).arg(uid);
}

std::string InfoWorker::getAuthKey()
{
    return s_authKey;
}

void InfoWorker::listNode(const QString objId)
{
    node::ListRequest req;
    RPC_ASYNC(node::ListReply, _listNode, listNodeFinished, objId, req);
}

void InfoWorker::listContainer(const QString objId, const std::vector<int64_t> &node_ids, const bool all)
{
    container::ListRequest req;
    req.set_list_all(all);
    for (auto &id : node_ids)
    {
        req.add_node_ids(id);
    }

    RPC_ASYNC(container::ListReply, _listContainer, listContainerFinished, objId, req);
}

void InfoWorker::createNode(const QString objId, const node::CreateRequest &req)
{
    RPC_ASYNC(node::CreateReply, _createNode, createNodeFinished, objId, req);
}

void InfoWorker::createContainer(const QString objId, const container::CreateRequest &req)
{
    RPC_ASYNC(container::CreateReply, _createContainer, createContainerFinished, objId, req);
}

//void InfoWorker::containerStatus(const QString objId, const int64_t node_id)
//{
//    container::StatusRequest req;
//    req.set_node_id(node_id);
//    RPC_ASYNC(container::StatusReply, _containerStatus, containerStatusFinished, objId, req);
//}

void InfoWorker::removeNode(const QString objId, const std::vector<int64_t> &node_ids)
{
    node::RemoveRequest req;
    for (auto &id : node_ids)
    {
        req.add_ids(id);
    }

    RPC_ASYNC(node::RemoveReply, _removeNode, removeNodeFinished, objId, req);
}

void InfoWorker::nodeStatus(const QString objId, const std::vector<int64_t> &node_ids)
{
    node::StatusRequest req;
    for (auto &id : node_ids)
    {
        req.add_node_ids(id);
    }

    RPC_ASYNC(node::StatusReply, _nodeStatus, statusNodeFinished, objId, req);
}

void InfoWorker::updateNode(const QString objId, const node::UpdateRequest &req)
{
    RPC_ASYNC(node::UpdateReply, _updateNode, updateNodeFinished, objId, req);
}

void InfoWorker::containerInspect(const QString objId, const int64_t node_id, const std::string &container_id)
{
    container::InspectRequest req;
    req.set_node_id(node_id);
    req.set_container_id(container_id);
    RPC_ASYNC(container::InspectReply, _containerInspect, containerInspectFinished, objId, req);
}

void InfoWorker::startContainer(const QString objId, const std::map<int64_t, std::vector<std::string>> &ids)
{
    container::StartRequest req;
    for (auto &id : ids)
    {
        auto pId = req.add_ids();
        pId->set_node_id(id.first);
        for (auto &container_id : id.second)
        {
            pId->add_container_ids(container_id);
        }
    }

    RPC_ASYNC(container::StartReply, _startContainer, startContainerFinished, objId, req);
}

void InfoWorker::stopContainer(const QString objId, const std::map<int64_t, std::vector<std::string>> &ids)
{
    container::StopRequest req;
    for (auto &id : ids)
    {
        auto pId = req.add_ids();
        pId->set_node_id(id.first);
        for (auto &container_id : id.second)
        {
            pId->add_container_ids(container_id);
        }
    }

    RPC_ASYNC(container::StopReply, _stopContainer, stopContainerFinished, objId, req);
}

//void InfoWorker::killContainer(const QString objId, const std::map<int64_t, std::vector<std::string>> &ids)
//{
//    container::KillRequest req;
//    for (auto &id : ids)
//    {
//        auto pId = req.add_ids();
//        pId->set_node_id(id.first);
//        for (auto &container_id : id.second)
//        {
//            pId->add_container_ids(container_id);
//        }
//    }

//    RPC_ASYNC(container::KillReply, _killContainer, killContainerFinished, objId, req);
//}

void InfoWorker::restartContainer(const QString objId, const std::map<int64_t, std::vector<std::string>> &ids)
{
    container::RestartRequest req;
    for (auto &id : ids)
    {
        auto pId = req.add_ids();
        pId->set_node_id(id.first);
        for (auto &container_id : id.second)
        {
            pId->add_container_ids(container_id);
        }
    }

    RPC_ASYNC(container::RestartReply, _restartContainer, restartContainerFinished, objId, req);
}

void InfoWorker::updateContainer(const QString objId, const container::UpdateRequest &req)
{
    RPC_ASYNC(container::UpdateReply, _updateContainer, updateContainerFinished, objId, req);
}

void InfoWorker::monitorHistory(const QString objId, int node_id, int start_time, int end_time, uint32_t interval, std::string container_id)
{
    container::MonitorHistoryRequest req;
    req.set_node_id(node_id);
    req.set_start_time(start_time);
    req.set_end_time(end_time);
    req.set_interval(interval);
    req.set_container_id(container_id);
    RPC_ASYNC(container::MonitorHistoryReply, _monitorHistory, monitorHistoryFinished, objId, req);
}

void InfoWorker::listTemplate(const QString objId, const int perPage, const int nextPage, const std::string sort, const std::string likeSearch)
{
    container::ListTemplateRequest req;
    req.set_per_page(perPage);
    req.set_next_page(nextPage);
    req.set_sort(sort);
    req.set_like_search(likeSearch);
    RPC_ASYNC(container::ListTemplateReply, _listTemplate, listTemplateFinished, objId, req);
}

void InfoWorker::listTemplate(const QString objId)
{
    container::ListTemplateRequest req;
    RPC_ASYNC(container::ListTemplateReply, _listTemplate, listTemplateFinished, objId, req);
}

void InfoWorker::inspectTemplate(const QString objId, int64_t id)
{
    container::InspectTemplateRequest req;
    req.set_id(id);
    RPC_ASYNC(container::InspectTemplateReply, _inspectTemplate, inspectTemplateFinished, objId, req);
}

void InfoWorker::createTemplate(const QString objId, const container::CreateTemplateRequest &req)
{
    RPC_ASYNC(container::CreateTemplateReply, _createTemplate, createTemplateFinished, objId, req);
}

void InfoWorker::updateTemplate(const QString objId, const container::UpdateTemplateRequest &req)
{
    RPC_ASYNC(container::UpdateTemplateReply, _updateTemplate, updateTemplateFinished, objId, req);
}

void InfoWorker::removeTemplate(const QString objId, QList<int64_t> ids)
{
    container::RemoveTemplateRequest req;
    foreach (int64_t id, ids)
    {
        req.add_ids(id);
    }
    RPC_ASYNC(container::RemoveTemplateReply, _removeTemplate, removeTemplateFinished, objId, req);
}

void InfoWorker::listBackup(const QString objId, int nodeId, std::string containerId)
{
    container::ListBackupRequest req;
    req.set_node_id(nodeId);
    req.set_container_id(containerId);
    RPC_ASYNC(container::ListBackupReply, _listBackup, listBackupFinished, objId, req);
}

void InfoWorker::updateBackup(const QString objId, int nodeId, int id, std::string backupDesc)
{
    container::UpdateBackupRequest req;
    req.set_node_id(nodeId);
    req.set_id(id);
    req.set_backup_desc(backupDesc);
    RPC_ASYNC(container::UpdateBackupReply, _updateBackup, updateBackupFinished, objId, req);
}

void InfoWorker::createBackup(const QString objId, int nodeId, std::string containerId, std::string backupDesc)
{
    container::CreateBackupRequest req;
    req.set_node_id(nodeId);
    req.set_container_id(containerId);
    req.set_backup_desc(backupDesc);
    RPC_ASYNC(container::CreateBackupReply, _createBackup, createBackupFinished, objId, req);
}

void InfoWorker::removeBackup(const QString objId, int nodeId, int64_t id)
{
    container::RemoveBackupRequest req;
    req.set_node_id(nodeId);
    req.set_id(id);
    RPC_ASYNC(container::RemoveBackupReply, _removeBackup, removeBackupFinished, objId, req);
}

void InfoWorker::resumeBackup(const QString objId, int nodeId, std::string containerId, int backupId)
{
    container::ResumeBackupRequest req;
    req.set_node_id(nodeId);
    req.set_container_id(containerId);
    req.set_backup_id(backupId);
    RPC_ASYNC(container::ResumeBackupReply, _resumeBackup, resumeBackupFinished, objId, req);
}

void InfoWorker::removeContainer(const QString objId, const std::map<int64_t, std::vector<std::string>> &ids)
{
    container::RemoveRequest req;
    for (auto &id : ids)
    {
        auto pId = req.add_ids();
        pId->set_node_id(id.first);
        for (auto &container_id : id.second)
        {
            pId->add_container_ids(container_id);
        }
    }
    RPC_ASYNC(container::RemoveReply, _removeContainer, removeContainerFinished, objId, req);
}

void InfoWorker::listRuntimeLogging(const QString objId, const logging::ListRuntimeRequest &req)
{
    RPC_ASYNC(logging::ListRuntimeReply, _listRuntimeLogging, loggingRuntimeFinished, objId, req);
}

void InfoWorker::listWarnLogging(const QString objId, const logging::ListWarnRequest &req)
{
    RPC_ASYNC(logging::ListWarnReply, _listWarnLogging, loggingListWarnFinished, objId, req);
}

void InfoWorker::readWarnLogging(const QString objId, QList<int64_t> ids)
{
    logging::ReadWarnRequest req;
    foreach (int64_t id, ids)
    {
        req.add_ids(id);
    }
    RPC_ASYNC(logging::ReadWarnReply, _listReadWarnLogging, loggingReadWarnFinished, objId, req);
}

void InfoWorker::listNetwork(const QString objId, const int64_t node_id)
{
    network::ListRequest req;
    req.set_node_id(node_id);
    RPC_ASYNC(network::ListReply, _listNetwork, listNetworkFinished, objId, req);
}

void InfoWorker::connectNetwork(const QString objId, const network::ConnectRequest &req)
{
    RPC_ASYNC(network::ConnectReply, _connectNetwork, connectNetworkFinished, objId, req);
}

void InfoWorker::disconnectNetwork(const QString objId, const int64_t node_id, std::string interface, std::string container_id)
{
    network::DisconnectRequest req;
    req.set_node_id(node_id);
    req.set_interface(interface);
    req.set_container_id(container_id);
    RPC_ASYNC(network::DisconnectReply, _disconnectNetwork, disconnectNetworkFinished, objId, req);
}

void InfoWorker::listIPtables(const QString objId, const int64_t node_id, std::string container_id)
{
    network::ListIPtablesRequest req;
    req.set_node_id(node_id);
    req.set_container_id(container_id);
    RPC_ASYNC(network::ListIPtablesReply, _listIPtables, listIPtablesFinished, objId, req);
}

void InfoWorker::enableIPtables(const QString objId, const int64_t node_id, bool enable, std::string container_id)
{
    network::EnableIPtablesRequest req;
    req.set_node_id(node_id);
    req.set_container_id(container_id);
    req.set_enable(enable);
    RPC_ASYNC(network::EnableIPtablesReply, _enableIPtables, enableIPtablesFinished, objId, req);
}

void InfoWorker::createIPtables(const QString objId, const network::CreateIPtablesRequest &req)
{
    RPC_ASYNC(network::CreateIPtablesReply, _createIPtables, createIPtablesFinished, objId, req);
}

void InfoWorker::modifyIPtables(const QString objId, const network::ModifyIPtablesRequest &req)
{
    RPC_ASYNC(network::ModifyIPtablesReply, _modifyIPtables, modifyIPtablesFinished, objId, req);
}

void InfoWorker::removeIPtables(const QString objId, const network::RemoveIPtablesRequest &req)
{
    RPC_ASYNC(network::RemoveIPtablesReply, _removeIPtables, removeIPtablesFinished, objId, req);
}

void InfoWorker::listImage(const QString objId, const int64_t node_id)
{
    image::ListRequest req;
    req.set_node_id(node_id);
    RPC_ASYNC(image::ListReply, _listImage, listImageFinished, objId, req);
}

void InfoWorker::listDBImage(const QString objId)
{
    image::ListDBRequest req;
    RPC_ASYNC(image::ListDBReply, _listDBImage, listDBImageFinished, objId, req);
}

void InfoWorker::uploadImage(const QString objId, image::UploadRequest &req, const QString &imageFile, const QString &signFile)
{
    RPC_ASYNC(image::UploadReply, _uploadImage, uploadFinished, objId, req, imageFile, signFile);
}

void InfoWorker::updateImage(const QString objId, image::UpdateRequest &req, const QString &imageFile, const QString &signFile)
{
    RPC_ASYNC(image::UpdateReply, _updateImage, updateFinished, objId, req, imageFile, signFile);
}

void InfoWorker::downloadImage(const QString objId, const int64_t &image_id, const QString &name, const QString &version, const QString &savePath)
{
    image::DownloadRequest req;
    RPC_ASYNC(downloadImageInfo, _downloadImage, downloadImageFinished, objId, req, image_id, name, version, savePath);
}

void InfoWorker::checkImage(const QString objId, const int64_t image_id, const bool approve, const std::string reject_reason)
{
    image::ApproveRequest req;
    req.set_image_id(image_id);
    req.set_approve(approve);
    req.set_reject_reason(reject_reason);
    RPC_ASYNC(image::ApproveReply, _checkImage, checkImageFinished, objId, req);
}

void InfoWorker::removeImage(const QString objId, const std::vector<int64_t> &image_ids)
{
    image::RemoveRequest req;
    for (auto &id : image_ids)
    {
        req.add_image_ids(id);
    }

    RPC_ASYNC(image::RemoveReply, _removeImage, removeImageFinished, objId, req);
}

void InfoWorker::login(const QString objId, const std::string &username, const std::string &password)
{
    user::LoginRequest req;
    req.set_username(username);
    req.set_password(password);
    RPC_ASYNC(user::LoginReply, _login, loginFinished, objId, req);
}

void InfoWorker::logout(const QString objId)
{
    user::LogoutRequest req;
    RPC_ASYNC(user::LogoutReply, _logout, logoutFinished, objId, req);
}

void InfoWorker::updatePassword(const QString objId, const std::string oldPassword, const std::string newPassword)
{
    user::UpdatePasswordRequest req;
    req.set_old_password(oldPassword);
    req.set_new_password(newPassword);
    RPC_ASYNC(user::UpdatePasswordReply, _updatePassword, updatePasswordFinished, objId, req);
}

void InfoWorker::stopTransfer(QString name, QString version, bool isStop)
{
    QMutexLocker locker(&mutex);
    m_transferStatusMap.insert(name + "-" + version, isStop);
}

bool InfoWorker::isTransferStoped(QString name, QString version)
{
    if (m_transferStatusMap.contains(name + "-" + version))
        return m_transferStatusMap.value(name + "-" + version);
    else
        return false;
}

QPair<grpc::Status, node::ListReply> InfoWorker::_listNode(const node::ListRequest &req)
{
    RPC_IMPL(node::ListReply, node::Node::NewStub, List);
}

QPair<grpc::Status, container::ListReply> InfoWorker::_listContainer(const container::ListRequest &req)
{
    RPC_IMPL(container::ListReply, container::Container::NewStub, List);
}

QPair<grpc::Status, node::CreateReply> InfoWorker::_createNode(const node::CreateRequest &req)
{
    RPC_IMPL(node::CreateReply, node::Node::NewStub, Create);
}

QPair<grpc::Status, container::CreateReply> InfoWorker::_createContainer(const container::CreateRequest &req)
{
    RPC_IMPL(container::CreateReply, container::Container::NewStub, Create);
}

//QPair<grpc::Status, container::StatusReply> InfoWorker::_containerStatus(const container::StatusRequest &req)
//{
//    RPC_IMPL(container::StatusReply, container::Container::NewStub, Status);
//}

QPair<grpc::Status, node::RemoveReply> InfoWorker::_removeNode(const node::RemoveRequest &req)
{
    RPC_IMPL(node::RemoveReply, node::Node::NewStub, Remove);
}

QPair<grpc::Status, node::StatusReply> InfoWorker::_nodeStatus(const node::StatusRequest &req)
{
    RPC_IMPL(node::StatusReply, node::Node::NewStub, Status);
}

QPair<grpc::Status, node::UpdateReply> InfoWorker::_updateNode(const node::UpdateRequest &req)
{
    RPC_IMPL(node::UpdateReply, node::Node::NewStub, Update);
}

QPair<grpc::Status, container::InspectReply> InfoWorker::_containerInspect(const container::InspectRequest &req)
{
    RPC_IMPL(container::InspectReply, container::Container::NewStub, Inspect);
}

QPair<grpc::Status, container::StartReply> InfoWorker::_startContainer(const container::StartRequest &req)
{
    RPC_IMPL(container::StartReply, container::Container::NewStub, Start);
}

QPair<grpc::Status, container::StopReply> InfoWorker::_stopContainer(const container::StopRequest &req)
{
    RPC_IMPL(container::StopReply, container::Container::NewStub, Stop);
}

//QPair<grpc::Status, container::KillReply> InfoWorker::_killContainer(const container::KillRequest &req)
//{
//    RPC_IMPL(container::KillReply, container::Container::NewStub, Kill);
//}

QPair<grpc::Status, container::RestartReply> InfoWorker::_restartContainer(const container::RestartRequest &req)
{
    RPC_IMPL(container::RestartReply, container::Container::NewStub, Restart);
}

QPair<grpc::Status, container::UpdateReply> InfoWorker::_updateContainer(const container::UpdateRequest &req)
{
    RPC_IMPL(container::UpdateReply, container::Container::NewStub, Update);
}

QPair<grpc::Status, container::RemoveReply> InfoWorker::_removeContainer(const container::RemoveRequest &req)
{
    RPC_IMPL(container::RemoveReply, container::Container::NewStub, Remove);
}

QPair<grpc::Status, container::MonitorHistoryReply> InfoWorker::_monitorHistory(const container::MonitorHistoryRequest &req)
{
    RPC_IMPL(container::MonitorHistoryReply, container::Container::NewStub, MonitorHistory);
}

QPair<grpc::Status, container::ListTemplateReply> InfoWorker::_listTemplate(const container::ListTemplateRequest &req)
{
    RPC_IMPL(container::ListTemplateReply, container::Container::NewStub, ListTemplate);
}

QPair<grpc::Status, container::InspectTemplateReply> InfoWorker::_inspectTemplate(const container::InspectTemplateRequest &req)
{
    RPC_IMPL(container::InspectTemplateReply, container::Container::NewStub, InspectTemplate);
}

QPair<grpc::Status, container::CreateTemplateReply> InfoWorker::_createTemplate(const container::CreateTemplateRequest &req)
{
    RPC_IMPL(container::CreateTemplateReply, container::Container::NewStub, CreateTemplate);
}

QPair<grpc::Status, container::UpdateTemplateReply> InfoWorker::_updateTemplate(const container::UpdateTemplateRequest &req)
{
    RPC_IMPL(container::UpdateTemplateReply, container::Container::NewStub, UpdateTemplate);
}

QPair<grpc::Status, container::RemoveTemplateReply> InfoWorker::_removeTemplate(const container::RemoveTemplateRequest &req)
{
    RPC_IMPL(container::RemoveTemplateReply, container::Container::NewStub, RemoveTemplate);
}

QPair<grpc::Status, container::ListBackupReply> InfoWorker::_listBackup(const container::ListBackupRequest &req)
{
    RPC_IMPL(container::ListBackupReply, container::Container::NewStub, ListBackup);
}

QPair<grpc::Status, container::UpdateBackupReply> InfoWorker::_updateBackup(const container::UpdateBackupRequest &req)
{
    RPC_IMPL(container::UpdateBackupReply, container::Container::NewStub, UpdateBackup);
}

QPair<grpc::Status, container::CreateBackupReply> InfoWorker::_createBackup(const container::CreateBackupRequest &req)
{
    RPC_IMPL(container::CreateBackupReply, container::Container::NewStub, CreateBackup);
}

QPair<grpc::Status, container::ResumeBackupReply> InfoWorker::_resumeBackup(const container::ResumeBackupRequest &req)
{
    RPC_IMPL(container::ResumeBackupReply, container::Container::NewStub, ResumeBackup);
}

QPair<grpc::Status, container::RemoveBackupReply> InfoWorker::_removeBackup(const container::RemoveBackupRequest &req)
{
    RPC_IMPL(container::RemoveBackupReply, container::Container::NewStub, RemoveBackup);
}

QPair<grpc::Status, logging::ListRuntimeReply> InfoWorker::_listRuntimeLogging(const logging::ListRuntimeRequest &req)
{
    RPC_IMPL(logging::ListRuntimeReply, logging::Logging::NewStub, ListRuntime);
}

QPair<grpc::Status, logging::ListWarnReply> InfoWorker::_listWarnLogging(const logging::ListWarnRequest &req)
{
    RPC_IMPL(logging::ListWarnReply, logging::Logging::NewStub, ListWarn);
}

QPair<grpc::Status, logging::ReadWarnReply> InfoWorker::_listReadWarnLogging(const logging::ReadWarnRequest &req)
{
    RPC_IMPL(logging::ReadWarnReply, logging::Logging::NewStub, ReadWarn);
}

QPair<grpc::Status, network::ListReply> InfoWorker::_listNetwork(const network::ListRequest &req)
{
    RPC_IMPL(network::ListReply, network::Network::NewStub, List);
}

QPair<grpc::Status, network::ConnectReply> InfoWorker::_connectNetwork(const network::ConnectRequest &req)
{
    RPC_IMPL(network::ConnectReply, network::Network::NewStub, Connect);
}

QPair<grpc::Status, network::DisconnectReply> InfoWorker::_disconnectNetwork(const network::DisconnectRequest &req)
{
    RPC_IMPL(network::DisconnectReply, network::Network::NewStub, Disconnect);
}

QPair<grpc::Status, network::ListIPtablesReply> InfoWorker::_listIPtables(const network::ListIPtablesRequest &req)
{
    RPC_IMPL(network::ListIPtablesReply, network::Network::NewStub, ListIPtables);
}

QPair<grpc::Status, network::EnableIPtablesReply> InfoWorker::_enableIPtables(const network::EnableIPtablesRequest &req)
{
    RPC_IMPL(network::EnableIPtablesReply, network::Network::NewStub, EnableIPtables);
}

QPair<grpc::Status, network::CreateIPtablesReply> InfoWorker::_createIPtables(const network::CreateIPtablesRequest &req)
{
    RPC_IMPL(network::CreateIPtablesReply, network::Network::NewStub, CreateIPtables);
}

QPair<grpc::Status, network::ModifyIPtablesReply> InfoWorker::_modifyIPtables(const network::ModifyIPtablesRequest &req)
{
    RPC_IMPL(network::ModifyIPtablesReply, network::Network::NewStub, ModifyIPtables);
}

QPair<grpc::Status, network::RemoveIPtablesReply> InfoWorker::_removeIPtables(const network::RemoveIPtablesRequest &req)
{
    RPC_IMPL(network::RemoveIPtablesReply, network::Network::NewStub, RemoveIPtables);
}

QPair<grpc::Status, image::ListReply> InfoWorker::_listImage(const image::ListRequest &req)
{
    RPC_IMPL(image::ListReply, image::Image::NewStub, List);
}

QPair<grpc::Status, image::ListDBReply> InfoWorker::_listDBImage(const image::ListDBRequest &req)
{
    RPC_IMPL(image::ListDBReply, image::Image::NewStub, ListDB);
}

QPair<grpc::Status, image::UploadReply> InfoWorker::_uploadImage(image::UploadRequest &req, const QString &imageFile, const QString &signFile)
{
    QPair<grpc::Status, image::UploadReply> r;
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());
    if (!chan)
    {
        KLOG_INFO() << "uploadImage failed to get connection";
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        emit InfoWorker::getInstance().transferImageFinished(QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()));
        return r;
    }

    grpc::ClientContext context;
    if (s_authKey.size() > 0)
        context.AddMetadata("authorization", s_authKey);

    auto stream = image::Image::NewStub(chan)->Upload(&context, &r.second);
    bool ret = stream->Write(req);
    if (!ret)
    {
        KLOG_INFO() << "send param err";
        r.first = grpc::Status(grpc::StatusCode::INTERNAL,
                               QObject::tr("Internal Error").toStdString());
        emit InfoWorker::getInstance().transferImageFinished(QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()));
        return r;
    }

    QFile file(imageFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        KLOG_INFO() << "Failed to open " << imageFile;
        r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                               QObject::tr("Invalid Argument").toStdString());
        emit InfoWorker::getInstance().transferImageFinished(QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()));
        return r;
    }

    QFile filesign(signFile);
    if (!filesign.open(QIODevice::ReadOnly))
    {
        KLOG_INFO() << "Failed to open " << signFile;
        r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                               QObject::tr("Invalid Argument").toStdString());
        file.close();
        emit InfoWorker::getInstance().transferImageFinished(QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()));
        return r;
    }

    char *pBuf = new char[CHUNK_SIZE];
    qint64 readret = filesign.read(pBuf, CHUNK_SIZE);
    req.set_chunk_data(pBuf, (size_t)readret);
    double totalCnt = ceil(req.info().size() / 1048576.0);
    int curCnt = 0;
    QString imageInfo = QString::fromStdString(req.info().name()) + "-" + QString::fromStdString(req.info().version());
    if (stream->Write(req))
    {
        int progess = 0;
        while (!file.atEnd())
        {
            if (InfoWorker::getInstance().m_transferStatusMap.contains(imageInfo))
            {
                if (!InfoWorker::getInstance().m_transferStatusMap.value(imageInfo))
                {
                    readret = file.read(pBuf, CHUNK_SIZE);
                    req.mutable_chunk_data();
                    req.set_chunk_data(pBuf, (size_t)readret);
                    if (!stream->Write(req))
                    {
                        KLOG_INFO() << "Broken stream, curCnt:" << curCnt;
                        emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), progess);
                        break;
                    }
                    int tmp = int(floor((curCnt / totalCnt) * 100));
                    if (progess != tmp)
                    {
                        emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), progess);
                    }
                    progess = tmp;
                    curCnt++;
                }
                else
                {
                    KLOG_INFO() << "Transmission interruption";
                    context.TryCancel();
                    emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), progess);
                    break;
                }
            }
        }
    }
    else
    {
        KLOG_INFO() << "sign file broken stream";
        emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), 0);
    }

    file.close();
    filesign.close();
    stream->WritesDone();
    r.first = stream->Finish();
    delete[] pBuf;

    emit InfoWorker::getInstance().transferImageFinished(QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()));
    KLOG_INFO() << "return:" << r.first.error_code() << r.second.image_id();

    if (curCnt == (int)totalCnt)
    {
        emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_SUCCESSFUL, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), 100);
    }
    return r;
}

QPair<grpc::Status, image::UpdateReply> InfoWorker::_updateImage(image::UpdateRequest &req, const QString &imageFile, const QString &signFile)
{
    QPair<grpc::Status, image::UpdateReply> r;
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());
    if (!chan)
    {
        KLOG_INFO() << "update image failed to get connection";
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        emit InfoWorker::getInstance().transferImageFinished(QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()));
        return r;
    }

    grpc::ClientContext context;
    if (s_authKey.size() > 0)
        context.AddMetadata("authorization", s_authKey);

    auto stream = image::Image::NewStub(chan)->Update(&context, &r.second);
    bool ret = stream->Write(req);
    if (!ret)
    {
        KLOG_INFO() << "send param err";
        r.first = grpc::Status(grpc::StatusCode::INTERNAL,
                               QObject::tr("Internal Error").toStdString());
        emit InfoWorker::getInstance().transferImageFinished(QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()));
        return r;
    }

    if (!imageFile.isEmpty() && !signFile.isEmpty())
    {
        QFile file(imageFile);
        if (!file.open(QIODevice::ReadOnly))
        {
            KLOG_INFO() << "Failed to open " << imageFile;
            r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                   QObject::tr("Invalid Argument").toStdString());
            emit InfoWorker::getInstance().transferImageFinished(QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()));
            return r;
        }

        QFile filesign(signFile);
        if (!filesign.open(QIODevice::ReadOnly))
        {
            KLOG_INFO() << "Failed to open " << signFile;
            r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                   QObject::tr("Invalid Argument").toStdString());
            file.close();
            emit InfoWorker::getInstance().transferImageFinished(QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()));
            return r;
        }

        char *pBuf = new char[CHUNK_SIZE];
        qint64 readret = filesign.read(pBuf, CHUNK_SIZE);
        req.set_chunk_data(pBuf, (size_t)readret);
        double totalCnt = ceil(req.info().size() / 1048576.0);
        int curCnt = 0;
        QString imageInfo = QString::fromStdString(req.info().name()) + "-" + QString::fromStdString(req.info().version());
        if (stream->Write(req))
        {
            int progess = 0;
            while (!file.atEnd())
            {
                if (InfoWorker::getInstance().m_transferStatusMap.contains(imageInfo))
                {
                    if (!InfoWorker::getInstance().m_transferStatusMap.value(imageInfo))
                    {
                        readret = file.read(pBuf, CHUNK_SIZE);
                        req.mutable_chunk_data();
                        req.set_chunk_data(pBuf, (size_t)readret);
                        if (!stream->Write(req))
                        {
                            KLOG_INFO() << "Broken stream, curCnt:" << curCnt;
                            emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), progess);
                            break;
                        }
                        int tmp = int(floor((curCnt / totalCnt) * 100));
                        if (progess != tmp)
                        {
                            emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), progess);
                        }
                        progess = tmp;
                        curCnt++;
                    }
                    else
                    {
                        KLOG_INFO() << "Transmission interruption";
                        context.TryCancel();
                        emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), progess);
                        break;
                    }
                }
            }
        }
        else
        {
            KLOG_INFO() << "sign file broken stream";
            emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), 0);
        }

        file.close();
        filesign.close();
        delete[] pBuf;

        if (curCnt == (int)totalCnt)
        {
            emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_SUCCESSFUL, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), 100);
        }
    }
    stream->WritesDone();
    r.first = stream->Finish();

    emit InfoWorker::getInstance().transferImageFinished(QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()));
    KLOG_INFO() << "return:" << r.first.error_code();
    return r;
}

QPair<grpc::Status, downloadImageInfo> InfoWorker::_downloadImage(image::DownloadRequest &req, const int64_t &image_id, const QString &name, const QString &version, const QString &savePath)
{
    QPair<grpc::Status, downloadImageInfo> r;
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());
    if (!chan)
    {
        KLOG_INFO() << "uploadImage failed to get connection";
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        emit InfoWorker::getInstance().transferImageFinished(name, version);
        return r;
    }

    image::DownloadReply reply;
    grpc::ClientContext context;
    if (s_authKey.size() > 0)
        context.AddMetadata("authorization", s_authKey);

    req.set_image_id(image_id);

    auto stream = image::Image::NewStub(chan)->Download(&context, req);
    bool ret = stream->Read(&reply);
    if (!ret)
    {
        KLOG_INFO() << "recv param err";
        r.first = stream->Finish();
        emit InfoWorker::getInstance().transferImageFinished(name, version);
        return r;
    }

    auto outName = reply.info().name();
    auto outVersion = reply.info().version();
    auto outType = reply.info().type();
    auto outChecksum = reply.info().checksum();
    auto outSize = reply.info().size();
    auto outDescription = reply.info().description();
    KLOG_INFO() << "recv param:" << outName.data() << outVersion.data()
                << outType.data() << outChecksum.data() << outSize << outDescription.data();

    QString wFileName = QString("%1%2_%3%4").arg(savePath).arg(outName.data()).arg(outVersion.data()).arg(outType.data());
    QFile wFile(wFileName);
    if (!wFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        KLOG_INFO() << "Failed to open " << wFileName.data();
        r.first = grpc::Status(grpc::StatusCode::INTERNAL,
                               QObject::tr("Failed to open %1").arg(wFileName.data()).toStdString());
        emit InfoWorker::getInstance().transferImageFinished(name, version);
        return r;
    }

    double totalCnt = ceil(outSize / 1048576.0);
    int curCnt = 0;
    int progess = 0;
    QString imageInfo = QString::fromStdString(outName) + "-" + QString::fromStdString(outVersion);
    while (stream->Read(&reply))
    {
        if (InfoWorker::getInstance().m_transferStatusMap.contains(imageInfo))
        {
            if (!InfoWorker::getInstance().m_transferStatusMap.value(imageInfo))
            {
                wFile.write(reply.chunk_data().data(), qint64(reply.chunk_data().size()));
                int tmp = int(floor((curCnt / totalCnt) * 100));
                if (progess != tmp)
                {
                    KLOG_INFO("download progess:%d, cnt:%d, %lf\n", progess, curCnt, totalCnt);
                    emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_DOWNLOADING, QString::fromStdString(outName), QString::fromStdString(outVersion), progess);
                }
                progess = tmp;
                curCnt++;
            }
            else
            {
                wFile.close();
                r.first = grpc::Status(grpc::StatusCode::ABORTED, QObject::tr("Cancel").toStdString());
                emit InfoWorker::getInstance().transferImageFinished(name, version);
                return r;
            }
        }
    }
    wFile.close();

    QString message;
    ImageTransmissionStatus status;
    grpc::StatusCode statusCode;
    //检测数据库中文件是否损坏
    do
    {
        QFile file(wFileName);
        QByteArray fileArray;
        if (!file.open(QIODevice::ReadOnly))
        {
            KLOG_INFO() << "Failed to open " << wFileName;
            message = tr("Failed to open %1!").arg(wFileName);
            statusCode = grpc::StatusCode::INTERNAL;
            status = IMAGE_TRANSMISSION_STATUS_DOWNLOADING_FAILED;
            progess = 99;
            break;
        }
        auto fileSize = file.size();
        fileArray = file.readAll();
        file.close();
        auto strSha256 = QCryptographicHash::hash(fileArray, QCryptographicHash::Sha256).toHex();

        KLOG_INFO() << "strSha256:" << strSha256 << ", fileSize:" << fileSize;
        if (outSize != fileSize || outChecksum != strSha256.toStdString())
        {
            KLOG_INFO() << outSize << fileSize << outChecksum.data() << strSha256.toStdString().data();
            message = tr("file was broken!");
            statusCode = grpc::StatusCode::INTERNAL;
            status = IMAGE_TRANSMISSION_STATUS_DOWNLOADING_FAILED;
            progess = 99;
            break;
        }

        message = tr("Ok");
        statusCode = grpc::StatusCode::OK;
        status = IMAGE_TRANSMISSION_STATUS_DOWNLOADING_SUCCESSFUL;
        progess = 100;

    } while (0);

    r.first = grpc::Status(statusCode, message.toStdString());
    r.second = downloadImageInfo{outName, outVersion, outType, outChecksum, wFileName.toStdString(), outSize};

    emit InfoWorker::getInstance().transferImageStatus(status, QString::fromStdString(outName), QString::fromStdString(outVersion), progess);
    emit InfoWorker::getInstance().transferImageFinished(name, version);
    return r;
}

QPair<grpc::Status, image::ApproveReply> InfoWorker::_checkImage(const image::ApproveRequest &req)
{
    RPC_IMPL(image::ApproveReply, image::Image::NewStub, Approve);
}

QPair<grpc::Status, image::RemoveReply> InfoWorker::_removeImage(const image::RemoveRequest &req)
{
    RPC_IMPL(image::RemoveReply, image::Image::NewStub, Remove);
}

QPair<grpc::Status, user::LoginReply> InfoWorker::_login(const user::LoginRequest &req)
{
    QPair<grpc::Status, user::LoginReply> r;
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());
    if (!chan)
    {
        KLOG_INFO("user::User::NewStub Login failed to get connection");
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        return r;
    }
    grpc::ClientContext ctx;
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(DEADLINE);
    ctx.set_deadline(deadline);
    r.first = user::User::NewStub(chan)->Login(&ctx, req, &r.second);
    if (r.first.ok())
        s_authKey = r.second.auth_key();
    if (grpc::StatusCode::DEADLINE_EXCEEDED == r.first.error_code())
    {
        r.first = grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED,
                               QObject::tr("Response timeout").toStdString());
    }
    return r;
}

QPair<grpc::Status, user::LogoutReply> InfoWorker::_logout(const user::LogoutRequest &req)
{
    QPair<grpc::Status, user::LogoutReply> r;
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());
    if (!chan)
    {
        KLOG_INFO("user::User::NewStub Logout failed to get connection");
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        return r;
    }
    grpc::ClientContext ctx;
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(DEADLINE);
    ctx.set_deadline(deadline);
    if (s_authKey.size() > 0)
        ctx.AddMetadata("authorization", s_authKey);
    r.first = user::User::NewStub(chan)->Logout(&ctx, req, &r.second);
    if (r.first.ok())
        s_authKey = "";
    if (grpc::StatusCode::DEADLINE_EXCEEDED == r.first.error_code())
    {
        r.first = grpc::Status(grpc::StatusCode::DEADLINE_EXCEEDED,
                               QObject::tr("Response timeout").toStdString());
    }
    return r;
}

QPair<grpc::Status, user::UpdatePasswordReply> InfoWorker::_updatePassword(const user::UpdatePasswordRequest &req)
{
    RPC_IMPL(user::UpdatePasswordReply, user::User::NewStub, UpdatePassword);
}
