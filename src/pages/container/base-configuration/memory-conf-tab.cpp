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
    QRegExp regExp("[0-9]+.?[0-9]*");
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
        int memLimit = limitDataHandle(true, cfg->memory_limit(), ui->cb_max_unit);
        int softLimit = limitDataHandle(true, cfg->memory_soft_limit(), ui->cb_soft_unit);
        KLOG_INFO() << cfg->memory_limit() << cfg->memory_limit();

        ui->lineEdit_soft_limit->setText(QString("%1").arg(softLimit));
        ui->lineEdit_max_limit->setText(QString("%1").arg(memLimit));
    }
}

ErrorCode MemoryConfTab::getMemoryInfo(container::ResourceLimit *cfg)
{
    if (cfg)
    {
        auto softLimit = limitDataHandle(false, ui->lineEdit_soft_limit->text().toDouble(), ui->cb_soft_unit);
        KLOG_INFO() << "Memory soft limit: " << softLimit;

        auto maxLimit = limitDataHandle(false, ui->lineEdit_max_limit->text().toDouble(), ui->cb_max_unit);
        KLOG_INFO() << "Memory max limit: " << maxLimit;

        if (softLimit <= maxLimit)
        {
            cfg->set_memory_limit(maxLimit);
            cfg->set_memory_soft_limit(softLimit);
            return NO_ERROR;
        }
        else
            return INPUT_ARG_ERROR;
    }
    return CONFIG_ARG_ERROR;
}

int MemoryConfTab::limitDataHandle(bool stom, double originData, QComboBox *unitWidget)
{
    QString unit = unitWidget->currentText();
    int limit = originData;
    if (stom)  // MB->GB
    {
        if (unit == "GB")
            limit = originData / 1024.0;
    }
    else  // GB->MB
    {
        if (unit == "GB")
            limit = originData * 1024.0;
    }
    return limit;
}
