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
    void setVolumeInfo(const container::Mount *mounts);

private:
    void initUI();

private:
    Ui::VolumesConfTab *ui;
    ConfigTable *m_configTable;
};

#endif  // VOLUMESCONFTAB_H
