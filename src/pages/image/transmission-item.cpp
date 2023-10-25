/**
 * @file          /ks-scmc-gui/src/pages/image/transmission-item.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "transmission-item.h"
#include <QPainter>
#include <QStyleOption>
#include "common/message-dialog.h"
#include "ui_transmission-item.h"
TransmissionItem::TransmissionItem(QWidget *parent) : QWidget(parent),
                                                      ui(new Ui::TransmissionItem)
{
    ui->setupUi(this);
    ui->btn_close->setIcon(QIcon(":/images/btn_close_n.svg"));
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setTextVisible(false);
    connect(ui->btn_close, &QPushButton::clicked, this, &TransmissionItem::onItemClose);
}

TransmissionItem::~TransmissionItem()
{
    delete ui;
}

void TransmissionItem::setName(QString name)
{
    QString newStr = name;
    QFontMetrics fontWidth(ui->lab_name->font());
    QString elideNote = fontWidth.elidedText(newStr, Qt::ElideRight, ui->lab_name->width());
    ui->lab_name->setText(elideNote);
    ui->lab_name->setToolTip(name);
}

void TransmissionItem::setVersion(QString version)
{
    QString newStr = version;
    QFontMetrics fontWidth(ui->lab_version->font());
    QString elideNote = fontWidth.elidedText(newStr, Qt::ElideRight, ui->lab_version->width());
    ui->lab_version->setText(elideNote);
    ui->lab_version->setToolTip(version);
}

void TransmissionItem::setStatus(ImageTransmissionStatus status)
{
    m_status = status;
    QString strStatus;
    switch (status)
    {
    case IMAGE_TRANSMISSION_STATUS_UPLOADING:
        strStatus = tr("Uploading");
        ui->progressBar->setStyleSheet("#progressBar::chunk{background-color: #00921b;}");
        break;
    case IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED:
        strStatus = tr("Uploading failed");
        ui->progressBar->setStyleSheet("#progressBar::chunk{background-color: #00921b;}");
        break;
    case IMAGE_TRANSMISSION_STATUS_UPLOADING_SUCCESSFUL:
        strStatus = tr("Uploading successful");
        ui->progressBar->setStyleSheet("#progressBar::chunk{background-color: #00921b;}");
        break;
    case IMAGE_TRANSMISSION_STATUS_DOWNLOADING:
        strStatus = tr("Downloading");
        ui->progressBar->setStyleSheet("#progressBar::chunk{background-color: #2eb3ff;}");
        break;
    case IMAGE_TRANSMISSION_STATUS_DOWNLOADING_FAILED:
        strStatus = tr("Downloading failed");
        ui->progressBar->setStyleSheet("#progressBar::chunk{background-color: #2eb3ff;}");
        break;
    case IMAGE_TRANSMISSION_STATUS_DOWNLOADING_SUCCESSFUL:
        strStatus = tr("Downloading successful");
        ui->progressBar->setStyleSheet("#progressBar::chunk{background-color: #2eb3ff;}");
        break;
    default:
        break;
    }
    ui->lab_status->setText(strStatus);
}

void TransmissionItem::setRate(int rate)
{
    ui->lab_rate->setText(QString("%1%").arg(rate));
    ui->progressBar->setValue(rate);
}

QString TransmissionItem::name()
{
    return ui->lab_name->toolTip();
}

QString TransmissionItem::version()
{
    return ui->lab_version->toolTip();
}

ImageTransmissionStatus TransmissionItem::status()
{
    return m_status;
}

void TransmissionItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void TransmissionItem::onItemClose()
{
    //判断传输状态, 若传输没有结束，则弹窗提示
    if (status() == IMAGE_TRANSMISSION_STATUS_DOWNLOADING || status() == IMAGE_TRANSMISSION_STATUS_UPLOADING)
    {
        auto ret = MessageDialog::message(tr("Cancel Transmission"),
                                          tr("Are you sure to cancel the transmission?"),
                                          tr("The image %1(%2) is being transferred,the transmission will be interrupted after close!").arg(name()).arg(version()),
                                          ":/images/warning.svg",
                                          MessageDialog::StandardButton::Cancel | MessageDialog::StandardButton::Yes);
        if (ret == MessageDialog::StandardButton::Cancel)
            return;
    }
    emit sigClose();
}
