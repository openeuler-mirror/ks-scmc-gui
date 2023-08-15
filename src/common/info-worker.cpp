#include "info-worker.h"

#include <kiran-log/qt5-log-i.h>

#include <QMutexLocker>
#include <QtConcurrent/QtConcurrent>

static std::string s_authKey = "";

const int CHUNK_SIZE = 1024 * 1024;

#define RPC_ASYNC(REPLY_TYPE, WORKER, CALLBACK, ...)                 \
    typedef QPair<grpc::Status, REPLY_TYPE> T;                       \
    QFutureWatcher<T> *watcher = new QFutureWatcher<T>();            \
    watcher->setFuture(QtConcurrent::run(WORKER, ##__VA_ARGS__));    \
    connect(watcher, &QFutureWatcher<T>::finished, [this, watcher] { \
        auto reply = watcher->result();                              \
        emit CALLBACK(reply);                                        \
        delete watcher;                                              \
    });

#define RPC_IMPL(REPLY_TYPE, STUB, RPC_NAME)                                \
    QPair<grpc::Status, REPLY_TYPE> r;                                      \
    auto chan = get_rpc_channel(g_server_addr);                             \
    if (!chan)                                                              \
    {                                                                       \
        KLOG_INFO("%s %s failed to get connection", #STUB, #RPC_NAME);      \
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,                   \
                               QObject::tr("Network Error").toStdString()); \
        return r;                                                           \
    }                                                                       \
    grpc::ClientContext ctx;                                                \
    if (s_authKey.size() > 0)                                               \
        ctx.AddMetadata("authorization", s_authKey);                        \
    r.first = STUB(chan)->RPC_NAME(&ctx, req, &r.second);                   \
    return r;

InfoWorker::InfoWorker(QObject *parent) : QObject(parent)
{
    QMutexLocker locker(&mutex);
}

void InfoWorker::listNode()
{
    node::ListRequest req;
    RPC_ASYNC(node::ListReply, _listNode, listNodeFinished, req);
}

void InfoWorker::listContainer(const std::vector<int64_t> &node_ids, const bool all)
{
    container::ListRequest req;
    req.set_list_all(all);
    for (auto &id : node_ids)
    {
        req.add_node_ids(id);
    }

    RPC_ASYNC(container::ListReply, _listContainer, listContainerFinished, req);
}

void InfoWorker::createNode(const node::CreateRequest &req)
{
    RPC_ASYNC(node::CreateReply, _createNode, createNodeFinished, req);
}

void InfoWorker::createContainer(const container::CreateRequest &req)
{
    RPC_ASYNC(container::CreateReply, _createContainer, createContainerFinished, req);
}

void InfoWorker::containerStatus(const int64_t node_id)
{
    container::StatusRequest req;
    req.set_node_id(node_id);
    RPC_ASYNC(container::StatusReply, _containerStatus, containerStatusFinished, req);
}

void InfoWorker::removeNode(const std::vector<int64_t> &node_ids)
{
    node::RemoveRequest req;
    for (auto &id : node_ids)
    {
        req.add_ids(id);
    }

    RPC_ASYNC(node::RemoveReply, _removeNode, removeNodeFinished, req);
}

void InfoWorker::nodeStatus(const std::vector<int64_t> &node_ids)
{
    node::StatusRequest req;
    for (auto &id : node_ids)
    {
        req.add_node_ids(id);
    }

    RPC_ASYNC(node::StatusReply, _nodeStatus, nodeStatusFinished, req);
}

void InfoWorker::containerInspect(const int64_t node_id, const std::string &container_id)
{
    container::InspectRequest req;
    req.set_node_id(node_id);
    req.set_container_id(container_id);
    RPC_ASYNC(container::InspectReply, _containerInspect, containerInspectFinished, req);
}

void InfoWorker::startContainer(const std::map<int64_t, std::vector<std::string>> &ids)
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

    RPC_ASYNC(container::StartReply, _startContainer, startContainerFinished, req);
}

void InfoWorker::stopContainer(const std::map<int64_t, std::vector<std::string>> &ids)
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

    RPC_ASYNC(container::StopReply, _stopContainer, stopContainerFinished, req);
}

void InfoWorker::killContainer(const std::map<int64_t, std::vector<std::string>> &ids)
{
    container::KillRequest req;
    for (auto &id : ids)
    {
        auto pId = req.add_ids();
        pId->set_node_id(id.first);
        for (auto &container_id : id.second)
        {
            pId->add_container_ids(container_id);
        }
    }

    RPC_ASYNC(container::KillReply, _killContainer, killContainerFinished, req);
}

