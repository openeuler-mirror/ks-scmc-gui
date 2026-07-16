/**
 * @file          /ks-scmc-gui/src/pages/image/image-list-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "image-list-page.h"
#include <archive.h>
#include <archive_entry.h>
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDesktopWidget>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QPlainTextEdit>
#include <QStandardPaths>
#include "common/message-dialog.h"
#include "def.h"
#include "notification-manager.h"
using namespace grpc;

ImageListPage::ImageListPage(QWidget *parent, bool flag) : TablePage(parent),
                                                           m_pImageOp(nullptr),
                                                           m_securityOpen(true)
{
    is_init_audit_btn = flag;
    m_objId = Image::generateId(this);

    initButtons();
    initTable();
    initImageConnect();
    System::getInstance().getSecuritySwitch(m_objId);
}

ImageListPage::~ImageListPage()
{
    if (m_pImageOp)
    {
        delete m_pImageOp;
        m_pImageOp = nullptr;
    }
}

void ImageListPage::updateInfo(QString keyword)
{
    clearCheckState();
    clearText();
    if (keyword.isEmpty())
    {
        getImageList();
    }
}

void ImageListPage::initTable()
{
    QList<QString> tableHHeaderDate = {
        "",
        tr("Image Name"),
        tr("Version"),
        tr("Description"),
        tr("Inspection Status"),
        tr("Approval Status"),
        tr("Last Update")};
    if (is_init_audit_btn)
    {
        tableHHeaderDate.append(tr("Operation"));
        setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>{{ACTION_BUTTON_TYPE_IMAGE_PASS, QPair<QString, QString>{tr("Pass"), tr("Pass")}},
                                                                                                       {ACTION_BUTTON_TYPE_IMAGE_REFUSE, QPair<QString, QString>{tr("Refuse"), tr("Refuse")}}});
    }
    setHeaderSections(tableHHeaderDate);
    setHeaderCheckable(false);
    setTableDefaultContent("-");
    setTableSingleChoose(true);
}

void ImageListPage::initButtons()
{
    if (!is_init_audit_btn)
    {
        QMap<int, QPushButton *> opBtnMap;
        // 按钮
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
                                   "color:#ffffff;"
                                   "font-size:12px;}"
                                   "#btn:hover{ background-color:#ff6c6c;}"
                                   "#btn:focus{outline:none;}"
                                   "#btn:disabled{color:#919191;background:#393939;}");
            }
            else
                btn->setStyleSheet("#btn{background-color:#2eb3ff;"
                                   "border:none;"
                                   "border-radius: 4px;"
                                   "color:#ffffff;"
                                   "font-size:12px;}"
                                   "#btn:hover{ background-color:#77ceff;}"
                                   "#btn:focus{outline:none;}"
                                   "#btn:disabled{color:#919191;background:#393939;}");

            btn->setText(name);
            btn->setFixedSize(QSize(78, 32));
            opBtnMap.insert(iter.key(), btn);
        }
        connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_UPLOAD], &QPushButton::clicked, this, &ImageListPage::onBtnUpload);
        connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_DOWNLOAD], &QPushButton::clicked, this, &ImageListPage::onBtnDownload);
        connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_UPDATE], &QPushButton::clicked, this, &ImageListPage::onBtnUpdate);
        connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_REMOVE], &QPushButton::clicked, this, &ImageListPage::onBtnRemove);

        addSingleOperationButton(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_UPLOAD]);
        addBatchOperationButtons(QList<QPushButton *>() << opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_UPDATE]
                                                        << opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_DOWNLOAD]
                                                        << opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_REMOVE]);
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
    }
    else
    {
        QMap<int, QPushButton *> opBtnMap;
        // 按钮
        const QMap<int, QString> btnNameMap = {
            {OPERATION_BUTTOM_IMAGE_MANAGER_PASS, tr("Pass")},
            {OPERATION_BUTTOM_IMAGE_MANAGER_REFUSE, tr("Refuse")}};
        for (auto iter = btnNameMap.begin(); iter != btnNameMap.end(); iter++)
        {
            QString name = iter.value();
            QPushButton *btn = new QPushButton(this);
            btn->setObjectName("btn");

            if (name == tr("Refuse"))
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
            {
                btn->setStyleSheet("#btn{background-color:#2eb3ff;"
                                   "border:none;"
                                   "border-radius: 4px;"
                                   "color:#ffffff;}"
                                   "#btn:hover{ background-color:#77ceff;}"
                                   "#btn:focus{outline:none;}"
                                   "#btn:disabled{color:#919191;background:#393939;}");
            }
            btn->setText(name);
            btn->setFixedSize(QSize(78, 32));
            opBtnMap.insert(iter.key(), btn);
        }
        connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_PASS], &QPushButton::clicked, this, &ImageListPage::onBtnPass);
        connect(opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_REFUSE], &QPushButton::clicked, this, &ImageListPage::onBtnRefuse);
        connect(this, &ImageListPage::sigImagePass, this, &ImageListPage::onBtnPassLabel);
        connect(this, &ImageListPage::sigImageRefuse, this, &ImageListPage::onBtnRefuseLabel);

        addBatchOperationButtons(QList<QPushButton *>() << opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_PASS]
                                                        << opBtnMap[OPERATION_BUTTOM_IMAGE_MANAGER_REFUSE]);
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
    }
}

void ImageListPage::initImageConnect()
{
    connect(&Image::getInstance(), &Image::listDBImageFinished, this, &ImageListPage::getListDBResult, Qt::UniqueConnection);
    connect(&Image::getInstance(), &Image::checkImageFinished, this, &ImageListPage::getCheckResult, Qt::UniqueConnection);
    connect(&Image::getInstance(), &Image::removeImageFinished, this, &ImageListPage::getRemoveResult, Qt::UniqueConnection);
    connect(&Image::getInstance(), &Image::uploadFinished, this, &ImageListPage::getUploadResult, Qt::UniqueConnection);
    connect(&Image::getInstance(), &Image::updateFinished, this, &ImageListPage::getUpdateResult, Qt::UniqueConnection);
    connect(&Image::getInstance(), &Image::downloadImageFinished, this, &ImageListPage::getDownloadImageResult, Qt::UniqueConnection);
    connect(&System::getInstance(), &System::getSecuritySwitchFinished, this, &ImageListPage::getSecuritySwitchResult, Qt::UniqueConnection);
    connect(&Common::getInstance(), &Common::transferImageFinished, this, &ImageListPage::getTransferImageFinishedResult, Qt::BlockingQueuedConnection);
}

int ImageListPage::getImageFileInfo(const QString fileName, QString &strSha256, qint64 &fileSize)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))
    {
        KLOG_WARNING() << "Failed to open " << fileName;
        return -1;
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);
    QByteArray buffer(1024 * 1024, '\0');
    int bytesRead = 0;
    while ((bytesRead = f.read(buffer.data(), buffer.size())) > 0)
    {
        hash.addData(buffer.constData(), bytesRead);
    }
    f.close();

    strSha256 = hash.result().toHex();
    fileSize = f.size();
    KLOG_DEBUG() << "Get file hash:" << strSha256 << ", fileSize:" << fileSize;
    return 0;
}

QString ImageListPage::checkImageLegality(const QString &fileName)
{
    struct archive *pArchive = archive_read_new();
    archive_read_support_format_tar(pArchive);
    size_t blockSize = 10240;
    if (archive_read_open_filename(pArchive, fileName.toStdString().c_str(), blockSize) != ARCHIVE_OK)
    {
        archive_read_free(pArchive);
        return tr("Image file is damaged");  // 镜像文件损坏
    }

    const QString manifestFile = "manifest.json";
    QStringList fileList;
    QString manifestContent;
    struct archive_entry *pEntry = nullptr;
    while (archive_read_next_header(pArchive, &pEntry) == ARCHIVE_OK)
    {
        QString pathName(archive_entry_pathname(pEntry));
        fileList << pathName;
        if (pathName == manifestFile)
        {
            size_t entrySize = archive_entry_size(pEntry);
            QByteArray buffer(entrySize + 1, '\0');
            archive_read_data(pArchive, buffer.data(), entrySize);
            manifestContent = buffer.constData();
        }
    }

    bool hasRootDir = false;
    for (const auto &file : fileList)
    {
        KLOG_DEBUG() << "file: " << file;
        if (file == "/")
        {
            hasRootDir = true;
            break;
        }
    }

    archive_read_close(pArchive);
    archive_read_free(pArchive);
    if (hasRootDir)
    {
        return QString();
    }

    if (!fileList.contains(manifestFile))
    {
        return tr("Image tar package without manifest.json");  // 镜像没有 manifest.json 文件
    }

    return parseManifest(manifestContent, fileList);
}

QString ImageListPage::parseManifest(const QString &manifestContent, const QStringList &fileList)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(manifestContent.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isArray())
    {
        return tr("Failed to read image data"); // 读写镜像数据失败
    }

    const QJsonArray &jsonArray = jsonDoc.array();
    for (QJsonValue data : jsonArray)
    {
        if (!data.isObject())
        {
            return tr("Failed to read image data");
        }

        const QJsonObject &jsonObj = data.toObject();
        if (jsonObj.contains("Config") && jsonObj["Config"].isString())
        {
            if (fileList.indexOf(jsonObj["Config"].toString()) == -1)
            {
                return tr("Image tar package without") + jsonObj["Config"].toString();  // 镜像没有$config
            }
        }

        if (jsonObj.contains("Layers") && jsonObj["Layers"].isArray())
        {
            for (auto layer : jsonObj["Layers"].toArray())
            {
                if (fileList.indexOf(layer.toString()) == -1)
                {
                    return tr("Image tar package without") + layer.toString();  // 镜像没有$layer
                }
            }
        }
    }

    return QString();
}

void ImageListPage::getImageList()
{
    setBusy(true);
    InfoWorker::getInstance().listDBImage(m_objId);
}

void ImageListPage::OperateImage(ImageOperateType type)
{
    if (!m_pImageOp)
    {
        m_pImageOp = new ImageOperateDialog(type, m_securityOpen);
        if (type == IMAGE_OPERATE_TYPE_UPDATE)
        {
            QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);  //只能选择一个
            if (!info.isEmpty())
                m_pImageOp->setImageInfo(info.at(0));
        }

        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_pImageOp->move(screenGeometry.x() + (screenGeometry.width() - m_pImageOp->width()) / 2,
                         screenGeometry.y() + (screenGeometry.height() - m_pImageOp->height()) / 2);

        m_pImageOp->show();

        switch (type)
        {
        case IMAGE_OPERATE_TYPE_UPLOAD:
            connect(m_pImageOp, &ImageOperateDialog::sigUploadSave, this, &ImageListPage::uploadSaveSlot);
            break;
        case IMAGE_OPERATE_TYPE_UPDATE:
            connect(m_pImageOp, &ImageOperateDialog::sigUpdateSave, this, &ImageListPage::updateSaveSlot);
            break;
        case IMAGE_OPERATE_TYPE_DOWNLOAD:
            connect(m_pImageOp, &ImageOperateDialog::sigDownloadSave, this, &ImageListPage::downloadSaveSlot);
            break;
        default:
            break;
        }
        connect(m_pImageOp, &ImageOperateDialog::destroyed,
                [=] {
                    m_pImageOp->deleteLater();
                    m_pImageOp = nullptr;
                });
    }
}

bool ImageListPage::imageIsTransfering(QString imageName, QString version, QString title)
{
    if (m_transferImages.contains(imageName + "-" + version))
    {
        MessageDialog::message(title,
                               tr("The image \"%1\" is being transferred.").arg(imageName),
                               tr(" Please operate after the transfer is completed!"),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        return true;
    }
    return false;
}

QString ImageListPage::getRefuseReason(bool *ok)
{
    QInputDialog *dlg = new QInputDialog(this);
    dlg->setObjectName("InputDialog");
    dlg->setFixedSize(400, 300);
    dlg->setStyleSheet("#InputDialog QLabel{font-size:12px;}");
    dlg->setOptions(QInputDialog::UsePlainTextEditForTextInput);
    dlg->setLabelText(tr("Please input refuse reason:"));
    dlg->setInputMethodHints(Qt::ImhNone);
    dlg->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    auto layout = dlg->layout();
    layout->setMargin(0);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(10);

    auto textEdit = dlg->findChild<QPlainTextEdit *>();
    textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);

    //限制输入最多200个字符
    connect(dlg, &QInputDialog::textValueChanged,
            [=](QString text) {
                int length = text.count();
                int maxLength = 200;  // 最大字符数
                if (length > maxLength)
                {
                    int position = textEdit->textCursor().position();
                    QTextCursor textCursor = textEdit->textCursor();
                    text.remove(position - (length - maxLength), length - maxLength);
                    textEdit->setPlainText(text);
                    textCursor.setPosition(position - (length - maxLength));
                    textEdit->setTextCursor(textCursor);
                }
            });

    auto buttonBox = dlg->findChild<QDialogButtonBox *>();
    auto okBtn = buttonBox->button(QDialogButtonBox::Ok);
    auto cancelBtn = buttonBox->button(QDialogButtonBox::Cancel);
    Kiran::WidgetPropertyHelper::setButtonType(okBtn, Kiran::BUTTON_Default);
    okBtn->setFixedSize(78, 32);
    cancelBtn->setFixedSize(78, 32);
    okBtn->setText(tr("OK"));
    cancelBtn->setText(tr("Cancel"));

    const int ret = dlg->exec();
    if (ok)
        *ok = !!ret;
    if (ret)
        return dlg->textValue();
    else
        return QString();
}

void ImageListPage::onBtnUpload()
{
    OperateImage(IMAGE_OPERATE_TYPE_UPLOAD);
}

void ImageListPage::onBtnDownload()
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);
    if (!info.isEmpty())
    {
        QString imageId = info.at(0).value(IMAGE_ID).toString();
        QString imageName = info.at(0).value(IMAGE_NAME).toString();
        QString imageVersion = info.at(0).value(IMAGE_VERSION).toString();

        QFileDialog *pFile = new QFileDialog(this);
        QString imagePath = pFile->getExistingDirectory(this, tr("Please select the path to save"), "./");
        if (imagePath.isEmpty())
        {
            return;
        }
        if (imagePath.at(imagePath.size() - 1) != "/")
            imagePath += "/";
        KLOG_DEBUG() << "Download image:" << imagePath;

        QMap<QString, QString> downloadInfo;
        downloadInfo.insert("Image Name", imageName);
        downloadInfo.insert("Image Version", imageVersion);
        downloadInfo.insert("Image Id", imageId);
        downloadInfo.insert("Image Path", imagePath);
        downloadSaveSlot(downloadInfo);
    }
}

void ImageListPage::onBtnUpdate()
{
    OperateImage(IMAGE_OPERATE_TYPE_UPDATE);
}

void ImageListPage::onBtnRemove()
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);
    std::vector<int64_t> ids;
    foreach (auto &idMap, info)
    {
        KLOG_DEBUG() << "Remove image: " << idMap.value(IMAGE_ID).toInt();
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
            InfoWorker::getInstance().removeImage(m_objId, ids);
        }
    }
}

void ImageListPage::onBtnPass()
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);
    if (!info.isEmpty())
    {
        QString imageId = info.at(0).value(IMAGE_ID).toString();

        QMap<QString, QString> checkInfo;
        checkInfo.insert("Image Id", imageId);
        checkInfo.insert("Image Check", "Pass");
        checkInfo.insert("Image Reason", "");
        checkSaveSlot(checkInfo);
    }
}

void ImageListPage::onBtnRefuse()
{
    QList<QMap<QString, QVariant>> info = getCheckedItemInfo(1);
    if (!info.isEmpty())
    {
        QString imageId = info.at(0).value(IMAGE_ID).toString();
        bool ok;
        QString reson = getRefuseReason(&ok);
        if (ok)
        {
            QMap<QString, QString> checkInfo;
            checkInfo.insert("Image Id", imageId);
            checkInfo.insert("Image Check", "Refuse");
            checkInfo.insert("Image Reason", reson);
            checkSaveSlot(checkInfo);
        }
    }
}

void ImageListPage::onBtnPassLabel(int row)
{
    auto infoMap = getItem(row, 1)->data().value<QMap<QString, QVariant>>();
    if (infoMap.isEmpty())
        return;
    auto image_id = infoMap.value(IMAGE_ID).toString();
    QMap<QString, QString> checkInfo;
    checkInfo.insert("Image Id", image_id);
    checkInfo.insert("Image Check", "Pass");
    checkInfo.insert("Image Reason", "");
    checkSaveSlot(checkInfo);
}

void ImageListPage::onBtnRefuseLabel(int row)
{
    auto infoMap = getItem(row, 1)->data().value<QMap<QString, QVariant>>();
    if (infoMap.isEmpty())
        return;

    bool ok;
    QString reson = getRefuseReason(&ok);
    if (ok)
    {
        auto image_id = infoMap.value(IMAGE_ID).toString();

        QMap<QString, QString> checkInfo;
        checkInfo.insert("Image Id", image_id);
        checkInfo.insert("Image Check", "Refuse");
        checkInfo.insert("Image Reason", reson);
        checkSaveSlot(checkInfo);
    }
}

void ImageListPage::uploadSaveSlot(QMap<QString, QString> Info)
{
    KLOG_DEBUG() << "Upload image."
                 << "Name" << Info["Image Name"] << "Version" << Info["Image Version"]
                 << "Description" << Info["Image Description"] << "File" << Info["Image File"];

    const QString imageFile = Info["Image File"];
    const QString signFile = Info["Sign File"];
    QString strSha256;
    qint64 fileSize;
    if (0 > getImageFileInfo(imageFile, strSha256, fileSize))
    {
        MessageDialog::message(tr("Upload Image"),
                               tr("Can't open \"%1\" file").arg(imageFile),
                               tr("Please make sure the file exists!"),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }
//    QString checkRet = checkImageLegality(imageFile);
//    if (!checkRet.isEmpty())
//    {
//        MessageDialog::message(tr("Upload Image"),
//                               tr("Upload image failed!"),
//                               checkRet,
//                               ":/images/error.svg",
//                               MessageDialog::StandardButton::Ok);
//        return;
//    }

    //在检查文件成功后再将其加入传输任务列表
    if (!imageIsTransfering(Info["Image Name"], Info["Image Version"], tr("Upload Image")))
    {
        m_transferImages.append(Info["Image Name"] + "-" + Info["Image Version"]);
    }
    else
        return;

    image::UploadRequest request;
    auto pInfo = request.mutable_info();
    pInfo->set_name(Info["Image Name"].toStdString());
    pInfo->set_version(Info["Image Version"].toStdString());
    QString suffix = "." + QFileInfo(imageFile).suffix();
    pInfo->set_type(suffix.toStdString());
    pInfo->set_checksum(strSha256.toStdString());
    pInfo->set_description(Info["Image Description"].toStdString());
    pInfo->set_size(fileSize);

    if (!signFile.isEmpty())
    {
        auto pSignInfo = request.mutable_sign();
        QFileInfo fileInfo = QFileInfo(signFile);
        KLOG_DEBUG() << "Get sign file info:" << signFile << fileInfo.fileName() << "file size: " << fileInfo.size();
        pSignInfo->set_size(fileInfo.size());
        pSignInfo->mutable_chunk_data();
    }

    InfoWorker::getInstance().stopTransfer(Info["Image Name"], Info["Image Version"], false);
    InfoWorker::getInstance().uploadImage(m_objId, request, imageFile, signFile);
}

void ImageListPage::updateSaveSlot(QMap<QString, QString> Info)
{
    KLOG_DEBUG() << "Update image."
                 << "Id" << Info["Image Id"] << "Name" << Info["Image Name"] << "Version" << Info["Image Version"]
                 << "Description" << Info["Image Description"] << "File" << Info["Image File"];

    const QString imageFile = Info["Image File"];
    const QString signFile = Info["Sign File"];
    const QString desc = Info["Image Description"];
    QString strSha256;
    qint64 fileSize = 0;
    bool check = false;

    if (!imageFile.isEmpty())
    {
        if (0 > getImageFileInfo(imageFile, strSha256, fileSize))
        {
            MessageDialog::message(tr("Update Image"),
                                   tr("Can't open \"%1\" file").arg(imageFile),
                                   tr("Please make sure the file exists!"),
                                   ":/images/warning.svg",
                                   MessageDialog::StandardButton::Ok);
            return;
        }
        else
            check = true;

//        QString checkRet = checkImageLegality(imageFile);
//        if (!checkRet.isEmpty())
//        {
//            MessageDialog::message(tr("Update Image"),
//                                   tr("Update image failed!"),
//                                   checkRet,
//                                   ":/images/error.svg",
//                                   MessageDialog::StandardButton::Ok);
//            return;
//        }
    }

    //在检查文件成功后再将其加入传输任务列表
    if (!imageIsTransfering(Info["Image Name"], Info["Image Version"], tr("Update Image")))
    {
        KLOG_DEBUG() << "Append" << Info["Image Name"] << "-" << Info["Image Version"] << "to transfering image.";
        m_transferImages.append(Info["Image Name"] + "-" + Info["Image Version"]);
    }
    else
        return;

    image::UpdateRequest request;
    request.set_image_id(Info["Image Id"].toInt());
    auto pInfo = request.mutable_info();
    pInfo->set_name(Info["Image Name"].toStdString());
    pInfo->set_version(Info["Image Version"].toStdString());

    if (!desc.isEmpty())
        pInfo->set_description(Info["Image Description"].toStdString());

    if (check)
    {
        QString suffix = "." + QFileInfo(imageFile).suffix();
        pInfo->set_type(suffix.toStdString());
        pInfo->set_checksum(strSha256.toStdString());

        if (!signFile.isEmpty())
        {
            auto pSignInfo = request.mutable_sign();
            QFileInfo fileInfo = QFileInfo(signFile);
            KLOG_DEBUG() << "Get sign file info:" << signFile << fileInfo.fileName() << "file size: " << fileInfo.size();
            pSignInfo->set_size(fileInfo.size());
            pSignInfo->mutable_chunk_data();
        }
    }

    pInfo->set_size(fileSize);

    InfoWorker::getInstance().stopTransfer(Info["Image Name"], Info["Image Version"], false);
    InfoWorker::getInstance().updateImage(m_objId, request, imageFile, Info["Sign File"]);
}

void ImageListPage::downloadSaveSlot(QMap<QString, QString> Info)
{
    KLOG_DEBUG() << "Download image."
                 << "Id" << Info["Image Id"] << "Path" << Info["Image Path"];

    if (!imageIsTransfering(Info["Image Name"], Info["Image Version"], tr("Download Image")))
    {
        m_transferImages.append(Info["Image Name"] + "-" + Info["Image Version"]);
    }
    else
        return;

    InfoWorker::getInstance().stopTransfer(Info["Image Name"], Info["Image Version"], false);
    InfoWorker::getInstance().downloadImage(m_objId, Info["Image Id"].toInt(), Info["Image Name"], Info["Image Version"], Info["Image Path"]);
}

void ImageListPage::checkSaveSlot(QMap<QString, QString> Info)
{
    KLOG_DEBUG() << "Check image."
                 << "Id" << Info["Image Id"] << "Check" << Info["Image Check"]
                 << "Reason" << Info["Image Reason"];

    bool checkStatus = Info["Image Check"] == "Pass" ? true : false;

    //    InfoWorker::getInstance().stopTransfer(Info["Image Name"], Info["Image Version"], false);
    InfoWorker::getInstance().checkImage(m_objId, Info["Image Id"].toInt(), checkStatus, Info["Image Reason"].toStdString());
}

void ImageListPage::getListDBResult(const QString objId, const QPair<grpc::Status, image::ListDBReply> &reply)
{
    if (m_objId != objId)
        return;

    setBusy(false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Get ListDB Result failed: " << reply.first.error_message().data();
        setTableDefaultContent("-");
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
        if (DEADLINE_EXCEEDED == reply.first.error_code())
        {
            setTips(tr("Response timeout!"));
        }
        return;
    }

    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
    clearTable();
    int size = reply.second.images_size();
    if (size <= 0)
    {
        emit sigUpdateAPproveTipSums();
        setTableDefaultContent("-");
        return;
    }

    int row = 0;
    int count = 0;
    for (auto image : reply.second.images())
    {
        if (image.approval_status() == 0)
            count++;
        QMap<QString, QVariant> infoMap;
        qint64 imageId = image.id();
        infoMap.insert(IMAGE_ID, imageId);

        QStandardItem *itemCheck = new QStandardItem();
        if (is_open_checkbox)
            itemCheck->setCheckable(true);
        else
            itemCheck->setCheckable(false);

        QStandardItem *itemName = new QStandardItem(image.name().data());
        infoMap.insert(IMAGE_NAME, image.name().data());

        QStandardItem *itemVer = new QStandardItem(image.version().data());
        infoMap.insert(IMAGE_VERSION, image.version().data());

        QStandardItem *itemDesc = new QStandardItem(image.description().data());
        infoMap.insert(IMAGE_DESC, image.description().data());

        QStandardItem *itemChkStatus = new QStandardItem();
        switch (image.verify_status())
        {
        case 0:
            itemChkStatus->setText(tr("Failed"));
            itemChkStatus->setForeground(QBrush(QColor("#d30000")));
            break;
        case 1:
            itemChkStatus->setText(tr("Abnormal"));
            itemChkStatus->setForeground(QBrush(QColor("#d30000")));
            break;
        case 2:
            itemChkStatus->setText(tr("Passed"));
            itemChkStatus->setForeground(QBrush(QColor("#00921b")));
            break;
        }

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
        QStandardItem *itemUpdateTime = new QStandardItem(updateTime);

        itemName->setData(QVariant::fromValue(infoMap));

        for (int i = 0; i < is_del_row.count(); i++)
        {
            QString str = itemApprovalStatus->text();
            if (is_del_row[i] == str)
                goto _END;
        }

        setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemVer << itemDesc << itemChkStatus << itemApprovalStatus << itemUpdateTime);
        row++;
    _END:
        continue;
    }
    if (getRowCount() == 0)
    {
        setTableDefaultContent("-");
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    }
    emit sigUpdateAPproveTipSums(count);
}

void ImageListPage::setDelRow(const QString type1, const QString type2)
{
    if (!is_del_row.isEmpty())
        is_del_row.clear();
    is_del_row.append(type1);
    is_del_row.append(type2);
}

void ImageListPage::getCheckResult(const QString objId, const QPair<grpc::Status, image::ApproveReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        getImageList();
    }
    else
    {
        MessageDialog::message(tr("Approve Image"),
                               tr("Approve image failed!"),
                               reply.first.error_message().data(),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ImageListPage::getRemoveResult(const QString objId, const QPair<grpc::Status, image::RemoveReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        NotificationManager::sendNotify(tr("Remove image success!"), "");
        getImageList();
    }
    else
    {
        MessageDialog::message(tr("Remove Image"),
                               tr("Remove image failed!"),
                               reply.first.error_message().data(),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ImageListPage::getUploadResult(const QString objId, const QPair<grpc::Status, image::UploadReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        KLOG_WARNING() << "Upload images success, return id:" << reply.second.image_id();
        NotificationManager::sendNotify(tr("Upload image success!"), "");
        getImageList();
    }
    else
    {
        MessageDialog::message(tr("Upload Image"),
                               tr("Upload image failed!"),
                               reply.first.error_message().data(),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ImageListPage::getUpdateResult(const QString objId, const QPair<grpc::Status, image::UpdateReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        NotificationManager::sendNotify(tr("Update image success!"), "");
        getImageList();
    }
    else
    {
        MessageDialog::message(tr("Update Image"),
                               tr("Update image failed!"),
                               reply.first.error_message().data(),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}

void ImageListPage::getDownloadImageResult(const QString objId, const QPair<grpc::Status, downloadImageInfo> &reply)
{
    if (m_objId != objId)
        return;

    bool ret = reply.first.error_code() == 0 ? true : false;
    std::string msg = reply.first.error_message();

    if (ret)
    {
        NotificationManager::sendNotify(tr("Download image success!"), "");
        getImageList();
    }
    else
    {
        MessageDialog::message(tr("Download Image"),
                               tr("Download image failed!"),
                               msg.data(),
                               ":/images/error.svg",
                               MessageDialog::StandardButton::Ok);
    }
}
void ImageListPage::getSecuritySwitchResult(const QString objId, const QPair<grpc::Status, sys::GetSecuritySwitchReply> &reply)
{
    if (m_objId != objId)
        return;

    if (reply.first.ok())
    {
        m_securityOpen = reply.second.is_on();
    }
    else
    {
        KLOG_WARNING() << "Get security switch result failed!" << reply.first.error_message().data();
    }
}

void ImageListPage::getTransferImageFinishedResult(QString name, QString version)
{
    QString image = name + "-" + version;
    QMutexLocker locker(&m_mutex);
    if (m_transferImages.contains(image))
    {
        m_transferImages.removeAll(image);
    }
}
void ImageListPage::setCheckBox(bool flag)
{
    is_open_checkbox = flag;
}
