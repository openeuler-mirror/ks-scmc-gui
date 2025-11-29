/**
 * @file          /ks-scmc-gui/src/pages/container/advanced-configuration/high-availability-tab.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "high-availability-tab.h"
#include <kiran-log/qt5-log-i.h>
#include "ui_high-availability-tab.h"

#define APP_HIGH_AVAILABILITY_ON QObject::tr("On")
#define APP_HIGH_AVAILABILITY_OFF QObject::tr("Off")

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
        KLOG_DEBUG() << "policy name: " << cfg->name().data()
                     << "policy max retry:" << cfg->max_retry();

        auto index = ui->cb_high_avail_policy->findData(cfg->name().data());
        ui->cb_high_avail_policy->setCurrentIndex(index);
        if (0 == QString::compare(cfg->name().data(), "on-failure"))
            setLineEditVisible(true);
        else
            setLineEditVisible(false);

        ui->lineEdit_times->setText(QString("%1").arg(cfg->max_retry()));
    }
}

void HighAvailabilityTab::getRestartPolicy(container::RestartPolicy *cfg)
{
    if (cfg)
    {
        KLOG_DEBUG() << "Policy :" << ui->cb_high_avail_policy->currentText() << "times: " << ui->lineEdit_times->text().toInt();
        QString policy = ui->cb_high_avail_policy->itemData(ui->cb_high_avail_policy->currentIndex()).toString();
        cfg->set_name(policy.toStdString());
        if (ui->lineEdit_times->isVisible())
        {
            cfg->set_max_retry(ui->lineEdit_times->text().toInt());
        }
    }
}

void HighAvailabilityTab::getAppHighAvailPolicy(container::ContainerMiscConfig *cfg)
{
    if (!cfg)
        return;

    KLOG_DEBUG() << "Set app high avail policy to backend: " << ui->cb_app_avail_policy->currentText();
    if (ui->cb_app_avail_policy->currentText() == APP_HIGH_AVAILABILITY_ON)
    {
        cfg->set_app_run(true);
    }
    else
    {
        cfg->set_app_run(false);
    }
}

void HighAvailabilityTab::setAppHighAvailPolicy(container::ContainerMiscConfig *cfg)
{
    if (!cfg)
        return;

    KLOG_DEBUG() << "Get app high avail policy from backend: " << cfg->app_run();
    if (cfg->app_run())
    {
        ui->cb_app_avail_policy->setCurrentText(APP_HIGH_AVAILABILITY_ON);
    }
    else
    {
        ui->cb_app_avail_policy->setCurrentText(APP_HIGH_AVAILABILITY_OFF);
    }
}

void HighAvailabilityTab::onCbActivated(QString text)
{
    if (ui->cb_high_avail_policy->currentData() == "on-failure")
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

    // 后面加空格，是为了防止在3.3-6Nari系统中下拉列表项显示不全
    // TODO:QListView item计算中文长度有误，后续完善
    ui->cb_high_avail_policy->addItem(tr("no    "), "no");
    ui->cb_high_avail_policy->addItem(tr("always    "), "always");
    ui->cb_high_avail_policy->addItem(tr("on-failure    "), "on-failure");
    ui->cb_high_avail_policy->addItem(tr("unless-stopped    "), "unless-stopped");

    ui->lineEdit_times->setValidator(new QIntValidator(0, 65535, this));
    ui->lineEdit_times->setPlaceholderText("0-65535");

    ui->cb_app_avail_policy->addItem(APP_HIGH_AVAILABILITY_ON);
    ui->cb_app_avail_policy->addItem(APP_HIGH_AVAILABILITY_OFF);
    ui->cb_app_avail_policy->setCurrentText(APP_HIGH_AVAILABILITY_OFF);

    //隐藏应用高可用设置
    ui->label_2->hide();
    ui->cb_app_avail_policy->hide();
    ui->label_2->setEnabled(false);    
    ui->cb_app_avail_policy->setEnabled(false);

    connect(ui->cb_high_avail_policy, QOverload<const QString &>::of(&QComboBox::activated), this, &HighAvailabilityTab::onCbActivated);
}
