#ifndef IMAGEOPERATE_H
#define IMAGEOPERATE_H

#include <QWidget>
#include <QMap>

namespace Ui {
class ImageOperate;
}

enum ImageOperatePage
{
    UPLOAD_PAGE,
    UPDATE_PAGE,
    DOWNLOAD_PAGE,
    CHECK_PAGE
};

class ImageOperate : public QWidget
{
    Q_OBJECT

public:
    explicit ImageOperate(int page, QMap<QString, qint64> imageInfo, QWidget *parent = nullptr);
    ~ImageOperate();

private:
    void initCombox(int page, QMap<QString, qint64> imageInfo);
    void UploadParamDeal();
    void updateParamDeal();
    void downloadParamDeal();
    void checkParamDeal();
    QString ChooseFile();

signals:
    void sigUploadSave(QMap<QString, QString>);
    void sigUpdateSave(QMap<QString, QString>);
    void sigDownloadSave(QMap<QString, QString>);
    void sigCheckSave(QMap<QString, QString>);

private slots:
    void uploadSelectFile();
    void updateSelectFile();
    void downloadSelectPath();
    void onSave();
    void onCancel();

private:
    Ui::ImageOperate *ui;
    QMap<QString, qint64> m_IdNameMap;
};

#endif // IMAGEOPERATE_H
