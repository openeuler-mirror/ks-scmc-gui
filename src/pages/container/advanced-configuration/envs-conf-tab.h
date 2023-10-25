/**
 * @file          /ks-scmc-gui/src/pages/container/advanced-configuration/envs-conf-tab.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef ENVSCONFTAB_H
#define ENVSCONFTAB_H

#include <QWidget>
#include "common/config-table.h"
#include "def.h"
#include "info-worker.h"
namespace Ui
{
class EnvsConfTab;
}

class EnvsConfTab : public QWidget
{
    Q_OBJECT

public:
    explicit EnvsConfTab(QWidget *parent = nullptr);
    ~EnvsConfTab();
    ErrorCode getEnvInfo(container::ContainerConfigs *cntrCfg);
    void setEnvInfo(const container::ContainerConfigs *cfg);

private:
    void initUI();

private:
    Ui::EnvsConfTab *ui;
    ConfigTable *m_configTable;
};

#endif  // ENVSCONFTAB_H
