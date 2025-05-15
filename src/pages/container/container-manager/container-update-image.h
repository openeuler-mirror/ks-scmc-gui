/**
 * @file          src/pages/container/container-manager/container-update-image.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#ifndef CONTAINERUPDATEIMAGE_H
#define CONTAINERUPDATEIMAGE_H

#include <kiran-titlebar-window.h>
#include <QWidget>

namespace Ui
{
class ContainerUpdateImage;
}

class ContainerUpdateImage : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit ContainerUpdateImage(QWidget *parent = nullptr);
    ~ContainerUpdateImage();
    void setContainerInfo(const QString &containerID, const QString &containerName);
    QString getContainerID();

    void setImageInfo(const QString &imageName, const QStringList &imageInfo);
    void getImageInfo(QString &imageName, QString &imageVersion);

private:
    void initUI();

signals:
    void updateImage();

private:
    Ui::ContainerUpdateImage *ui;
    QString m_containerID;
};

#endif  // CONTAINERUPDATEIMAGE_H
