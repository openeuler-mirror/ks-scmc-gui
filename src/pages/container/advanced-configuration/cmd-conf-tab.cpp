/**
 * @file          src/pages/container/advanced-configuration/cmd-conf-tab.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */

#include "cmd-conf-tab.h"
#include <kiran-log/qt5-log-i.h>
#include "../container-setting.h"
#include "ui_cmd-conf-tab.h"

CmdConfTab::CmdConfTab(ContainerSettingType type, QWidget *parent) : QWidget(parent),
                                                                     ui(new Ui::CmdConfTab),
                                                                     m_type(type)
{
    ui->setupUi(this);
    initUI();
}

CmdConfTab::~CmdConfTab()
{
    delete ui;
}

void CmdConfTab::getCMDInfo(container::ContainerConfigs *cfg)
{
    if (cfg) {
        std::string cmd = ui->lineEdit_cmd->text().toStdString();
        KLOG_DEBUG() << "cmd: %s" << cmd.c_str();
        cfg->set_cmd(cmd);
    }
}

void CmdConfTab::getCMDInfo(container::UpdateRequest *req)
{
    if (req) {
        std::string cmd = ui->lineEdit_cmd->text().toStdString();
        KLOG_DEBUG() << "cmd: %s" << cmd.c_str();
        req->set_cmd(cmd);
    }
}


void CmdConfTab::setCMDInfo(container::ContainerConfigs *cfg)
{
    if (cfg) {
        ui->lineEdit_cmd->setText(QString::fromStdString(cfg->cmd().data()));
    }
}


void CmdConfTab::initUI()
{
    if (m_type == CONTAINER_SETTING_TYPE_CONTAINER_EDIT) {
        ui->lineEdit_cmd->setEnabled(false);
    }
    ui->lineEdit_cmd->setPlaceholderText(tr("example: /bin/bash"));
}
