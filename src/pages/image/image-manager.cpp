#include "image-manager.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QCryptographicHash>
#include <QDesktopWidget>
#include <QFile>
#include <QFileInfo>
#include "common/def.h"
#include "common/message-dialog.h"
#include "image-operate.h"
#define IMAGE_ID "image id"
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
        tr("Last Update"),
        tr("Quick Actions")};
    setHeaderSections(tableHHeaderDate);
    setTableActions(tableHHeaderDate.size() - 1, QStringList() << ":/images/edit.svg");
    setTableDefaultContent("-");
}

void ImageManager::initButtons()
{
    QMap<int, QPushButton *> opBtnMap;
    //按钮
    const QMap<int, QString> btnNameMap = {
        {OPERATION_BUTTOM_IMAGE_MANAGER_IMPORT, tr("Import")},
        {OPERATION_BUTTOM_IMAGE_MANAGER_EXPORT, tr("Export")},
        {OPERATION_BUTTOM_IMAGE_MANAGER_EDIT, tr("Edit")},
        {OPERATION_BUTTOM_IMAGE_MANAGER_CHECK, tr("Check")},
        {OPERATION_BUTTOM_IMAGE_MANAGER_REMOVE, tr("Remove")}};

    for (auto iter = btnNameMap.begin(); iter != btnNameMap.end(); iter++)
    {
        QString name = iter.value();
        QPushButton *btn = new QPushButton(this);

        btn->setObjectName("btn");
        btn->setStyleSheet("#btn{background-color:#2eb3ff;"
                           "border:none;"
                           "border-radius: 4px;"
                           "color:#ffffff;}"
                           "#btn:hover{ background-color:#77ceff;}"
                           "#btn:focus{outline:none;}");

        btn->setText(name);
        btn->setFixedSize(QSize(78, 32));
        opBtnMap.insert(iter.key(), btn);
    }
    connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_IMPORT], &QPushButton::clicked, this, &ImageManager::onBtnImport);
    connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_EXPORT], &QPushButton::clicked, this, &ImageManager::onBtnExport);
    connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_EDIT], &QPushButton::clicked, this, &ImageManager::onBtnEdit);
    connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_CHECK], &QPushButton::clicked, this, &ImageManager::onBtnCheck);
    connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_REMOVE], &QPushButton::clicked, this, &ImageManager::onBtnRemove);

    addBatchOperationButtons(opBtnMap.values());
}

void ImageManager::initImageConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::listDBImageFinished, this, &ImageManager::getListDBResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::checkImageFinished, this, &ImageManager::getCheckResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::removeImageFinished, this, &ImageManager::getRemoveResult);
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

void ImageManager::OperateImage(int page)
{
    if (!m_pImageOp)
    {
        m_pImageOp = new ImageOperate(page, m_IdNameMap);
        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_pImageOp->move(screenGeometry.x() + (screenGeometry.width() - this->width()) / 2,
                         screenGeometry.y() + (screenGeometry.height() - this->height()) / 2);

        m_pImageOp->show();

        switch (page)
        {
        case UPLOAD_PAGE:
            connect(m_pImageOp, &ImageOperate::sigUploadSave, this, &ImageManager::uploadSaveSlot);
            break;
        case UPDATE_PAGE:
            connect(m_pImageOp, &ImageOperate::sigUpdateSave, this, &ImageManager::updateSaveSlot);
            break;
        case DOWNLOAD_PAGE:
            connect(m_pImageOp, &ImageOperate::sigDownloadSave, this, &ImageManager::downloadSaveSlot);
            break;
        case CHECK_PAGE:
            connect(m_pImageOp, &ImageOperate::sigCheckSave, this, &ImageManager::checkSaveSlot);
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

void ImageManager::onBtnImport()
{
    OperateImage(UPLOAD_PAGE);
}

void ImageManager::onBtnExport()
{
    OperateImage(DOWNLOAD_PAGE);
}

void ImageManager::onBtnEdit()
{
    OperateImage(UPDATE_PAGE);
}

void ImageManager::onBtnCheck()
{
    OperateImage(CHECK_PAGE);
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

    QPair<grpc::Status, image::UploadReply> reply = InfoWorker::getInstance().uploadImage(request, imageFile);
    KLOG_INFO() << reply.first.error_code() << reply.first.error_message().data();
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
    m_IdNameMap.clear();
    if (reply.first.ok())
    {
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
        clearTable();
        int size = reply.second.images_size();
        if (size <= 0)
        {
            return;
        }

        int row = 0;
        QMap<QString, QVariant> idMap;
        for (auto image : reply.second.images())
        {
            qint64 imageId = image.id();
            idMap.insert(IMAGE_ID, imageId);
            QString str = QString("%1_%2").arg(image.name().data()).arg(image.version().data());
            m_IdNameMap.insert(str, imageId);

            QStandardItem *itemCheck = new QStandardItem();
            itemCheck->setCheckable(true);

            QStandardItem *itemName = new QStandardItem(image.name().data());
            itemName->setData(QVariant::fromValue(idMap));
            itemName->setTextAlignment(Qt::AlignCenter);

            QStandardItem *itemVer = new QStandardItem(image.version().data());
            itemVer->setTextAlignment(Qt::AlignCenter);

            QStandardItem *itemDesc = new QStandardItem(image.description().data());
            itemDesc->setTextAlignment(Qt::AlignCenter);

            QStandardItem *itemChkStatus = new QStandardItem("Passed");
            itemChkStatus->setTextAlignment(Qt::AlignCenter);

            QStandardItem *itemApprovalStatus = new QStandardItem(image.approval_status() ? "Passed" : "Rejected");
            itemApprovalStatus->setForeground(image.approval_status() ? QBrush(QColor("#00921b")) : QBrush(QColor("#d30000")));
            itemApprovalStatus->setTextAlignment(Qt::AlignCenter);

            QStandardItem *itemUpdateTime = new QStandardItem(image.update_time().data());
            itemUpdateTime->setTextAlignment(Qt::AlignCenter);

            KLOG_INFO() << "imageId:" << image.id() << "name:" << image.name().data()
                        << "version:" << image.version().data() << "description:" << image.description().data()
                        << "approval_status:" << image.approval_status() << "update_time:" << image.update_time().data();

            setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemVer << itemDesc << itemChkStatus << itemApprovalStatus << itemUpdateTime);
            row++;
        }
    }
}

void ImageManager::getCheckResult(const QPair<grpc::Status, image::CheckReply> &reply)
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
