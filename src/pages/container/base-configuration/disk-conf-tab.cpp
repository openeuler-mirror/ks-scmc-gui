/**
 * @file          src/pages/container/base-configuration/disk-conf-tab.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "disk-conf-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QIntValidator>
#include "ui_disk-conf-tab.h"

DiskConfTab::DiskConfTab(QWidget* parent) : QWidget(parent),
                                            ui(new Ui::DiskConfTab)
{
    ui->setupUi(this);

    ui->lineEdit_read_speed->setTextMargins(10, 0, 0, 0);
    ui->lineEdit_write_speed->setTextMargins(10, 0, 0, 0);

    ui->lineEdit_read_speed->setPlaceholderText(tr("0-4096, default 0"));
    ui->lineEdit_write_speed->setPlaceholderText(tr("0-4096, default 0"));

    ui->lineEdit_read_speed->setText("0");
    ui->lineEdit_write_speed->setText("0");

    ui->lineEdit_read_speed->setValidator(new QIntValidator(0, 4096));
    ui->lineEdit_write_speed->setValidator(new QIntValidator(0, 4096));
}

DiskConfTab::~DiskConfTab()
{
    delete ui;
}

void DiskConfTab::setSpeed(container::ResourceLimit* cfg)
{
    if (!cfg)
        return;

    auto readSpeed = cfg->disk_rbw();
    auto writeSpeed = cfg->disk_wbw();
    KLOG_INFO() << "Get disk speed from backed:"
                << "read speed:" << readSpeed
                << "write speed:" << writeSpeed;

    ui->lineEdit_read_speed->setText(QString::number(readSpeed));
    ui->lineEdit_write_speed->setText(QString::number(writeSpeed));
}

void DiskConfTab::getSpeed(container::ResourceLimit* cfg)
{
    if (!cfg)
        return;

    auto readSpeed = ui->lineEdit_read_speed->text().toInt();
    auto writeSpeed = ui->lineEdit_write_speed->text().toInt();

    KLOG_INFO() << "Set disk speed to backed:"
                << "read speed:" << readSpeed
                << "write speed:" << writeSpeed;

    cfg->set_disk_rbw(readSpeed);
    cfg->set_disk_wbw(writeSpeed);
}
