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

void CPUConfTab::setCPUInfo(container::HostConfig* cfg)
{
    if (cfg)
    {
        auto resourceCfg = cfg->mutable_resource_config();
        KLOG_INFO() << resourceCfg->nano_cpus() / 1e9 << resourceCfg->cpu_shares();
        ui->lineEdit_cpu_core->setText(QString("%1").arg(resourceCfg->nano_cpus() / 1e9));
        ui->lineEdit_sche_pri->setText(QString("%1").arg(resourceCfg->cpu_shares()));
    }
}

void CPUConfTab::getCPUInfo(container::ResourceConfig* cfg)
{
    if (cfg)
    {
        KLOG_INFO() << "cpu core:" << ui->lineEdit_cpu_core->text().toInt() * 1e9;
        cfg->set_nano_cpus(ui->lineEdit_cpu_core->text().toInt() * 1e9);

        //调度优先级
        cfg->set_cpu_shares(ui->lineEdit_sche_pri->text().toInt());
    }
}