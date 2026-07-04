#ifndef IMAGEMANAGERPAGE_H
#define IMAGEMANAGERPAGE_H

#include <QMutex>
#include <QWidget>
#include "base/info-worker.h"
#include "common-page.h"
#include "image-operate-dialog.h"

class ImageOperate;
class ImageManagerPage : public CommonPage
{
    Q_OBJECT
public:
    explicit ImageManagerPage(QWidget *parent = nullptr);
    ~ImageManagerPage();
    void updateInfo(QString keyword = "");  //刷新表格

private:
    void initTable();
    void initButtons();
    void initImageConnect();
    int getImageFileInfo(const QString fileName, QString &strSha256, qint64 &fileSize);
    void getImageList();
    void OperateImage(ImageOperateType type);
    bool imageIsTransfering(QString imageName, QString version, QString title);

private slots:
    void onBtnUpload();
    void onBtnDownload();
    void onBtnUpdate();
    void onBtnRemove();
    void uploadSaveSlot(QMap<QString, QString> Info);
    void updateSaveSlot(QMap<QString, QString> Info);
    void downloadSaveSlot(QMap<QString, QString> Info);
    void checkSaveSlot(QMap<QString, QString> Info);
    void getListDBResult(const QPair<grpc::Status, image::ListDBReply> &);
    void getCheckResult(const QPair<grpc::Status, image::ApproveReply> &);
    void getRemoveResult(const QPair<grpc::Status, image::RemoveReply> &);
    void getUploadResult(const QPair<grpc::Status, image::UploadReply> &reply);
    void getUpdateResult(const QPair<grpc::Status, image::UpdateReply> &reply);
    void getDownloadImageResult(const QPair<grpc::Status, downloadImageInfo> &reply);
    void getTransferImageFinishedResult(QString name, QString version);

signals:
    //void sigTransferImageInfo(ImageTransmissionStatus status, std::string name, std::string version, int rate);

private:
    ImageOperateDialog *m_pImageOp;
    QStringList m_transferImages = {};
    QMutex m_mutex;
};

#endif  // IMAGEMANAGERPAGE_H
