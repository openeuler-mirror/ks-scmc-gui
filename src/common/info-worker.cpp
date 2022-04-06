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
    qRegisterMetaType<ImageTransmissionStatus>("ImageTransmissionStatus");
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

void InfoWorker::uploadImage(image::UploadRequest &req, const QString &imageFile, const QString &signFile)
{
    RPC_ASYNC(image::UploadReply, _uploadImage, uploadFinished, req, imageFile, signFile);
}

void InfoWorker::updateImage(image::UpdateRequest &req, const QString &imageFile, const QString &signFile)
{
    RPC_ASYNC(image::UpdateReply, _updateImage, updateFinished, req, imageFile, signFile);
}

void InfoWorker::downloadImage(const int64_t &image_id, const QString &savePath)
{
    image::DownloadRequest req;
    RPC_ASYNC(downloadImageInfo, _downloadImage, downloadImageFinished, req, image_id, savePath);
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

void InfoWorker::stopTransfer(QString name, QString version, bool isStop)
{
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

QPair<grpc::Status, image::UploadReply> InfoWorker::_uploadImage(image::UploadRequest &req, const QString &imageFile, const QString &signFile)
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
    if (s_authKey.size() > 0)
        context.AddMetadata("authorization", s_authKey);

    auto stream = image::Image::NewStub(chan)->Upload(&context, &r.second);
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

    QFile filesign(signFile);
    if (!filesign.open(QIODevice::ReadOnly))
    {
        KLOG_INFO() << "Failed to open " << signFile;
        r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                               QObject::tr("Invalid Argument").toStdString());
        file.close();
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
                    r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                           QObject::tr("Invalid Argument").toStdString());
                    file.close();
                    filesign.close();
                    delete[] pBuf;
                    return r;
                    //emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), 0);
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
    if (curCnt == (int)totalCnt)
    {
        emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_SUCCESSFUL, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), 100);
    }
    KLOG_INFO() << "return:" << r.first.error_code() << r.second.image_id();

    return r;
}

QPair<grpc::Status, image::UpdateReply> InfoWorker::_updateImage(image::UpdateRequest &req, const QString &imageFile, const QString &signFile)
{
    QPair<grpc::Status, image::UpdateReply> r;
    auto chan = get_rpc_channel(g_server_addr);
    if (!chan)
    {
        KLOG_INFO() << "update image failed to get connection";
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        return r;
    }

    grpc::ClientContext context;
    auto stream = image::Image::NewStub(chan)->Update(&context, &r.second);
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

    QFile filesign(signFile);
    if (!filesign.open(QIODevice::ReadOnly))
    {
        KLOG_INFO() << "Failed to open " << signFile;
        r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                               QObject::tr("Invalid Argument").toStdString());
        file.close();
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
                    r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                           QObject::tr("Invalid Argument").toStdString());
                    file.close();
                    filesign.close();
                    delete[] pBuf;
                    return r;
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
    KLOG_INFO() << "return:" << r.first.error_code();
    if (curCnt == (int)totalCnt)
    {
        emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_SUCCESSFUL, QString::fromStdString(req.info().name()), QString::fromStdString(req.info().version()), 100);
    }

    return r;
}

QPair<grpc::Status, downloadImageInfo> InfoWorker::_downloadImage(image::DownloadRequest &req, const int64_t &image_id, const QString &savePath)
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
                return r;
            }
        }
    }

    wFile.close();
    r.first = grpc::Status(grpc::StatusCode::OK, QObject::tr("OK").toStdString());
    r.second = downloadImageInfo{outName, outVersion, outType, outChecksum, wFileName.toStdString(), outSize};
    emit InfoWorker::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_DOWNLOADING_SUCCESSFUL, QString::fromStdString(outName), QString::fromStdString(outVersion), 100);
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
