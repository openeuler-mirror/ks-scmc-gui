#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QWidget>
#include "common/common-page.h"
#include "common/info-worker.h"

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
    void OperateImage(int page);

private slots:
    void onBtnImport();
    void onBtnExport();
    void onBtnEdit();
    void onBtnRemove();
    void onBtnCheck();
    void uploadSaveSlot(QMap<QString, QString> Info);
    void updateSaveSlot(QMap<QString, QString> Info);
    void downloadSaveSlot(QMap<QString, QString> Info);
    void checkSaveSlot(QMap<QString, QString> Info);
    void getListDBResult(const QPair<grpc::Status, image::ListDBReply> &);
    void getCheckResult(const QPair<grpc::Status, image::CheckReply> &);
    void getRemoveResult(const QPair<grpc::Status, image::RemoveReply> &);

private:
    ImageOperate *m_pImageOp;
    QMap<QString, qint64> m_IdNameMap;
};

#endif  // IMAGEMANAGER_H
