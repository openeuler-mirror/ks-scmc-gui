#ifndef CPUCONFTAB_H
#define CPUCONFTAB_H

#include <QWidget>
#include "base/info-worker.h"
#include "def.h"
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
    void setCPUInfo(container::HostConfig *cfg);
    void getCPUInfo(container::ResourceConfig *cfg);

private:
    Ui::CPUConfTab *ui;
    double m_totalCPU;
};

#endif  // CPUCONFTAB_H
