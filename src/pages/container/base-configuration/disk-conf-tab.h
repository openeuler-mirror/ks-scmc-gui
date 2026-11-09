/**
 * @file          src/pages/container/base-configuration/disk-conf-tab.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#ifndef DISKCONFTAB_H
#define DISKCONFTAB_H

#include <QWidget>
#include "proto/container.grpc.pb.h"

namespace Ui
{
class DiskConfTab;
}

class DiskConfTab : public QWidget
{
    Q_OBJECT

public:
    explicit DiskConfTab(QWidget* parent = nullptr);
    ~DiskConfTab();

    void setSpeed(container::ResourceLimit* cfg);
    void getSpeed(container::ResourceLimit* cfg);

private:
    Ui::DiskConfTab* ui;
};

#endif  // DISKCONFTAB_H
