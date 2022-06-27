/**
 * @file          /ks-scmc-gui/src/pages/container/base-configuration/cpu-conf-tab.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef CPUCONFTAB_H
#define CPUCONFTAB_H

#include <QTimer>
#include <QWidget>
#include "def.h"
#include "info-worker.h"
namespace Ui
{
class CPUConfTab;
}

class CPUConfTab : public QWidget
{
    Q_OBJECT

public:
    explicit CPUConfTab(QWidget *parent = nullptr);
    ~CPUConfTab();
    void setTotalCPU(double totalCPU);
    void setCPUInfo(container::ResourceLimit *cfg);
    void getCPUInfo(container::ResourceLimit *cfg);

private:
    Ui::CPUConfTab *ui;
    double m_totalCPU;
    QTimer *m_timer;
};

#endif  // CPUCONFTAB_H