void InfoWorker::restartContainer(const std::map<int64_t, std::vector<std::string>> &ids)
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

    RPC_ASYNC(container::RestartReply, _restartContainer, restartContainerFinished, req);
}

void InfoWorker::updateContainer(const container::UpdateRequest &req)
{
    RPC_ASYNC(container::UpdateReply, _updateContainer, updateContainerFinished, req);
}

void InfoWorker::removeContainer(const std::map<int64_t, std::vector<std::string>> &ids)
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
    RPC_ASYNC(container::RemoveReply, _removeContainer, removeContainerFinished, req);
}

void InfoWorker::listNetwork(const int64_t node_id)
{
    network::ListRequest req;
    req.set_node_id(node_id);
    RPC_ASYNC(network::ListReply, _listNetwork, listNetworkFinished, req);
}

void InfoWorker::listImage(const int64_t node_id)
{
    image::ListRequest req;
    req.set_node_id(node_id);
    RPC_ASYNC(image::ListReply, _listImage, listImageFinished, req);
}

void InfoWorker::listDBImage()
{
    image::ListDBRequest req;
    RPC_ASYNC(image::ListDBReply, _listDBImage, listDBImageFinished, req);
}

QPair<grpc::Status, image::UploadReply> InfoWorker::uploadImage(image::UploadRequest &req, const QString &imageFile)
{
    QPair<grpc::Status, image::UploadReply> r;
    auto chan = get_rpc_channel(g_server_addr);
    if (!chan)
    {
        KLOG_INFO() << "uploadImage failed to get connection";
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        return r;
    }

    grpc::ClientContext context;
    image::UploadReply reply;
    auto stream = image::Image::NewStub(chan)->Upload(&context, &reply);
    bool ret = stream->Write(req);
    if (!ret)
    {
        KLOG_INFO() << "send param err";
        r.first = grpc::Status(grpc::StatusCode::INTERNAL,
                               QObject::tr("Internal Error").toStdString());
        return r;
    }

    QFile file(imageFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        KLOG_INFO() << "Failed to open " << imageFile;
        r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                               QObject::tr("Invalid Argument").toStdString());
        return r;
    }

    char *pBuf = new char[CHUNK_SIZE];
    while (!file.atEnd())
    {
        qint64 ret = file.read(pBuf, CHUNK_SIZE);
        req.mutable_chunk_data();
        req.set_chunk_data(pBuf, (size_t)ret);
        if (!stream->Write(req))
        {
            KLOG_INFO() << "Broken stream";
            break;
        }
    }

    file.close();
    stream->WritesDone();
    r.first = stream->Finish();
    r.second = reply;
    delete[] pBuf;
    KLOG_INFO() << "return:" << r.first.error_code() << r.second.image_id();

    return r;
}

QPair<grpc::Status, image::UpdateReply> InfoWorker::updateImage(image::UpdateRequest &req, const QString &imageFile)
{
    QPair<grpc::Status, image::UpdateReply> r;
    auto chan = get_rpc_channel(g_server_addr);
    if (!chan)
    {
        KLOG_INFO() << "uploadImage failed to get connection";
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        return r;
    }

    grpc::ClientContext context;
    image::UpdateReply reply;
    auto stream = image::Image::NewStub(chan)->Update(&context, &reply);
    bool ret = stream->Write(req);
    if (!ret)
    {
        KLOG_INFO() << "send param err";
        r.first = grpc::Status(grpc::StatusCode::INTERNAL,
                               QObject::tr("Internal Error").toStdString());
        return r;
    }

    QFile file(imageFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        KLOG_INFO() << "Failed to open " << imageFile;
        r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                               QObject::tr("Invalid Argument").toStdString());
        return r;
    }

    char *pBuf = new char[CHUNK_SIZE];
    while (!file.atEnd())
    {
        qint64 ret = file.read(pBuf, CHUNK_SIZE);
        req.mutable_chunk_data();
        req.set_chunk_data(pBuf, (size_t)ret);
        if (!stream->Write(req))
        {
            KLOG_INFO() << "Broken stream";
            break;
        }
    }

    file.close();
    stream->WritesDone();
    r.first = stream->Finish();
    r.second = reply;
    delete[] pBuf;
    KLOG_INFO() << "return:" << r.first.error_code();

    return r;
}

