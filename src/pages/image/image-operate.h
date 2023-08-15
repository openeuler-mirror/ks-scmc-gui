#ifndef IMAGEOPERATE_H
#define IMAGEOPERATE_H

#include <kiran-titlebar-window.h>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QWidget>

#define IMAGE_ID "image id"
#define IMAGE_NAME "image name"
#define IMAGE_VERSION "image version"
#define IMAGE_DESC "image description"
#define IMAGE_FILE "image file"
#define IMAGE_SIGN "image sign"

namespace Ui
{
class ImageOperate;
}

enum ImageOperateType
{
    IMAGE_OPERATE_TYPE_UPLOAD,
    IMAGE_OPERATE_TYPE_UPDATE,
    IMAGE_OPERATE_TYPE_DOWNLOAD
};

class ImageOperate : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit ImageOperate(ImageOperateType type, QWidget *parent = nullptr);
    ~ImageOperate();
    void setImageInfo(QMap<QString, QVariant> imageInfoMap);

private:
    void initUI();
    void initLineEdit(QLineEdit *lineEdit, QPushButton *addBtn);
    void UploadParamDeal();
    void updateParamDeal();
    void downloadParamDeal();
    QString ChooseFile(QString nameFilter);

signals:
    void sigUploadSave(QMap<QString, QString>);
    void sigUpdateSave(QMap<QString, QString>);
    void sigDownloadSave(QMap<QString, QString>);

private slots:
    void selectImage();
    void selectSign();
    void downloadSelectPath();
    void onSave();
    void onCancel();

private:
    Ui::ImageOperate *ui;
    QMap<QString, QVariant> m_imageInfoMap;
    ImageOperateType m_type;
};

#endif  // IMAGEOPERATE_H
