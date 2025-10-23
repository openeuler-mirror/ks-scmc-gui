/**
 * @file          /ks-scmc-gui/src/pages/container/base-configuration/cpu-conf-tab.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "cpu-conf-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QValidator>
#include "ui_cpu-conf-tab.h"

#define TIMEOUT 500
CPUConfTab::CPUConfTab(QWidget* parent) : QWidget(parent),
                                          ui(new Ui::CPUConfTab)

{
    ui->setupUi(this);
    ui->label_tips->hide();
    ui->label_tips->setStyleSheet("color:#F56C6C;font-size:12px;");

    QRegExp regExp("[0-9]*\\.?([0-9]{2})");
    ui->lineEdit_cpu_core->setValidator(new QRegExpValidator(regExp, this));
    ui->lineEdit_cpu_core->setTextMargins(10, 0, 0, 0);
    ui->lineEdit_cpu_core->setMaxLength(7);

    ui->cb_sche_pri->addItem(tr("Low"), QVariant(1));
    ui->cb_sche_pri->addItem(tr("Medium"), QVariant(3));
    ui->cb_sche_pri->addItem(tr("High"), QVariant(5));

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout,
            [this]
            {
                double num = ui->lineEdit_cpu_core->text().toDouble();
                ui->label_tips->setVisible(num > m_totalCPU ? true : false);
                m_timer->stop();
            });
    connect(ui->lineEdit_cpu_core, &QLineEdit::textEdited,
            [this](QString str)
            {
                if (!str.isEmpty())
                    m_timer->start(TIMEOUT);
                else
                    ui->label_tips->hide();
            });
}

CPUConfTab::~CPUConfTab()
{
    delete ui;
    if (m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }
}

void CPUConfTab::setTotalCPU(double totalCPU)
{
    m_totalCPU = totalCPU;
    ui->lineEdit_cpu_core->setPlaceholderText(tr("less than or equal to %1, default:system cores %1").arg(QString::number(totalCPU)));
}

void CPUConfTab::setCPUInfo(container::ResourceLimit* cfg)
{
    if (cfg)
    {
        ui->lineEdit_cpu_core->setText(QString("%1").arg(cfg->cpu_limit()));

        int priority = cfg->cpu_prio();
        ui->cb_sche_pri->setCurrentIndex(ui->cb_sche_pri->findData(QVariant(priority)));
    }
}

bool CPUConfTab::getCPUInfo(container::ResourceLimit* cfg, QString& errMsg)
{
    if (cfg)
    {
        double core;
        // 如果没有输入ｃｐｕ核心数，则默认为总核心数
        if (!ui->lineEdit_cpu_core->text().isEmpty())
            core = ui->lineEdit_cpu_core->text().toDouble();
        else
        {
            core = m_totalCPU;
        }

        if (core > m_totalCPU)
        {
            errMsg = tr("CPU core can't be greater than the node cpu core limit!");
            return false;
        }
        else if (core <= 0)
        {
            errMsg = tr("CPU cores must be greater than 0!");
            return false;
        }

        cfg->set_cpu_limit(core);
        // 调度优先级
        cfg->set_cpu_prio(ui->cb_sche_pri->itemData(ui->cb_sche_pri->currentIndex()).toInt());
        return true;
    }
    else
    {
        errMsg = tr("The container resource limit arg is error.");
        return false;
    }
}
