#include "cpu-conf-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QValidator>
#include "ui_cpu-conf-tab.h"
CPUConfTab::CPUConfTab(double totalCPU, QWidget* parent) : QWidget(parent),
                                                           ui(new Ui::CPUConfTab),
                                                           m_totalCPU(totalCPU)
{
    ui->setupUi(this);

    QDoubleValidator* doubleValidator = new QDoubleValidator(0.01, totalCPU, 2, this);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEdit_cpu_core->setValidator(doubleValidator);
    ui->lineEdit_cpu_core->setTextMargins(10, 0, 0, 0);

    ui->cb_sche_pri->addItem(tr("Low"), QVariant(1));
    ui->cb_sche_pri->addItem(tr("Medium"), QVariant(5));
    ui->cb_sche_pri->addItem(tr("High"), QVariant(10));
}

CPUConfTab::~CPUConfTab()
{
    delete ui;
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

void CPUConfTab::getCPUInfo(container::ResourceLimit* cfg)
{
    if (cfg)
    {
        KLOG_INFO() << "cpu core:" << ui->lineEdit_cpu_core->text().toDouble();
        cfg->set_cpu_limit(ui->lineEdit_cpu_core->text().toDouble());

        //调度优先级
        cfg->set_cpu_prio(ui->cb_sche_pri->itemData(ui->cb_sche_pri->currentIndex()).toInt());
    }
}
