/**
 * @file          /ks-scmc-gui/src/pages/image/image-list-page.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef IMAGELISTPAGE_H
#define IMAGELISTPAGE_H

#include <QMutex>
#include <QVector>
#include <QWidget>
#include "image-operate-dialog.h"
#include "info-worker.h"
#include "table-page.h"

class ImageOperate;
class ImageListPage : public TablePage
{
    Q_OBJECT
public:
    explicit ImageListPage(QWidget *parent = nullptr, bool flag = false);
    ~ImageListPage();
    void updateInfo(QString keyword = "");                               //刷新表格
    void setDelRow(const QString type1 = "", const QString type2 = "");  //"Wait for Approve","Rejected","Passed"
    void setCheckBox(bool);

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
    void onBtnPass();
    void onBtnRefuse();
    void onBtnPassLabel(int row);
    void onBtnRefuseLabel(int row);
    void uploadSaveSlot(QMap<QString, QString> Info);
    void updateSaveSlot(QMap<QString, QString> Info);
    void downloadSaveSlot(QMap<QString, QString> Info);
    void checkSaveSlot(QMap<QString, QString> Info);
    void getListDBResult(const QString objId, const QPair<grpc::Status, image::ListDBReply> &);
    void getCheckResult(const QString objId, const QPair<grpc::Status, image::ApproveReply> &);
    void getRemoveResult(const QString objId, const QPair<grpc::Status, image::RemoveReply> &);
    void getUploadResult(const QString objId, const QPair<grpc::Status, image::UploadReply> &reply);
    void getUpdateResult(const QString objId, const QPair<grpc::Status, image::UpdateReply> &reply);
    void getDownloadImageResult(const QString objId, const QPair<grpc::Status, downloadImageInfo> &reply);
    void getTransferImageFinishedResult(QString name, QString version);

signals:
    //void sigTransferImageInfo(ImageTransmissionStatus status, std::string name, std::string version, int rate);
    void sigUpdateAuditInfo();
    void sigUpdateAPproveTipSums(int tips = 0);

private:
    QString m_objId;
    ImageOperateDialog *m_pImageOp;
    QStringList m_transferImages = {};
    QMutex m_mutex;
    QVector<QString> is_del_row;
    bool is_init_audit_btn = false;
    bool is_open_checkbox = true;
};

#endif  // IMAGELISTPAGE_H
