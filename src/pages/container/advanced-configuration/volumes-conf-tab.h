/**
 * @file          /ks-scmc-gui/src/pages/container/advanced-configuration/volumes-conf-tab.h
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef VOLUMESCONFTAB_H
#define VOLUMESCONFTAB_H

#include <QWidget>
#include "def.h"
#include "proto/container.grpc.pb.h"
#include "table-widgets/config-table.h"
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
    bool getVolumeInfo(container::ContainerConfigs *cfg, QString &errMSg);
    bool getVolumeInfo(container::UpdateRequest *req, QString &errMSg);
    void setVolumeInfo(const container::ContainerConfigs *cfg);

private:
    void initUI();

private:
    Ui::VolumesConfTab *ui;
    ConfigTable *m_configTable;
};

#endif  // VOLUMESCONFTAB_H
