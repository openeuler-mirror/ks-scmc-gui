/**
 * @file          /ks-scmc-gui/src/pages/container/base-configuration/memory-conf-tab.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "memory-conf-tab.h"
#include <kiran-log/qt5-log-i.h>
#include "ui_memory-conf-tab.h"

#define MIN_SOFT_LIMIT 6
MemoryConfTab::MemoryConfTab(QWidget *parent) : QWidget(parent),
                                                ui(new Ui::MemoryConfTab),
                                                m_totalMemory(0)
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
    ui->lineEdit_max_limit->setMaxLength(7);
    ui->lineEdit_soft_limit->setMaxLength(7);

    QRegExp regExp("[0-9]*\\.?([0-9]{2})");
    ui->lineEdit_soft_limit->setValidator(new QRegExpValidator(regExp, this));
    ui->lineEdit_max_limit->setValidator(new QRegExpValidator(regExp, this));
}

MemoryConfTab::~MemoryConfTab()
{
    delete ui;
}

void MemoryConfTab::setTotalMemory(double memory)
{
    m_totalMemory = memory;
    ui->lineEdit_max_limit->setPlaceholderText(tr("6MB-%1MB").arg(QString::number(m_totalMemory)));
    ui->lineEdit_soft_limit->setPlaceholderText(tr("6MB-%1MB").arg(QString::number(m_totalMemory)));
    ui->lineEdit_soft_limit->setText(QString::number(memory));
    ui->lineEdit_max_limit->setText(QString::number(memory));
}

void MemoryConfTab::setMemoryInfo(container::ResourceLimit *cfg)
{
    if (cfg)
    {
        auto memLimit = cfg->memory_limit();
        auto softLimit = cfg->memory_soft_limit();
        KLOG_INFO() << "memory_limit: " << cfg->memory_limit()
                    << "memory_soft_limit" << cfg->memory_soft_limit();

        ui->cb_max_unit->setCurrentText("MB");
        ui->cb_soft_unit->setCurrentText("MB");

        ui->lineEdit_soft_limit->setText(QString("%1").arg(softLimit));
        ui->lineEdit_max_limit->setText(QString("%1").arg(memLimit));
    }
}

bool MemoryConfTab::getMemoryInfo(container::ResourceLimit *cfg, QString &errMsg)
{
    if (cfg)
    {
        auto softLimit = limitDataHandle(ui->lineEdit_soft_limit->text().toDouble(), ui->cb_soft_unit);
        KLOG_INFO() << "Memory soft limit: " << softLimit;

        auto maxLimit = limitDataHandle(ui->lineEdit_max_limit->text().toDouble(), ui->cb_max_unit);
        KLOG_INFO() << "Memory max limit: " << maxLimit;

        if (maxLimit < 0 || softLimit < 0)  //判断内存软限制和最大值是否溢出
        {
            errMsg = tr("The memory soft limit or max limit is more than %1 MB.").arg(m_totalMemory);
            return false;
        }
        else if (maxLimit > m_totalMemory || softLimit > m_totalMemory)
        {
            errMsg = tr("The memory soft limit or max limit is more than %1 MB.").arg(m_totalMemory);
            return false;
        }
        else if (softLimit < MIN_SOFT_LIMIT)
        {
            errMsg = tr("The memory soft limit is less then %1 MB.").arg(MIN_SOFT_LIMIT);
            return false;
        }
        else if (softLimit > maxLimit)  // 判断内存软限制是否大于最大限制
        {
            errMsg = tr("The momory soft limit is greater then the memory max limit.");
            return false;
        }

        cfg->set_memory_limit(maxLimit);
        cfg->set_memory_soft_limit(softLimit);
        return true;
    }
    else
    {
        errMsg = tr("The container resource limit config arg is error.");
        return false;
    }
}

double MemoryConfTab::limitDataHandle(double originData, QComboBox *unitWidget)
{
    QString unit = unitWidget->currentText();
    auto limit = originData;
    if (unit == "GB")
    {
        limit = originData * 1024.0;
    }
    return limit;
}
