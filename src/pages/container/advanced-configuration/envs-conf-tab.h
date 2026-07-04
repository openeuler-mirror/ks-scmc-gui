#ifndef ENVSCONFTAB_H
#define ENVSCONFTAB_H

#include <QWidget>
#include "base/info-worker.h"
#include "common/config-table.h"
#include "def.h"
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
    ErrorCode getEnvInfo(container::ContainerConfig *cfg);

private:
    void initUI();

private:
    Ui::EnvsConfTab *ui;
    ConfigTable *m_configTable;
};

#endif  // ENVSCONFTAB_H
