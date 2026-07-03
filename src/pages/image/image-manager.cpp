#include "image-manager.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDesktopWidget>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>
#include "common/def.h"
#include "common/message-dialog.h"

ImageManager::ImageManager(QWidget *parent) : CommonPage(parent), m_pImageOp(nullptr)
{
    initButtons();
    initTable();
}

ImageManager::~ImageManager()
{
    KLOG_INFO() << "************Deconstruction ImageManager";
    if (m_pImageOp)
    {
        delete m_pImageOp;
        m_pImageOp = nullptr;
    }
}

void ImageManager::updateInfo(QString keyword)
{
    KLOG_INFO() << "ImageList updateInfo";
    clearText();
    InfoWorker::getInstance().disconnect();
    if (keyword.isEmpty())
    {
        initImageConnect();
        getImageList();
    }
}

void ImageManager::initTable()
{
    QList<QString> tableHHeaderDate = {
        "",
        tr("Image Name"),
        tr("Version"),
        tr("Description"),
        tr("Inspection Status"),
        tr("Approval Status"),
        tr("Last Update")};
    setHeaderSections(tableHHeaderDate);
    setHeaderCheckable(false);
    //setTableActions(tableHHeaderDate.size() - 1, QStringList() << ":/images/edit.svg");
    setTableDefaultContent("-");
    setTableSingleChoose(true);
}

void ImageManager::initButtons()
{
    QMap<int, QPushButton *> opBtnMap;
    //按钮
    const QMap<int, QString> btnNameMap = {
        {OPERATION_BUTTOM_IMAGE_MANAGER_UPLOAD, tr("Upload")},
        {OPERATION_BUTTOM_IMAGE_MANAGER_UPDATE, tr("Update")},
        {OPERATION_BUTTOM_IMAGE_MANAGER_DOWNLOAD, tr("Download")},
        {OPERATION_BUTTOM_IMAGE_MANAGER_REMOVE, tr("Remove")}};

    for (auto iter = btnNameMap.begin(); iter != btnNameMap.end(); iter++)
    {
        QString name = iter.value();
        QPushButton *btn = new QPushButton(this);
        btn->setObjectName("btn");

        if (name == tr("Remove"))
        {
            btn->setStyleSheet("#btn{background-color:#ff4b4b;"
                               "border:none;"
                               "border-radius: 4px;"
                               "color:#ffffff;}"
                               "#btn:hover{ background-color:#ff6c6c;}"
                               "#btn:focus{outline:none;}"
                               "#btn:disabled{color:#919191;background:#393939;}");
        }
        else
            btn->setStyleSheet("#btn{background-color:#2eb3ff;"
                               "border:none;"
                               "border-radius: 4px;"
                               "color:#ffffff;}"
                               "#btn:hover{ background-color:#77ceff;}"
                               "#btn:focus{outline:none;}"
                               "#btn:disabled{color:#919191;background:#393939;}");

        btn->setText(name);
        btn->setFixedSize(QSize(78, 32));
        opBtnMap.insert(iter.key(), btn);
    }
    connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_UPLOAD], &QPushButton::clicked, this, &ImageManager::onBtnUpload);
    connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_DOWNLOAD], &QPushButton::clicked, this, &ImageManager::onBtnDownload);
    connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_UPDATE], &QPushButton::clicked, this, &ImageManager::onBtnUpdate);
    connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_REMOVE], &QPushButton::clicked, this, &ImageManager::onBtnRemove);

    addSingleOperationButton(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_UPLOAD]);
    addBatchOperationButtons(QList<QPushButton *>() << opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_UPDATE]
                                                    << opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_DOWNLOAD]
                                                    << opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_REMOVE]);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void ImageManager::initImageConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::listDBImageFinished, this, &ImageManager::getListDBResult);
    //connect(&InfoWorker::getInstance(), &InfoWorker::checkImageFinished, this, &ImageManager::getCheckResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::removeImageFinished, this, &ImageManager::getRemoveResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::uploadFinished, this, &ImageManager::getUploadResult);
}

