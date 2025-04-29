/**
 * @file          src/base/interface/image.h
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

struct downloadImageInfo
{
    std::string name;
    std::string version;
    std::string type;
    std::string checksum;
    std::string imageFile;
    int64_t filesize;
};

class Image : public QObject
{
    Q_OBJECT
public:
    static Image &getInstance()
    {
        static Image s_Image;
        return s_Image;
    }
    static QString generateId(QObject *callObj);

    // image management
    void listImage(const QString objId, const int64_t);
    void listImage(const QString objId);
    void listDBImage(const QString objId);
    void uploadImage(const QString objId, image::UploadRequest &req, const QString &imageFile, const QString &signFile);
    void updateImage(const QString objId, image::UpdateRequest &req, const QString &imageFile, const QString &signFile);
    void downloadImage(const QString objId, const int64_t &image_id, const QString &name, const QString &version, const QString &savePath);
    void checkImage(const QString objId, const int64_t image_id, const bool approve, const std::string &reject_reason);
    void removeImage(const QString objId, const std::vector<int64_t> &image_ids);

private:
    Image(QObject *parent = nullptr);
    ~Image();

    // image management
    static QPair<grpc::Status, image::ListReply> _listImage(const image::ListRequest &);
    static QPair<grpc::Status, image::ListDBReply> _listDBImage(const image::ListDBRequest &);
    static QPair<grpc::Status, image::UploadReply> _uploadImage(image::UploadRequest &req, const QString &imageFile, const QString &signFile);
    static QPair<grpc::Status, image::UpdateReply> _updateImage(image::UpdateRequest &req, const QString &imageFile, const QString &signFile);
    static QPair<grpc::Status, downloadImageInfo> _downloadImage(image::DownloadRequest &req, const int64_t &image_id, const QString &name, const QString &version, const QString &savePath);
    static QPair<grpc::Status, image::ApproveReply> _checkImage(const image::ApproveRequest &);
    static QPair<grpc::Status, image::RemoveReply> _removeImage(const image::RemoveRequest &);

signals:
    // image management
    void listImageFinished(const QString objId, const QPair<grpc::Status, image::ListReply> &);
    void listDBImageFinished(const QString objId, const QPair<grpc::Status, image::ListDBReply> &);
    void uploadFinished(const QString objId, const QPair<grpc::Status, image::UploadReply> &);
    void updateFinished(const QString objId, const QPair<grpc::Status, image::UpdateReply> &);
    void downloadImageFinished(const QString objId, const QPair<grpc::Status, downloadImageInfo> &);
    void checkImageFinished(const QString objId, const QPair<grpc::Status, image::ApproveReply> &);
    void removeImageFinished(const QString objId, const QPair<grpc::Status, image::RemoveReply> &);

private:
    QMutex mutex;
};
