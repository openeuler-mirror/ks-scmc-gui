/**
 * @file          /ks-scmc-gui/src/pages/image/image-operate-dialog.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "image-operate-dialog.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QFileDialog>
#include <QFileInfo>
#include <QRegExpValidator>
#include <QToolTip>
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
        ui->textDesc->setText(imageInfoMap.value(IMAGE_DESC).toString());
        m_desc = imageInfoMap.value(IMAGE_DESC).toString();
    }
}

void ImageOperateDialog::initUI()
{
    setIcon(QIcon(":/images/logo.png"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    setTitle(tr("Image Operate"));
    setFixedSize(700, 600);
    setResizeable(false);
    setButtonHints(TitlebarCloseButtonHint | TitlebarMinimizeButtonHint);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btnSave, Kiran::BUTTON_Default);

    ui->btn_tip_name->setIcon(QIcon(":/images/tips.svg"));
    ui->btn_tip_name->setToolTip(tr("Only lowercase, digit or ._- three special characters;\nThe first and last characters cannot be special characters"));
    ui->btn_tip_version->setIcon(QIcon(":/images/tips.svg"));
    ui->btn_tip_version->setToolTip(tr("Only letter, digit or ._- three special characters;\nThe first characters must be letter or digit"));

    ui->lineEditName->setMaxLength(50);
    ui->lineEditName->setPlaceholderText(tr("Please input 1 to 50 characters"));
    ui->lineEditName->setTextMargins(10, 0, 10, 0);
    ui->lineEditVersion->setMaxLength(20);
    ui->lineEditVersion->setPlaceholderText(tr("Please input 1 to 20 characters"));
    ui->lineEditVersion->setTextMargins(10, 0, 10, 0);
    ui->textDesc->setPlaceholderText(tr("Please input 0 to 200 characters"));

    QPushButton *imageFileBtn = new QPushButton(this);
    imageFileBtn->setFocusPolicy(Qt::NoFocus);
    connect(imageFileBtn, &QPushButton::clicked, this, &ImageOperateDialog::selectImage);
    initLineEdit(ui->lineEditImageFile, imageFileBtn);

    QPushButton *imageSignBtn = new QPushButton(this);
    imageSignBtn->setFocusPolicy(Qt::NoFocus);
    connect(imageSignBtn, &QPushButton::clicked, this, &ImageOperateDialog::selectSign);
    initLineEdit(ui->lineEditImageSign, imageSignBtn);

    connect(ui->textDesc, &QTextEdit::textChanged, this, &ImageOperateDialog::limitLength);
}

void ImageOperateDialog::initLineEdit(QLineEdit *lineEdit, QPushButton *addBtn)
{
    QHBoxLayout *hLayout = new QHBoxLayout(lineEdit);
    hLayout->setContentsMargins(10, 0, 10, 0);

    addBtn->setText(tr("Add"));
    addBtn->setFixedSize(56, 30);
    addBtn->setCursor(Qt::PointingHandCursor);
    lineEdit->setTextMargins(10, 0, addBtn->width() + 10, 0);
    hLayout->addStretch();
    hLayout->addWidget(addBtn);
}

void ImageOperateDialog::UploadParamDeal()
{
    QString name = ui->lineEditName->text();
    QString version = ui->lineEditVersion->text();
    QString desc = ui->textDesc->toPlainText();
    QString imageFile = ui->lineEditImageFile->text();
    QString signFile = ui->lineEditImageSign->text();

    if (name.isEmpty() || version.isEmpty() || imageFile.isEmpty() || signFile.isEmpty())
    {
        KLOG_INFO() << name << version << imageFile << signFile;
        MessageDialog::message(tr("Upload Image"),
                               tr("Upload Image failed!"),
                               tr("Please improve the content!"),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    if (!checkFileOpenable(imageFile))
    {
        ui->label_tip_image->setText(tr("Can't open image file!"));
        return;
    }
    if (!checkFileOpenable(signFile))
    {
        ui->label_tip_sign->setText(tr("Can't open signature file!"));
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
    QString desc = ui->textDesc->toPlainText();
    QString imageFile = ui->lineEditImageFile->text();
    QString imageId = m_imageId;
    QString signFile = ui->lineEditImageSign->text();

    KLOG_INFO() << name << version << imageId << desc << imageFile;

    bool checkVal = false;

    //逻辑没有问题，用户如果想把desc变空，只能将镜像和签名重新传一次
    if ((!imageFile.isEmpty() && !signFile.isEmpty()) ||
        (imageFile.isEmpty() && signFile.isEmpty() && !desc.isEmpty()))
    {
        if (!imageFile.isEmpty() && !signFile.isEmpty())
        {
            QFile fileImage(imageFile);
            QFile fileSign(signFile);
            do
            {
                if (!fileImage.open(QIODevice::ReadOnly))
                {
                    ui->label_tip_image->setText(tr("Can't open image file!"));
                    checkVal = false;
                    break;
                }
                else if (!fileSign.open(QIODevice::ReadOnly))
                {
                    ui->label_tip_sign->setText(tr("Can't open signature file!"));
                    checkVal = false;
                    break;
                }
                checkVal = true;

            } while (0);

            fileImage.close();
            fileSign.close();
            if (!checkVal)
                return;
        }

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
        //修改判断顺序，当镜像/签名有一个存在时，判断另一个
        if (imageFile.isEmpty() && !signFile.isEmpty())
            ui->label_tip_image->setText(tr("Please input image file!"));
        else if (signFile.isEmpty() && !imageFile.isEmpty())
            ui->label_tip_sign->setText(tr("Please input signature file!"));
        else if (desc.isEmpty())
            ui->label_tip_desc->setText(tr("Please input the description of image!"));
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

bool ImageOperateDialog::checkFileOpenable(QString fileName)
{
    bool ret = false;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        ret = true;
    }

    file.close();
    return ret;
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
    ui->label_tip_image->clear();
    ui->label_tip_sign->clear();
    ui->label_tip_desc->clear();
    ui->label_tip_name->clear();
    ui->label_tip_version->clear();

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

void ImageOperateDialog::limitLength()
{
    QString textContent = ui->textDesc->toPlainText();
    int length = textContent.count();
    int maxLength = 200;  // 最大字符数
    if (length > maxLength)
    {
        int position = ui->textDesc->textCursor().position();
        QTextCursor textCursor = ui->textDesc->textCursor();
        textContent.remove(position - (length - maxLength), length - maxLength);
        ui->textDesc->setText(textContent);
        textCursor.setPosition(position - (length - maxLength));
        ui->textDesc->setTextCursor(textCursor);
    }
}
