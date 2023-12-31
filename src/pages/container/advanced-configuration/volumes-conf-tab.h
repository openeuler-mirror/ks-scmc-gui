/**
 * @file          /ks-scmc-gui/src/pages/container/advanced-configuration/volumes-conf-tab.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef VOLUMESCONFTAB_H
#define VOLUMESCONFTAB_H

#include <QWidget>
#include "common/config-table.h"
#include "def.h"
#include "info-worker.h"
namespace Ui
{
class VolumesConfTab;
}

class VolumesConfTab : public QWidget
{
    Q_OBJECT

public:
    explicit VolumesConfTab(QWidget *parent = nullptr);
    ~VolumesConfTab();
    ErrorCode getVolumeInfo(container::ContainerConfigs *cfg);
    void setVolumeInfo(const container::ContainerConfigs *cfg);

private:
    void initUI();

private:
    Ui::VolumesConfTab *ui;
    ConfigTable *m_configTable;
};

#endif  // VOLUMESCONFTAB_H
