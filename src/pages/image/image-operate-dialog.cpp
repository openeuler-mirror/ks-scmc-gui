#include "image-operate-dialog.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QFileDialog>
#include <QFileInfo>
#include "common/message-dialog.h"
#include "ui_image-operate-dialog.h"
ImageOperateDialog::ImageOperateDialog(ImageOperateType type, QWidget *parent) : KiranTitlebarWindow(parent),
                                                                                 ui(new Ui::ImageOperateDialog),
                                                                                 m_type(type)
{
    KLOG_INFO() << "ImageOperateDialog";
    ui->setupUi(getWindowContentWidget());
    initUI();

    connect(ui->btnSave, &QPushButton::clicked, this, &ImageOperateDialog::onSave);
    connect(ui->btnCancel, &QPushButton::clicked, this, &ImageOperateDialog::onCancel);
}

ImageOperateDialog::~ImageOperateDialog()
{
    KLOG_INFO() << "**********Deconstruction ImageOperateDialog";
    delete ui;
}

void ImageOperateDialog::setImageInfo(QMap<QString, QVariant> imageInfoMap)
{
    m_imageId = imageInfoMap.value(IMAGE_ID).toString();
    if (m_type == IMAGE_OPERATE_TYPE_UPDATE)
    {
        //设置界面初始值
        ui->lineEditName->setText(imageInfoMap.value(IMAGE_NAME).toString());
        ui->lineEditName->setDisabled(true);
        ui->lineEditVersion->setText(imageInfoMap.value(IMAGE_VERSION).toString());
        ui->lineEditVersion->setDisabled(true);
        ui->lineEditImageFile->setText(imageInfoMap.value(IMAGE_FILE).toString());
        ui->lineEditImageSign->setText(imageInfoMap.value(IMAGE_SIGN).toString());
        ui->lineEditDesc->setText(imageInfoMap.value(IMAGE_DESC).toString());
        m_desc = imageInfoMap.value(IMAGE_DESC).toString();
    }
}

void ImageOperateDialog::initUI()
{
    setIcon(QIcon(":/images/logo.png"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    setTitle(tr("Image Operate"));
    setFixedSize(600, 500);
    setButtonHints(TitlebarCloseButtonHint | TitlebarMinimizeButtonHint);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btnSave, Kiran::BUTTON_Default);

    QPushButton *imageFileBtn = new QPushButton(this);
    connect(imageFileBtn, &QPushButton::clicked, this, &ImageOperateDialog::selectImage);
    initLineEdit(ui->lineEditImageFile, imageFileBtn);

    QPushButton *imageSignBtn = new QPushButton(this);
    connect(imageSignBtn, &QPushButton::clicked, this, &ImageOperateDialog::selectSign);
    initLineEdit(ui->lineEditImageSign, imageSignBtn);
}

void ImageOperateDialog::initLineEdit(QLineEdit *lineEdit, QPushButton *addBtn)
{
    QHBoxLayout *hLayout = new QHBoxLayout(lineEdit);
    hLayout->setContentsMargins(10, 0, 10, 0);

    addBtn->setText(tr("Add"));
    addBtn->setFixedSize(56, 30);
    addBtn->setCursor(Qt::PointingHandCursor);
    lineEdit->setTextMargins(0, 0, addBtn->width(), 0);
    hLayout->addStretch();
    hLayout->addWidget(addBtn);
}

void ImageOperateDialog::UploadParamDeal()
{
    QString name = ui->lineEditName->text();
    QString version = ui->lineEditVersion->text();
    QString desc = ui->lineEditDesc->text();
    QString imageFile = ui->lineEditImageFile->text();
    QString signFile = ui->lineEditImageSign->text();

    if (name.isEmpty() || version.isEmpty() || desc.isEmpty() || imageFile.isEmpty() || signFile.isEmpty())
    {
        KLOG_INFO() << name << version << desc << imageFile << signFile;
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
    uploadInfo.insert("Sign File", signFile);
    emit sigUploadSave(uploadInfo);
    close();
}

void ImageOperateDialog::updateParamDeal()
{
    QString name = ui->lineEditName->text();
    QString version = ui->lineEditVersion->text();
    QString desc = ui->lineEditDesc->text().isEmpty() ? m_desc : ui->lineEditDesc->text();
    QString imageFile = ui->lineEditImageFile->text();
    QString imageId = m_imageId;
    QString signFile = ui->lineEditImageSign->text();

    KLOG_INFO() << name << version << imageId << ui->lineEditDesc->text() << imageFile;

    if ((!imageFile.isEmpty() && !signFile.isEmpty()) ||
        (imageFile.isEmpty() && signFile.isEmpty() && !desc.isEmpty()))
    {
        QMap<QString, QString> updateInfo;
        updateInfo.insert("Image Id", imageId);
        updateInfo.insert("Image Name", name);
        updateInfo.insert("Image Version", version);
        updateInfo.insert("Image Description", desc);
        updateInfo.insert("Image File", imageFile);
        updateInfo.insert("Sign File", signFile);
        emit sigUpdateSave(updateInfo);
        close();
    }
    else
    {
        MessageDialog::message(tr("Update Image"),
                               tr("Update Image failed!"),
                               tr("Please improve the content!"),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }
}

//void ImageOperateDialog::checkParamDeal()
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

QString ImageOperateDialog::ChooseFile(QString nameFilter)
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

void ImageOperateDialog::selectImage()
{
    ui->lineEditImageFile->setText(ChooseFile("*.tar *.zip"));
}

void ImageOperateDialog::selectSign()
{
    ui->lineEditImageSign->setText(ChooseFile("*.sig"));
}

void ImageOperateDialog::onSave()
{
    switch (m_type)
    {
    case IMAGE_OPERATE_TYPE_UPLOAD:
        UploadParamDeal();
        break;
    case IMAGE_OPERATE_TYPE_UPDATE:
        updateParamDeal();
        break;
    default:
        break;
    }
}

void ImageOperateDialog::onCancel()
{
    this->close();
}
