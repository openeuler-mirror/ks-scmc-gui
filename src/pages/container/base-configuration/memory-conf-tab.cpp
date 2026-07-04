/**
 * @file          /ks-scmc-gui/src/pages/container/base-configuration/memory-conf-tab.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "memory-conf-tab.h"
#include <kiran-log/qt5-log-i.h>
#include "ui_memory-conf-tab.h"
MemoryConfTab::MemoryConfTab(QWidget *parent) : QWidget(parent),
                                                ui(new Ui::MemoryConfTab)
{
    ui->setupUi(this);
    QList<QComboBox *> comboboxs = this->findChildren<QComboBox *>();
    foreach (QComboBox *cb, comboboxs)
    {
        cb->addItems(QStringList() << "MB"
                                   << "GB");
    }
    ui->lineEdit_soft_limit->setTextMargins(10, 0, 0, 0);
    ui->lineEdit_max_limit->setTextMargins(10, 0, 0, 0);

    QRegExp regExp("[0-9]+\\.?[0-9]+");
    ui->lineEdit_soft_limit->setValidator(new QRegExpValidator(regExp));
    ui->lineEdit_max_limit->setValidator(new QRegExpValidator(regExp));
}

MemoryConfTab::~MemoryConfTab()
{
    delete ui;
}

void MemoryConfTab::setMemoryInfo(container::ResourceLimit *cfg)
{
    if (cfg)
    {
        int memLimit = limitDataHandle(cfg->memory_limit(), ui->cb_max_unit);
        int softLimit = limitDataHandle(cfg->memory_soft_limit(), ui->cb_soft_unit);
        KLOG_INFO() << "memory_limit: " << cfg->memory_limit()
                    << "memory_soft_limit" << cfg->memory_soft_limit();

        ui->lineEdit_soft_limit->setText(QString("%1").arg(softLimit));
        ui->lineEdit_max_limit->setText(QString("%1").arg(memLimit));
    }
}

ErrorCode MemoryConfTab::getMemoryInfo(container::ResourceLimit *cfg)
{
    if (cfg)
    {
        auto softLimit = limitDataHandle(ui->lineEdit_soft_limit->text().toDouble(), ui->cb_soft_unit);
        KLOG_INFO() << "Memory soft limit: " << softLimit;

        auto maxLimit = limitDataHandle(ui->lineEdit_max_limit->text().toDouble(), ui->cb_max_unit);
        KLOG_INFO() << "Memory max limit: " << maxLimit;

        if (maxLimit < 0 || softLimit < 0)  //判断内存软限制和最大值是否溢出
        {
            KLOG_INFO() << "The soft memory or max memory is overload";
            return INPUT_OVERLIMIT_ERROR;
        }
        else if (softLimit > maxLimit)  // 判断内存软限制是否大于最大限制
        {
            KLOG_INFO() << "The soft momory is greater then the max memory";
            return INPUT_ARG_ERROR;
        }

        cfg->set_memory_limit(maxLimit);
        cfg->set_memory_soft_limit(softLimit);
        return NO_ERROR;
    }
    return CONFIG_ARG_ERROR;
}

int MemoryConfTab::limitDataHandle(double originData, QComboBox *unitWidget)
{
    QString unit = unitWidget->currentText();
    int limit = originData;
    if (unit == "GB")
    {
        limit = originData * 1024.0;
    }
    return limit;
}
