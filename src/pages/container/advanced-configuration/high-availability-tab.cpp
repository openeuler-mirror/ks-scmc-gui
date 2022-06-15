#include "high-availability-tab.h"
#include <kiran-log/qt5-log-i.h>
#include "ui_high-availability-tab.h"

HighAvailabilityTab::HighAvailabilityTab(QWidget *parent) : QWidget(parent),
                                                            ui(new Ui::HighAvailabilityTab)
{
    ui->setupUi(this);
    initUI();
}

HighAvailabilityTab::~HighAvailabilityTab()
{
    delete ui;
}

void HighAvailabilityTab::setRestartPolicy(container::RestartPolicy *cfg)
{
    if (cfg)
    {
        KLOG_INFO() << "policy name: " << cfg->name().data()
                    << "policy max retry:" << cfg->max_retry();

        auto index = ui->cb_high_avail_policy->findData(cfg->name().data());
        ui->cb_high_avail_policy->setCurrentIndex(index);
        ui->lineEdit_times->setText(QString("%1").arg(cfg->max_retry()));
        onCbActivated(ui->cb_high_avail_policy->currentText());
    }
}

void HighAvailabilityTab::getRestartPolicy(container::RestartPolicy *cfg)
{
    if (cfg)
    {
        KLOG_INFO() << "Policy :" << ui->cb_high_avail_policy->currentText() << "times: " << ui->lineEdit_times->text();
        QString policy = ui->cb_high_avail_policy->itemData(ui->cb_high_avail_policy->currentIndex()).toString();
        cfg->set_name(policy.toStdString());
        if (ui->lineEdit_times->isVisible())
            cfg->set_max_retry(ui->lineEdit_times->text().toInt());
    }
}

void HighAvailabilityTab::onCbActivated(QString text)
{
    if (text == tr("on-failure"))
        setLineEditVisible(true);
    else
        setLineEditVisible(false);
}

void HighAvailabilityTab::setLineEditVisible(bool visible)
{
    ui->lab_auto_pull_time->setVisible(visible);
    ui->lineEdit_times->setVisible(visible);
    ui->lab_time->setVisible(visible);
}

void HighAvailabilityTab::initUI()
{
    setLineEditVisible(false);
    ui->cb_high_avail_policy->addItem(tr("no"), "no");
    ui->cb_high_avail_policy->addItem(tr("always"), "always");
    ui->cb_high_avail_policy->addItem(tr("on-failure"), "on-failure");
    ui->cb_high_avail_policy->addItem(tr("unless-stopped"), "unless-stopped");

    ui->lineEdit_times->setValidator(new QIntValidator(0, 65535, this));
    ui->lineEdit_times->setPlaceholderText("0-65535");

    connect(ui->cb_high_avail_policy, QOverload<const QString &>::of(&QComboBox::activated), this, &HighAvailabilityTab::onCbActivated);
}
