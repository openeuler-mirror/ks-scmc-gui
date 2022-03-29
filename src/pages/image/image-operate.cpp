#include "image-operate.h"
#include <kiran-log/qt5-log-i.h>
#include <QFileDialog>
#include <QFileInfo>
#include "common/message-dialog.h"
#include "ui_image-operate.h"

ImageOperate::ImageOperate(ImageOperateType type, QWidget *parent) : KiranTitlebarWindow(parent),
                                                                     ui(new Ui::ImageOperate),
                                                                     m_type(type)
{
    KLOG_INFO() << "ImageOperate";
    ui->setupUi(getWindowContentWidget());
    initUI();

    connect(ui->btnSave, &QPushButton::clicked, this, &ImageOperate::onSave);
    connect(ui->btnCancel, &QPushButton::clicked, this, &ImageOperate::onCancel);
}

ImageOperate::~ImageOperate()
{
    KLOG_INFO() << "**********Deconstruction ImageOperate";
    delete ui;
}

void ImageOperate::setImageInfo(QMap<QString, QVariant> imageInfoMap)
{
    m_imageInfoMap = imageInfoMap;
    if (m_type == IMAGE_OPERATE_TYPE_UPDATE)
    {
        //设置界面初始值
        ui->lineEditName->setText(m_imageInfoMap.value(IMAGE_NAME).toString());
        ui->lineEditName->setDisabled(true);
        ui->lineEditVersion->setText(m_imageInfoMap.value(IMAGE_VERSION).toString());
        ui->lineEditVersion->setDisabled(true);
        ui->lineEditImageFile->setText(m_imageInfoMap.value(IMAGE_FILE).toString());
        ui->lineEditImageSign->setText(m_imageInfoMap.value(IMAGE_SIGN).toString());
        ui->lineEditDesc->setText(m_imageInfoMap.value(IMAGE_DESC).toString());
    }
}

void ImageOperate::initUI()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Image Operate"));
    setButtonHints(TitlebarCloseButtonHint | TitlebarMinimizeButtonHint);

    QPushButton *imageFileBtn = new QPushButton(this);
    connect(imageFileBtn, &QPushButton::clicked, this, &ImageOperate::selectImage);
    initLineEdit(ui->lineEditImageFile, imageFileBtn);

    QPushButton *imageSignBtn = new QPushButton(this);
    connect(imageSignBtn, &QPushButton::clicked, this, &ImageOperate::selectSign);
    initLineEdit(ui->lineEditImageSign, imageSignBtn);
}

void ImageOperate::initLineEdit(QLineEdit *lineEdit, QPushButton *addBtn)
{
    QHBoxLayout *hLayout = new QHBoxLayout(lineEdit);
    hLayout->setContentsMargins(10, 0, 0, 0);

    addBtn->setText(tr("Add"));
    addBtn->setFixedSize(56, 30);
    addBtn->setCursor(Qt::PointingHandCursor);
    lineEdit->setTextMargins(0, 0, addBtn->width(), 0);
    hLayout->addStretch();
    hLayout->addWidget(addBtn);
}

