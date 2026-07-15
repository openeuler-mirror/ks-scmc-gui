/**
 * @file          src/base/interface/image.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "image.h"
#include <kiran-log/qt5-log-i.h>
#include "common.h"

#include <QMutexLocker>
#include <QUuid>

const int CHUNK_SIZE = 16 * 1024;

Image::Image(QObject *parent) : QObject(parent)
{
    QMutexLocker locker(&mutex);
}

Image::~Image()
{
}

QString Image::generateId(QObject *callObj)
{
    QString uid = QUuid::createUuid().toString();
    return QString("%1_%2").arg(callObj->metaObject()->className()).arg(uid);
}

void Image::listImage(const QString objId, const int64_t node_id)
{
    image::ListRequest req;
    req.set_node_id(node_id);
    RPC_ASYNC(image::ListReply, _listImage, listImageFinished, objId, req);
}

void Image::listImage(const QString objId)
{
    image::ListRequest req;
    RPC_ASYNC(image::ListReply, _listImage, listImageFinished, objId, req);
}

void Image::listDBImage(const QString objId)
{
    image::ListDBRequest req;
    RPC_ASYNC(image::ListDBReply, _listDBImage, listDBImageFinished, objId, req);
}

void Image::uploadImage(const QString objId, image::UploadRequest &req, const QString &imageFile, const QString &signFile)
{
    RPC_ASYNC(image::UploadReply, _uploadImage, uploadFinished, objId, req, imageFile, signFile);
}

void Image::updateImage(const QString objId, image::UpdateRequest &req, const QString &imageFile, const QString &signFile)
{
    RPC_ASYNC(image::UpdateReply, _updateImage, updateFinished, objId, req, imageFile, signFile);
}

void Image::downloadImage(const QString objId, const int64_t &image_id, const QString &name, const QString &version, const QString &savePath)
{
    image::DownloadRequest req;
    RPC_ASYNC(downloadImageInfo, _downloadImage, downloadImageFinished, objId, req, image_id, name, version, savePath);
}

void Image::checkImage(const QString objId, const int64_t image_id, const bool approve, const std::string &reject_reason)
{
    image::ApproveRequest req;
    req.set_image_id(image_id);
    req.set_approve(approve);
    req.set_reject_reason(reject_reason);
    RPC_ASYNC(image::ApproveReply, _checkImage, checkImageFinished, objId, req);
}

void Image::removeImage(const QString objId, const std::vector<int64_t> &image_ids)
{
    image::RemoveRequest req;
    for (auto &id : image_ids)
    {
        req.add_image_ids(id);
    }

    RPC_ASYNC(image::RemoveReply, _removeImage, removeImageFinished, objId, req);
}

QPair<grpc::Status, image::ListReply> Image::_listImage(const image::ListRequest &req)
{
    RPC_IMPL(image::ListReply, image::Image::NewStub, List);
}

QPair<grpc::Status, image::ListDBReply> Image::_listDBImage(const image::ListDBRequest &req)
{
    RPC_IMPL(image::ListDBReply, image::Image::NewStub, ListDB);
}

QPair<grpc::Status, image::UploadReply> Image::_uploadImage(image::UploadRequest &req, const QString &imageFile, const QString &signFile)
{
    const auto name = QString::fromStdString(req.info().name());
    const auto version = QString::fromStdString(req.info().version());
    QPair<grpc::Status, image::UploadReply> r;

    // 检测rpc连接
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());
    if (!chan)
    {
        KLOG_WARNING() << "UploadImage failed to get connection";
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        emit Common::getInstance().transferImageFinished(name, version);
        return r;
    }

    // 检测用户认证
    grpc::ClientContext context;
    if (Common::getInstance().getAuthKey().size() > 0)
        context.AddMetadata("authorization", Common::getInstance().getAuthKey());

    // 检测镜像文件
    QFile imgFile(imageFile);
    if (!imgFile.open(QIODevice::ReadOnly))
    {
        KLOG_WARNING() << "Failed to open " << imageFile;
        r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                               QObject::tr("Invalid Argument").toStdString());
        emit Common::getInstance().transferImageFinished(name, version);
        return r;
    }

    const auto imgFileSize = imgFile.size();
    int64_t trans(0), progress(0);
    size_t n;
    char buf[CHUNK_SIZE];

    // 检测签名文件
    if (!signFile.isEmpty())
    {
        QFile sigFile(signFile);
        if (!sigFile.open(QIODevice::ReadOnly) || sigFile.size() > 8192 || sigFile.size() == 0)
        {
            KLOG_WARNING() << "Failed to open " << signFile;
            imgFile.close();
            r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                   QObject::tr("Invalid Argument").toStdString());
            emit Common::getInstance().transferImageFinished(name, version);
            return r;
        }

        auto signContent = sigFile.readAll();
        if (signContent.size() == 0)
        {
            // 读取签名文件错误
            KLOG_WARNING() << "Read sign file " << signFile << "faild.";
            r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                   QObject::tr("Invalid Argument").toStdString());
            emit Common::getInstance().transferImageFinished(name, version);
            sigFile.close();
            imgFile.close();
            return r;
        }
        sigFile.close();
        req.mutable_sign()->set_chunk_data(signContent.data(), signContent.size());
    }

    auto stream = image::Image::NewStub(chan)->Upload(&context, &r.second);
    if (!stream->Write(req))
    {
        KLOG_WARNING() << "Send image param err";
        r.first = grpc::Status(grpc::StatusCode::INTERNAL,
                               QObject::tr("Internal Error").toStdString());
        emit Common::getInstance().transferImageFinished(name, version);
        goto finish;
    }
    if (req.has_sign())
    {
        // 第一次Write后就会将签名文件写入至Stream，这里释放是为了防止后面将签名文件重复写入，节约时间
        req.release_sign();
    }

    while ((n = imgFile.read(buf, CHUNK_SIZE)) > 0)
    {
        if (Common::getInstance().isTransferStoped(name, version))
        {
            KLOG_DEBUG() << "Transmission interruption";
            context.TryCancel();
            emit Common::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED, name, version, progress);
            break;
        }
        else
        {
            trans += n;
            req.mutable_chunk_data();
            req.set_chunk_data(buf, n);
            if (!stream->Write(req))
            {
                KLOG_DEBUG() << "Broken stream, bytes transmitted:" << trans;
                emit Common::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED, name, version, progress);
                break;
            }
            auto prog = int(floor(trans * 100.0 / imgFileSize));
            if (progress != prog)
            {
                progress = prog;
                emit Common::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING, name, version, progress);
                if (prog == 100)
                    emit Common::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_SUCCESSFUL, name, version, progress);
            }
        }
    }

finish:
    imgFile.close();
    stream->WritesDone();
    r.first = stream->Finish();
    if (r.first.error_code() == grpc::StatusCode::CANCELLED)
        r.first = grpc::Status(r.first.error_code(), tr("The transmission was cancelled.").toStdString());

    emit Common::getInstance().transferImageFinished(name, version);
    return r;
}

QPair<grpc::Status, image::UpdateReply> Image::_updateImage(image::UpdateRequest &req, const QString &imageFile, const QString &signFile)
{
    const auto name = QString::fromStdString(req.info().name());
    const auto version = QString::fromStdString(req.info().version());
    QPair<grpc::Status, image::UpdateReply> r;

    // 检测rpc连接
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());
    if (!chan)
    {
        KLOG_WARNING() << "Update image failed to get connection";
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        emit Common::getInstance().transferImageFinished(name, version);
        return r;
    }

    // 检测用户认证
    grpc::ClientContext context;
    if (Common::getInstance().getAuthKey().size() > 0)
        context.AddMetadata("authorization", Common::getInstance().getAuthKey());

    // 检测镜像文件
    auto stream = image::Image::NewStub(chan)->Update(&context, &r.second);
    if (imageFile.isEmpty() && signFile.isEmpty())  // 只修改描述信息
    {
        if (!stream->Write(req))
        {
            KLOG_WARNING() << "Send image param err";
            r.first = grpc::Status(grpc::StatusCode::INTERNAL,
                                   QObject::tr("Internal Error").toStdString());
            emit Common::getInstance().transferImageFinished(name, version);
            return r;
        }
    }
    else
    {
        QFile imgFile(imageFile);
        if (!imgFile.open(QIODevice::ReadOnly))
        {
            KLOG_WARNING() << "Failed to open " << imageFile;
            r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                   QObject::tr("Invalid Argument").toStdString());
            emit Common::getInstance().transferImageFinished(name, version);
            return r;
        }

        if (!signFile.isEmpty())
        {
            QFile sigFile(signFile);
            if (!sigFile.open(QIODevice::ReadOnly) || sigFile.size() > 8192 || sigFile.size() == 0)
            {
                KLOG_WARNING() << "Failed to open " << signFile;
                r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                       QObject::tr("Invalid Argument").toStdString());
                emit Common::getInstance().transferImageFinished(name, version);
                imgFile.close();
                return r;
            }

            auto signContent = sigFile.readAll();
            if (signContent.size() == 0)
            {
                // 读取签名文件错误
                KLOG_WARNING() << "Read sign file " << signFile << "faild.";
                r.first = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                       QObject::tr("Invalid Argument").toStdString());
                emit Common::getInstance().transferImageFinished(name, version);
                imgFile.close();
                sigFile.close();
                return r;
            }

            req.mutable_sign()->set_chunk_data(signContent.data(), signContent.size());
            sigFile.close();
        }

        const auto imgFileSize = imgFile.size();
        int64_t trans(0), progress(0);
        size_t n;
        char buf[CHUNK_SIZE];

        if (!stream->Write(req))
        {
            KLOG_WARNING() << "Send image param err";
            r.first = grpc::Status(grpc::StatusCode::INTERNAL,
                                   QObject::tr("Internal Error").toStdString());
            emit Common::getInstance().transferImageFinished(name, version);
            imgFile.close();
            goto finish;
        }
        if (req.has_sign())
        {
            // 第一次Write后就会将签名文件写入至Stream，这里释放是为了防止后面将签名文件重复写入，节约时间
            req.release_sign();
        }

        while ((n = imgFile.read(buf, CHUNK_SIZE)) > 0)
        {
            if (Common::getInstance().isTransferStoped(name, version))
            {
                KLOG_DEBUG() << "Transmission interruption";
                context.TryCancel();
                emit Common::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED, name, version, progress);
                break;
            }
            else
            {
                trans += n;
                req.mutable_chunk_data();
                req.set_chunk_data(buf, n);
                if (!stream->Write(req))
                {
                    KLOG_DEBUG() << "Broken stream, bytes transmitted:" << trans;
                    emit Common::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED, name, version, progress);
                    break;
                }
                auto prog = int(floor(trans * 100.0 / imgFileSize));
                if (progress != prog)
                {
                    progress = prog;
                    emit Common::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING, name, version, progress);
                    if (prog == 100)
                        emit Common::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_UPLOADING_SUCCESSFUL, name, version, progress);
                }
            }
        }
        imgFile.close();
    }
finish:
    stream->WritesDone();
    r.first = stream->Finish();
    if (r.first.error_code() == grpc::StatusCode::CANCELLED)
        r.first = grpc::Status(r.first.error_code(), tr("The transmission was cancelled.").toStdString());

    emit Common::getInstance().transferImageFinished(name, version);
    return r;
}

QPair<grpc::Status, downloadImageInfo> Image::_downloadImage(image::DownloadRequest &req, const int64_t &image_id, const QString &name, const QString &version, const QString &savePath)
{
    QPair<grpc::Status, downloadImageInfo> r;
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());
    if (!chan)
    {
        KLOG_WARNING() << "Download image failed to get connection";
        r.first = grpc::Status(grpc::StatusCode::UNKNOWN,
                               QObject::tr("Network Error").toStdString());
        emit Common::getInstance().transferImageFinished(name, version);
        return r;
    }

    image::DownloadReply reply;
    grpc::ClientContext context;
    if (Common::getInstance().getAuthKey().size() > 0)
        context.AddMetadata("authorization", Common::getInstance().getAuthKey());

    req.set_image_id(image_id);

    auto stream = image::Image::NewStub(chan)->Download(&context, req);
    bool ret = stream->Read(&reply);
    if (!ret)
    {
        KLOG_WARNING() << "Recv image param err";
        r.first = stream->Finish();
        emit Common::getInstance().transferImageFinished(name, version);
        return r;
    }

    const auto type = reply.info().type();
    const auto checksum = reply.info().checksum();
    const auto size = reply.info().size();

    QString filePath = QString("%1%2_%3%4").arg(savePath).arg(name).arg(version).arg(type.data());
    QFile imgFile(filePath);
    if (!imgFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        KLOG_WARNING() << "Failed to open " << filePath;
        r.first = grpc::Status(grpc::StatusCode::INTERNAL,
                               QObject::tr("Failed to open %1").arg(filePath).toStdString());
        emit Common::getInstance().transferImageFinished(name, version);
        return r;
    }

    int64_t trans(0), progress(0);
    while (stream->Read(&reply))
    {
        if (Common::getInstance().isTransferStoped(name, version))
        {
            imgFile.close();
            r.first = grpc::Status(grpc::StatusCode::ABORTED, QObject::tr("The transmission was cancelled.").toStdString());
            emit Common::getInstance().transferImageFinished(name, version);
            return r;
        }

        imgFile.write(reply.chunk_data().data(), qint64(reply.chunk_data().size()));
        trans += reply.chunk_data().size();
        int tmp = int(floor(trans * 100.0 / size));
        if (progress != tmp)
        {
            progress = tmp;
            emit Common::getInstance().transferImageStatus(IMAGE_TRANSMISSION_STATUS_DOWNLOADING, name, version, progress);
        }
    }
    imgFile.close();

    QString message;
    ImageTransmissionStatus status;
    grpc::StatusCode statusCode;
    // 检测数据库中文件是否损坏
    do
    {
        QFile file(filePath);
        // QByteArray fileArray;
        if (!file.open(QIODevice::ReadOnly))
        {
            KLOG_WARNING() << "Failed to open " << filePath;
            message = tr("Failed to open %1!").arg(filePath);
            statusCode = grpc::StatusCode::INTERNAL;
            status = IMAGE_TRANSMISSION_STATUS_DOWNLOADING_FAILED;
            progress = 99;
            break;
        }
        auto fileSize = file.size();

        QCryptographicHash hash(QCryptographicHash::Sha256);
        if (!hash.addData(&file))
        {
            KLOG_WARNING() << "Failed to read file " << filePath;
            message = tr("file was broken!");
            statusCode = grpc::StatusCode::INTERNAL;
            status = IMAGE_TRANSMISSION_STATUS_DOWNLOADING_FAILED;
            progress = 99;
            file.close();
            break;
        }
        file.close();

        auto strSha256 = hash.result().toHex();
        KLOG_DEBUG() << "strSha256:" << strSha256 << ", fileSize:" << fileSize;

        if (size != fileSize || checksum != strSha256.toStdString())
        {
            KLOG_DEBUG() << size << fileSize << checksum.data() << strSha256.toStdString().data();
            message = tr("file was broken!");
            statusCode = grpc::StatusCode::INTERNAL;
            status = IMAGE_TRANSMISSION_STATUS_DOWNLOADING_FAILED;
            progress = 99;
            break;
        }

        message = tr("Ok");
        statusCode = grpc::StatusCode::OK;
        status = IMAGE_TRANSMISSION_STATUS_DOWNLOADING_SUCCESSFUL;
        progress = 100;
    } while (0);

    r.first = grpc::Status(statusCode, message.toStdString());
    r.second = downloadImageInfo{name.toStdString(), version.toStdString(), type, checksum, filePath.toStdString(), size};

    emit Common::getInstance().transferImageStatus(status, name, version, progress);
    emit Common::getInstance().transferImageFinished(name, version);
    return r;
}

QPair<grpc::Status, image::ApproveReply> Image::_checkImage(const image::ApproveRequest &req)
{
    RPC_IMPL(image::ApproveReply, image::Image::NewStub, Approve);
}

QPair<grpc::Status, image::RemoveReply> Image::_removeImage(const image::RemoveRequest &req)
{
    RPC_IMPL(image::RemoveReply, image::Image::NewStub, Remove);
}