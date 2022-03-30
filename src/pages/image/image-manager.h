#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QWidget>
#include "common/common-page.h"
#include "common/info-worker.h"
#include "image-operate.h"

class ImageOperate;
class ImageManager : public CommonPage
{
    Q_OBJECT
public:
    explicit ImageManager(QWidget *parent = nullptr);
    ~ImageManager();
    void updateInfo(QString keyword = "");  //刷新表格

private:
    void initTable();
    void initButtons();
    void initImageConnect();
    int getImageFileInfo(const QString fileName, QString &strSha256, qint64 &fileSize);
    void getImageList();
    void OperateImage(ImageOperateType type);

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

private:
    ImageOperate *m_pImageOp;
    QMap<QString, QVariant> m_imageInfoMap;
};

#endif  // IMAGEMANAGER_H