void ImageOperate::UploadParamDeal()
{
    QString name = ui->lineEditName->text();
    QString version = ui->lineEditVersion->text();
    QString desc = ui->lineEditDesc->text();
    QString imageFile = ui->lineEditImageFile->text();

    if (name.isEmpty() || version.isEmpty() || desc.isEmpty() || imageFile.isEmpty())
    {
        KLOG_INFO() << name << version << desc << imageFile;
        MessageDialog::message(tr("Upload Image"),
                               tr("Upload Image failed!"),
                               tr("Please improve the content!"),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    QMap<QString, QString> uploadInfo;
    uploadInfo.insert("Image Name", name);
    uploadInfo.insert("Image Version", version);
    uploadInfo.insert("Image Description", desc);
    uploadInfo.insert("Image File", imageFile);
    emit sigUploadSave(uploadInfo);
}

void ImageOperate::updateParamDeal()
{
    QString name = ui->lineEditName->text();
    QString version = ui->lineEditVersion->text();
    QString desc = ui->lineEditDesc->text();
    QString imageFile = ui->lineEditImageFile->text();
    QString imageId = m_imageInfoMap.value(IMAGE_ID).toString();

    KLOG_INFO() << name << version << imageId << ui->lineEditDesc->text() << imageFile;

    if (name.isEmpty() || version.isEmpty() || desc.isEmpty() || imageFile.isEmpty())
    {
        MessageDialog::message(tr("Update Image"),
                               tr("Update Image failed!"),
                               tr("Please improve the content!"),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    QMap<QString, QString> updateInfo;
    updateInfo.insert("Image Id", imageId);
    updateInfo.insert("Image Name", name);
    updateInfo.insert("Image Version", version);
    updateInfo.insert("Image Description", desc);
    updateInfo.insert("Image File", imageFile);
    emit sigUpdateSave(updateInfo);
}

void ImageOperate::downloadParamDeal()
{
    QString imageId = m_imageInfoMap.value(IMAGE_ID).toString();
    QString imagePath = ui->downloadPath->text();
    if (imagePath.at(imagePath.size() - 1) != "/")
        imagePath += "/";

    KLOG_INFO() << imageId << ui->downloadImage->currentText() << imagePath;

    if (imagePath.isEmpty())
    {
        MessageDialog::message(tr("Download Image"),
                               tr("Download Image failed!"),
                               tr("Please improve the content!"),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    QMap<QString, QString> downloadInfo;
    downloadInfo.insert("Image Id", imageId);
    downloadInfo.insert("Image Path", imagePath);
    emit sigDownloadSave(downloadInfo);
}

//void ImageOperate::checkParamDeal()
//{
//    QString imageId = QString::number(m_IdNameMap[ui->checkImage->currentText()]);
//    QString status = ui->checkStatus->currentText();
//    QString reason = ui->checkReason->text();

//    KLOG_INFO() << status << reason << imageId << ui->checkImage->currentText();

//    if (status == "Reject" && reason.isEmpty())
//    {
//        MessageDialog::message(tr("Check Image"),
//                               tr("Check Image failed!"),
//                               tr("Please improve the content!"),
//                               ":/images/warning.svg",
//                               MessageDialog::StandardButton::Ok);
//        return;
//    }

//    QMap<QString, QString> checkInfo;
//    checkInfo.insert("Image Id", imageId);
//    checkInfo.insert("Image Check", status);
//    checkInfo.insert("Image Reason", reason);
//    emit sigCheckSave(checkInfo);
//}

QString ImageOperate::ChooseFile(QString nameFilter)
{
    QString file;
    QFileDialog *pFile = new QFileDialog(this);
    pFile->setWindowTitle(tr("Please select an image file"));
    pFile->setDirectory(".");
    pFile->setNameFilter(nameFilter);
    pFile->setViewMode(QFileDialog::Detail);
    if (pFile->exec())
    {
        KLOG_INFO() << "fileNames:" << pFile->selectedFiles()[0];
        file = pFile->selectedFiles()[0];
        delete pFile;
    }

    return file;
}

void ImageOperate::selectImage()
{
    ui->lineEditImageFile->setText(ChooseFile("*.tar *.zip"));
}

void ImageOperate::selectSign()
{
    ui->lineEditImageSign->setText(ChooseFile("*.sign"));
}

void ImageOperate::downloadSelectPath()
{
    QFileDialog *pFile = new QFileDialog(this);
    QString file_path = pFile->getExistingDirectory(this, tr("Please select the path to save"), "./");
    if (file_path.isEmpty())
    {
        return;
    }
    KLOG_INFO() << "file_path:" << file_path;
    ui->downloadPath->setText(file_path);
}

void ImageOperate::onSave()
{
    switch (m_type)
    {
    case IMAGE_OPERATE_TYPE_UPLOAD:
        UploadParamDeal();
        break;
    case IMAGE_OPERATE_TYPE_UPDATE:
        updateParamDeal();
        break;
    case IMAGE_OPERATE_TYPE_DOWNLOAD:
        downloadParamDeal();
        break;
    default:
        break;
    }

    this->close();
}

void ImageOperate::onCancel()
{
    this->close();
}