QPair<grpc::Status, downloadImageInfo> InfoWorker::downloadImage(const int64_t image_id, const QString &savePath)
{
    QPair<grpc::Status, downloadImageInfo> r;
    auto chan = get_rpc_channel(g_server_addr);
    if (!chan)
    {
        KLOG_INFO() << "uploadImage failed to get connection";
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        return r;
    }

    image::DownloadReply reply;
    grpc::ClientContext context;
    image::DownloadRequest req;
    req.set_image_id(image_id);

    auto stream = image::Image::NewStub(chan)->Download(&context, req);
    bool ret = stream->Read(&reply);
    if (!ret)
    {
        KLOG_INFO() << "recv param err";
        r.first = grpc::Status(grpc::StatusCode::INTERNAL,
                               QObject::tr("Internal Error").toStdString());
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
                               QObject::tr("Internal Error").toStdString());
        return r;
    }

    while (stream->Read(&reply))
    {
        wFile.write(reply.chunk_data().data(), qint64(reply.chunk_data().size()));
    }

    wFile.close();
    r.first = grpc::Status(grpc::StatusCode::OK, QObject::tr("OK").toStdString());
    r.second = downloadImageInfo{outName, outVersion, outType, outChecksum, wFileName.toStdString(), outSize};
    return r;
}

void InfoWorker::checkImage(const int64_t image_id, const bool approve, const std::string reject_reason)
{
    image::ApproveRequest req;
    req.set_image_id(image_id);
    req.set_approve(approve);
    req.set_reject_reason(reject_reason);
    RPC_ASYNC(image::ApproveReply, _checkImage, checkImageFinished, req);
}

void InfoWorker::removeImage(const std::vector<int64_t> &image_ids)
{
    image::RemoveRequest req;
    for (auto &id : image_ids)
    {
        req.add_image_ids(id);
    }

    RPC_ASYNC(image::RemoveReply, _removeImage, removeImageFinished, req);
}

void InfoWorker::login(const std::string &username, const std::string &password)
{
    user::LoginRequest req;
    req.set_username(username);
    req.set_password(password);
    RPC_ASYNC(user::LoginReply, _login, loginFinished, req);
}

void InfoWorker::logout()
{
    user::LogoutRequest req;
    RPC_ASYNC(user::LogoutReply, _logout, logoutFinished, req);
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

QPair<grpc::Status, container::StatusReply> InfoWorker::_containerStatus(const container::StatusRequest &req)
{
    RPC_IMPL(container::StatusReply, container::Container::NewStub, Status);
}

QPair<grpc::Status, node::RemoveReply> InfoWorker::_removeNode(const node::RemoveRequest &req)
{
    RPC_IMPL(node::RemoveReply, node::Node::NewStub, Remove);
}

QPair<grpc::Status, node::StatusReply> InfoWorker::_nodeStatus(const node::StatusRequest &req)
{
    RPC_IMPL(node::StatusReply, node::Node::NewStub, Status);
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

QPair<grpc::Status, container::KillReply> InfoWorker::_killContainer(const container::KillRequest &req)
{
    RPC_IMPL(container::KillReply, container::Container::NewStub, Kill);
}

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

QPair<grpc::Status, network::ListReply> InfoWorker::_listNetwork(const network::ListRequest &req)
{
    RPC_IMPL(network::ListReply, network::Network::NewStub, List);
}

QPair<grpc::Status, image::ListReply> InfoWorker::_listImage(const image::ListRequest &req)
{
    RPC_IMPL(image::ListReply, image::Image::NewStub, List);
}

QPair<grpc::Status, image::ListDBReply> InfoWorker::_listDBImage(const image::ListDBRequest &req)
{
    RPC_IMPL(image::ListDBReply, image::Image::NewStub, ListDB);
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
    auto chan = get_rpc_channel(g_server_addr);
    if (!chan)
    {
        KLOG_INFO("user::User::NewStub Login failed to get connection");
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        return r;
    }
    grpc::ClientContext ctx;
    r.first = user::User::NewStub(chan)->Login(&ctx, req, &r.second);
    if (r.first.ok())
        s_authKey = r.second.auth_key();
    return r;
}

QPair<grpc::Status, user::LogoutReply> InfoWorker::_logout(const user::LogoutRequest &req)
{
    QPair<grpc::Status, user::LogoutReply> r;
    auto chan = get_rpc_channel(g_server_addr);
    if (!chan)
    {
        KLOG_INFO("user::User::NewStub Logout failed to get connection");
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        return r;
    }
    grpc::ClientContext ctx;
    if (s_authKey.size() > 0)
        ctx.AddMetadata("authorization", s_authKey);
    r.first = user::User::NewStub(chan)->Logout(&ctx, req, &r.second);
    if (r.first.ok())
        s_authKey = "";
    return r;
}