int ImageManager::getImageFileInfo(const QString fileName, QString &strSha256, qint64 &fileSize)
{
    QFile file(fileName);
    QByteArray fileArray;

    if (!file.open(QIODevice::ReadOnly))
    {
        KLOG_INFO() << "Failed to open " << fileName;
        return -1;
    }

    fileSize = file.size();
    fileArray = file.readAll();
    file.close();
    strSha256 = QCryptographicHash::hash(fileArray, QCryptographicHash::Sha256).toHex();

    KLOG_INFO() << "strSha256:" << strSha256 << ", fileSize:" << fileSize;
    return 0;
}

void ImageManager::getImageList()
{
    InfoWorker::getInstance().listDBImage();
}

void ImageManager::OperateImage(ImageOperateType type)
{
    if (!m_pImageOp)
    {
        m_pImageOp = new ImageOperate(type);
        if (type == IMAGE_OPERATE_TYPE_UPDATE)
        {
            QList<QMap<QString, QVariant>> info = getCheckedItemInfo(0);
            if (!info.isEmpty())
                m_pImageOp->setImageInfo(info.at(0));
        }

        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_pImageOp->move(screenGeometry.x() + (screenGeometry.width() - this->width()) / 2,
                         screenGeometry.y() + (screenGeometry.height() - this->height()) / 2);

        m_pImageOp->show();

        switch (type)
        {
        case IMAGE_OPERATE_TYPE_UPLOAD:
            connect(m_pImageOp, &ImageOperate::sigUploadSave, this, &ImageManager::uploadSaveSlot);
            break;
        case IMAGE_OPERATE_TYPE_UPDATE:
            connect(m_pImageOp, &ImageOperate::sigUpdateSave, this, &ImageManager::updateSaveSlot);
            break;
        case IMAGE_OPERATE_TYPE_DOWNLOAD:
            connect(m_pImageOp, &ImageOperate::sigDownloadSave, this, &ImageManager::downloadSaveSlot);
            break;
        default:
            break;
        }
        connect(m_pImageOp, &ImageOperate::destroyed,
                [=] {
                    KLOG_INFO() << "destroy  ImageOperate";
                    m_pImageOp->deleteLater();
                    m_pImageOp = nullptr;
                });
    }
}

void ImageManager::onBtnUpload()
{
    OperateImage(IMAGE_OPERATE_TYPE_UPLOAD);
}

void ImageManager::onBtnDownload()
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(0);
    if (!info.isEmpty())
    {
        QString imageId = info.at(0).value(IMAGE_ID).toString();

        QFileDialog *pFile = new QFileDialog(this);
        QString imagePath = pFile->getExistingDirectory(this, tr("Please select the path to save"), "./");
        if (imagePath.isEmpty())
        {
            return;
        }
        if (imagePath.at(imagePath.size() - 1) != "/")
            imagePath += "/";
        KLOG_INFO() << "imagePath:" << imagePath;

        QMap<QString, QString> downloadInfo;
        downloadInfo.insert("Image Id", imageId);
        downloadInfo.insert("Image Path", imagePath);
        downloadSaveSlot(downloadInfo);
    }
}

void ImageManager::onBtnUpdate()
{
    OperateImage(IMAGE_OPERATE_TYPE_UPDATE);
}

