#include <kiran-log/qt5-log-i.h>
#include <QValidator>
#include "cpu-conf-tab.h"
#include "ui_cpu-conf-tab.h"
CPUConfTab::CPUConfTab(double totalCPU, QWidget* parent) : QWidget(parent),
                                                           ui(new Ui::CPUConfTab),
                                                           m_totalCPU(totalCPU)
{
    ui->setupUi(this);
    QValidator* intValidator = new QIntValidator(2, 262144, this);
    ui->lineEdit_sche_pri->setValidator(intValidator);

    QDoubleValidator* doubleValidator = new QDoubleValidator(0.01, totalCPU, 2, this);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEdit_cpu_core->setValidator(doubleValidator);
}

CPUConfTab::~CPUConfTab()
{
    delete ui;
}

void CPUConfTab::setCPUInfo(container::ResourceLimit* cfg)
{
    if (cfg)
    {
        KLOG_INFO() << cfg->cpu_limit() << cfg->cpu_prio();
        ui->lineEdit_cpu_core->setText(QString("%1").arg(cfg->cpu_limit()));
        ui->lineEdit_sche_pri->setText(QString("%1").arg(cfg->cpu_prio()));
    }
}

void CPUConfTab::getCPUInfo(container::ResourceLimit *cfg)
{
    if (cfg)
    {
        KLOG_INFO() << "cpu core:" << ui->lineEdit_cpu_core->text().toDouble();
        cfg->set_cpu_limit(ui->lineEdit_cpu_core->text().toDouble());

        //调度优先级
        cfg->set_cpu_prio(ui->lineEdit_sche_pri->text().toInt());
    }
}
