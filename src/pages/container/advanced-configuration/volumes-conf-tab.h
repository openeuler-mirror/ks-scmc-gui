#ifndef VOLUMESCONFTAB_H
#define VOLUMESCONFTAB_H

#include <QWidget>
#include "base/info-worker.h"
#include "common/config-table.h"
#include "def.h"
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
    ErrorCode getVolumeInfo(container::HostConfig *cfg);

private:
    void initUI();

private:
    Ui::VolumesConfTab *ui;
    ConfigTable *m_configTable;
};

#endif  // VOLUMESCONFTAB_H