void ImageManager::onBtnRemove()
{
    KLOG_INFO() << "onRemoveImage";
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(0);
    std::vector<int64_t> ids;
    foreach (auto &idMap, info)
    {
        KLOG_INFO() << idMap.value(IMAGE_ID).toInt();
        ids.push_back(idMap.value(IMAGE_ID).toInt());
    }

    if (!ids.empty())
    {
        MessageDialog::StandardButton ret = MessageDialog::message(tr("Delete Image"),
                                                                   tr("Are you sure you want to delete the image?"),
                                                                   tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                                                   ":/images/warning.svg",
                                                                   MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
        if (ret == MessageDialog::StandardButton::Yes)
        {
            InfoWorker::getInstance().removeImage(ids);
        }
        else
            KLOG_INFO() << "cancel";
    }
}

void ImageManager::uploadSaveSlot(QMap<QString, QString> Info)
{
    KLOG_INFO() << "Name" << Info["Image Name"] << "Version" << Info["Image Version"]
                << "Description" << Info["Image Description"] << "File" << Info["Image File"];

    QString imageFile = Info["Image File"];
    QString strSha256;
    qint64 fileSize;
    if (getImageFileInfo(imageFile, strSha256, fileSize))
        return;

    image::UploadRequest request;
    auto pInfo = request.mutable_info();
    pInfo->set_name(Info["Image Name"].toStdString());
    pInfo->set_version(Info["Image Version"].toStdString());
    QString suffix = "." + QFileInfo(imageFile).completeSuffix();
    pInfo->set_type(suffix.toStdString());
    pInfo->set_checksum(strSha256.toStdString());
    pInfo->set_description(Info["Image Description"].toStdString());
    pInfo->set_size(fileSize);

    InfoWorker::getInstance().uploadImage(request, imageFile);
}

void ImageManager::updateSaveSlot(QMap<QString, QString> Info)
{
    KLOG_INFO() << "Id" << Info["Image Id"] << "Name" << Info["Image Name"] << "Version" << Info["Image Version"]
                << "Description" << Info["Image Description"] << "File" << Info["Image File"];

    const QString imageFile = Info["Image File"];
    QString strSha256;
    qint64 fileSize;
    if (getImageFileInfo(imageFile, strSha256, fileSize))
        return;

    image::UpdateRequest request;
    request.set_image_id(Info["Image Id"].toInt());
    auto pInfo = request.mutable_info();
    pInfo->set_name(Info["Image Name"].toStdString());
    pInfo->set_version(Info["Image Version"].toStdString());
    QString suffix = "." + QFileInfo(imageFile).completeSuffix();
    pInfo->set_type(suffix.toStdString());
    pInfo->set_checksum(strSha256.toStdString());
    pInfo->set_description(Info["Image Description"].toStdString());
    pInfo->set_size(fileSize);

    QPair<grpc::Status, image::UpdateReply> reply = InfoWorker::getInstance().updateImage(request, imageFile);
    KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
    if (reply.first.ok())
    {
        KLOG_INFO() << "update images success";
        MessageDialog::message(tr("update Image"),
                               tr("update Image success!"),
                               tr(""),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        getImageList();
    }
    else
    {
        MessageDialog::message(tr("update Image"),
                               tr("update Image failed!"),
                               tr(reply.first.error_message().data()),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ImageManager::downloadSaveSlot(QMap<QString, QString> Info)
{
    KLOG_INFO() << "Id" << Info["Image Id"] << "Path" << Info["Image Path"];
    QPair<grpc::Status, downloadImageInfo> reply = InfoWorker::getInstance().downloadImage(Info["Image Id"].toInt(), Info["Image Path"]);
    KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
    bool ret = reply.first.error_code() == 0 ? true : false;
    std::string msg = reply.first.error_message();
    if (ret)
    {
        QString strSha256;
        qint64 fileSize;
        if (getImageFileInfo(reply.second.imageFile.data(), strSha256, fileSize))
            ret = false;

        if (reply.second.filesize != fileSize || reply.second.checksum != strSha256.toStdString())
        {
            KLOG_INFO() << reply.second.filesize << fileSize << reply.second.checksum.data() << strSha256.toStdString().data();
            msg = "receive data error";
            ret = false;
        }
    }

    if (ret)
    {
        KLOG_INFO() << "download images success";
        MessageDialog::message(tr("download Image"),
                               tr("download Image success!"),
                               tr(""),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        getImageList();
    }
    else
    {
        MessageDialog::message(tr("download Image"),
                               tr("download Image failed!"),
                               tr(msg.data()),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ImageManager::checkSaveSlot(QMap<QString, QString> Info)
{
    KLOG_INFO() << "Id" << Info["Image Id"] << "Check" << Info["Image Check"]
                << "Reason" << Info["Image Reason"];

    bool checkStatus = Info["Image Check"] == "Pass" ? true : false;
    InfoWorker::getInstance().checkImage(Info["Image Id"].toInt(), checkStatus, Info["Image Reason"].toStdString());
}

void ImageManager::getListDBResult(const QPair<grpc::Status, image::ListDBReply> &reply)
{
    m_imageInfoMap.clear();
    if (reply.first.ok())
    {
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
        clearTable();
        int size = reply.second.images_size();
        if (size <= 0)
        {
            setTableDefaultContent("-");
            return;
        }

        int row = 0;

        for (auto image : reply.second.images())
        {
            QMap<QString, QVariant> infoMap;
            qint64 imageId = image.id();
            infoMap.insert(IMAGE_ID, imageId);

            QStandardItem *itemCheck = new QStandardItem();
            itemCheck->setCheckable(true);

            QStandardItem *itemName = new QStandardItem(image.name().data());
            infoMap.insert(IMAGE_NAME, image.name().data());

            QStandardItem *itemVer = new QStandardItem(image.version().data());
            infoMap.insert(IMAGE_VERSION, image.version().data());

            QStandardItem *itemDesc = new QStandardItem(image.description().data());
            infoMap.insert(IMAGE_DESC, image.description().data());

            QStandardItem *itemChkStatus = new QStandardItem(image.check_status() ? tr("Passed") : tr("Rejected"));
            //TODO:add chkStatus color
            itemChkStatus->setForeground(image.check_status() ? QBrush(QColor("#00921b")) : QBrush(QColor("#d30000")));

            QStandardItem *itemApprovalStatus = new QStandardItem();
            switch (image.approval_status())
            {
            case 0:
                itemApprovalStatus->setText(tr("Wait for Approve"));
                itemApprovalStatus->setForeground(QBrush(QColor("#EEA43C")));
                break;
            case 1:
                itemApprovalStatus->setText(tr("Rejected"));
                itemApprovalStatus->setForeground(QBrush(QColor("#d30000")));
                break;
            case 2:
                itemApprovalStatus->setText(tr("Passed"));
                itemApprovalStatus->setForeground(QBrush(QColor("#00921b")));
                break;
            }

            // TODO parse unix timestamp
            QDateTime time = QDateTime::fromMSecsSinceEpoch(image.update_at() * 1000);
            QString updateTime = time.toString("yyyy/MM/dd hh:mm:ss");
            KLOG_INFO() << "due time = " << updateTime;
            QStandardItem *itemUpdateTime = new QStandardItem(updateTime);

            itemName->setData(QVariant::fromValue(infoMap));

            //            KLOG_INFO() << "imageId:" << image.id() << "name:" << image.name().data()
            //                        << "version:" << image.version().data() << "description:" << image.description().data()
            //                        << "approval_status:" << image.approval_status() << "update_time:" << image.update_time().data();

            setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemVer << itemDesc << itemChkStatus << itemApprovalStatus << itemUpdateTime);
            row++;
        }
    }
    else
    {
        KLOG_INFO() << "get ListDB Result failed: " << reply.first.error_message().data();
        setTableDefaultContent("-");
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    }
}

void ImageManager::getCheckResult(const QPair<grpc::Status, image::ApproveReply> &reply)
{
    KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
    if (reply.first.ok())
    {
        KLOG_INFO() << "check images success";
        MessageDialog::message(tr("Check Image"),
                               tr("Check Image success!"),
                               tr(""),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        getImageList();
    }
    else
    {
        MessageDialog::message(tr("Check Image"),
                               tr("Check Image failed!"),
                               tr(reply.first.error_message().data()),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ImageManager::getRemoveResult(const QPair<grpc::Status, image::RemoveReply> &reply)
{
    KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
    if (reply.first.ok())
    {
        KLOG_INFO() << "remove images success";
        MessageDialog::message(tr("Remove Image"),
                               tr("Remove Image success!"),
                               tr(""),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        for (auto id : reply.second.ok_ids())
        {
            KLOG_INFO() << "remove success id:" << id.data();
        }
        getImageList();
    }
    else
    {
        MessageDialog::message(tr("Remove Image"),
                               tr("Remove Image failed!"),
                               tr(reply.first.error_message().data()),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ImageManager::getUploadResult(const QPair<grpc::Status, image::UploadReply> &reply)
{
    if (reply.first.ok())
    {
        KLOG_INFO() << "upload images success, return id:" << reply.second.image_id();
        MessageDialog::message(tr("Upload Image"),
                               tr("Upload Image success!"),
                               tr(""),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        getImageList();
    }
    else
    {
        MessageDialog::message(tr("Upload Image"),
                               tr("Upload Image failed!"),
                               tr(reply.first.error_message().data()),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
    }
}
