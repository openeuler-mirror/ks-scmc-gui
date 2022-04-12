#ifndef IMAGEOPERATEDIALOG_H
#define IMAGEOPERATEDIALOG_H

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
class ImageOperateDialog;
}

enum ImageOperateType
{
    IMAGE_OPERATE_TYPE_UPLOAD,
    IMAGE_OPERATE_TYPE_UPDATE,
    IMAGE_OPERATE_TYPE_DOWNLOAD
};

class ImageOperateDialog : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit ImageOperateDialog(ImageOperateType type, QWidget *parent = nullptr);
    ~ImageOperateDialog();
    void setImageInfo(QMap<QString, QVariant> imageInfoMap);

private:
    void initUI();
    void initLineEdit(QLineEdit *lineEdit, QPushButton *addBtn);
    void UploadParamDeal();
    void updateParamDeal();
    QString ChooseFile(QString nameFilter);

signals:
    void sigUploadSave(QMap<QString, QString>);
    void sigUpdateSave(QMap<QString, QString>);
    void sigDownloadSave(QMap<QString, QString>);

private slots:
    void selectImage();
    void selectSign();
    void onSave();
    void onCancel();

private:
    Ui::ImageOperateDialog *ui;
    QString m_imageId;
    ImageOperateType m_type;
};

#endif  // IMAGEOPERATEDIALOG_H
