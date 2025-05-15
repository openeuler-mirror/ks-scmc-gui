/**
 * @file          src/pages/container/container-manager/container-update-image.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "container-update-image.h"
#include <widget-property-helper.h>
#include "ui_container-update-image.h"

ContainerUpdateImage::ContainerUpdateImage(QWidget *parent) : KiranTitlebarWindow(parent),
                                                              ui(new Ui::ContainerUpdateImage)
{
    ui->setupUi(getWindowContentWidget());
    initUI();
}

ContainerUpdateImage::~ContainerUpdateImage()
{
    delete ui;
}

void ContainerUpdateImage::setContainerInfo(const QString &containerID, const QString &containerName)
{
    m_containerID = containerID;
    ui->lineEdit_container_name->setText(containerName);
}

QString ContainerUpdateImage::getContainerID()
{
    return m_containerID;
}

void ContainerUpdateImage::setImageInfo(const QString &imageName, const QStringList &imageInfo)
{
    QStringList versions;
    auto currentName = imageName.split(":").first();
    auto currentVersion = imageName.split(":").last();

    for (auto image : imageInfo)
    {
        auto info = image.split(":");
        if (info.size() != 2)
            continue;

        auto name = image.split(":")[0];
        auto version = image.split(":")[1];

        if (!name.compare(currentName) && !versions.contains(version))
        {
            versions.append(version);
        }
    }

    ui->lineEdit_image_name->setText(currentName);
    ui->comboBox_image_version->addItems(versions);
    ui->comboBox_image_version->setCurrentText(currentVersion);
}

void ContainerUpdateImage::getImageInfo(QString &imageName, QString &imageVersion)
{
    imageName = ui->lineEdit_image_name->text();
    imageVersion = ui->comboBox_image_version->currentText();
}

void ContainerUpdateImage::initUI()
{
    setWindowModality(Qt::ApplicationModal);
    setButtonHints(TitlebarCloseButtonHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(QSize(600, 400));

    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_confirm, Kiran::BUTTON_Default);

    ui->lineEdit_container_name->setDisabled(true);
    ui->lineEdit_image_name->setDisabled(true);

    connect(ui->btn_cancel, &QPushButton::clicked, this, &ContainerUpdateImage::close);
    connect(ui->btn_confirm, &QPushButton::clicked, this, &ContainerUpdateImage::updateImage);
}
