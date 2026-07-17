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

void ContainerUpdateImage::setContainerInfo(int containerID, const QString &containerName)
{
    m_containerID = containerID;
    ui->lineEdit_container_name->setText(containerName);
}

void ContainerUpdateImage::setImageInfo(int imageID, const QString &imageName, const QStringList &imageVersions)
{
    m_imageID = imageID;
    ui->lineEdit_image_name->setText(imageName);
    ui->comboBox_image_version->addItems(imageVersions);
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
