#include "image-operate.h"
#include <kiran-log/qt5-log-i.h>
#include <QFileDialog>
#include <QFileInfo>
#include "common/message-dialog.h"
#include "ui_image-operate.h"

ImageOperate::ImageOperate(int page, QMap<QString, qint64> imageInfo, QWidget *parent) : QWidget(parent),
                                                                                         ui(new Ui::ImageOperate),
                                                                                         m_IdNameMap(imageInfo)
{
    KLOG_INFO() << "ImageOperate";
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Image Operate"));
    ui->stackedWidget->setCurrentIndex(page);
    initCombox(page, imageInfo);
    connect(ui->uploadChooseFile, SIGNAL(clicked()), this, SLOT(uploadSelectFile()));
    connect(ui->updateChooseFile, SIGNAL(clicked()), this, SLOT(updateSelectFile()));
    connect(ui->downloadChoosePath, SIGNAL(clicked()), this, SLOT(downloadSelectPath()));
    connect(ui->btnSave, &QPushButton::clicked, this, &ImageOperate::onSave);
    connect(ui->btnCancel, &QPushButton::clicked, this, &ImageOperate::onCancel);
}

ImageOperate::~ImageOperate()
{
    KLOG_INFO() << "**********Deconstruction ImageOperate";
    delete ui;
}

void ImageOperate::initCombox(int page, QMap<QString, qint64> imageInfo)
{
    if (UPDATE_PAGE == page)
    {
        auto it = imageInfo.begin();
        while (it != imageInfo.end())
        {
            ui->updateImage->addItem(it.key());
            it++;
        }
    }
    if (DOWNLOAD_PAGE == page)
    {
        auto it = imageInfo.begin();
        while (it != imageInfo.end())
        {
            ui->downloadImage->addItem(it.key());
            it++;
        }
    }
    else if (CHECK_PAGE == page)
    {
        auto it = imageInfo.begin();
        while (it != imageInfo.end())
        {
            ui->checkImage->addItem(it.key());
            it++;
        }

        QStringList listCheck;
        listCheck << tr("Pass") << tr("Reject");
        for (auto check : listCheck)
        {
            ui->checkStatus->addItem(check);
        }
    }
}

void ImageOperate::UploadParamDeal()
{
    QString name = ui->uploadName->text();
    QString version = ui->uploadVersion->text();
    QString desc = ui->uploadDesc->text();
    QString imageFile = ui->uploadFile->text();

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
    QString name = ui->updateName->text();
    QString version = ui->updateVersion->text();
    QString desc = ui->updateDesc->text();
    QString imageFile = ui->updateFile->text();
    QString imageId = QString::number(m_IdNameMap[ui->updateImage->currentText()]);

    KLOG_INFO() << name << version << imageId << ui->updateImage->currentText() << imageFile;

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
    QString imageId = QString::number(m_IdNameMap[ui->downloadImage->currentText()]);
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

void ImageOperate::checkParamDeal()
{
    QString imageId = QString::number(m_IdNameMap[ui->checkImage->currentText()]);
    QString status = ui->checkStatus->currentText();
    QString reason = ui->checkReason->text();

    KLOG_INFO() << status << reason << imageId << ui->checkImage->currentText();

    if (status == "Reject" && reason.isEmpty())
    {
        MessageDialog::message(tr("Check Image"),
                               tr("Check Image failed!"),
                               tr("Please improve the content!"),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    QMap<QString, QString> checkInfo;
    checkInfo.insert("Image Id", imageId);
    checkInfo.insert("Image Check", status);
    checkInfo.insert("Image Reason", reason);
    emit sigCheckSave(checkInfo);
}

QString ImageOperate::ChooseFile()
{
    QString file;
    QFileDialog *pFile = new QFileDialog(this);
    pFile->setWindowTitle(tr("Please select an image file"));
    pFile->setDirectory(".");
    pFile->setNameFilter("*.tar *.zip");
    pFile->setViewMode(QFileDialog::Detail);
    if (pFile->exec())
    {
        KLOG_INFO() << "fileNames:" << pFile->selectedFiles()[0];
        file = pFile->selectedFiles()[0];
        delete pFile;
    }

    return file;
}

void ImageOperate::uploadSelectFile()
{
    ui->uploadFile->setText(ChooseFile());
}

void ImageOperate::updateSelectFile()
{
    ui->updateFile->setText(ChooseFile());
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
    int curPage = ui->stackedWidget->currentIndex();
    KLOG_INFO() << "curPage:" << curPage;
    switch (curPage)
    {
    case UPLOAD_PAGE:
        UploadParamDeal();
        break;
    case UPDATE_PAGE:
        updateParamDeal();
        break;
    case DOWNLOAD_PAGE:
        downloadParamDeal();
        break;
    case CHECK_PAGE:
        checkParamDeal();
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
