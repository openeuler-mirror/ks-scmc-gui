#ifndef CPUCONFTAB_H
#define CPUCONFTAB_H

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
    explicit CPUConfTab(double totalCPU, QWidget *parent = nullptr);
    ~CPUConfTab();
    void setCPUInfo(container::ResourceLimit *cfg);
    void getCPUInfo(container::ResourceLimit *cfg);

private:
    Ui::CPUConfTab *ui;
    double m_totalCPU;
};

#endif  // CPUCONFTAB_H
