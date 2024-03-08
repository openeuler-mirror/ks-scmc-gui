#ifndef CMDCONFTAB_H
#define CMDCONFTAB_H

#include <QWidget>
#include "proto/container.grpc.pb.h"
#include "../container-setting.h"

namespace Ui
{
class CmdConfTab;
}

class CmdConfTab : public QWidget
{
    Q_OBJECT

public:
    explicit CmdConfTab(ContainerSettingType type,QWidget *parent = nullptr);
    ~CmdConfTab();
    void getCMDInfo(container::ContainerConfigs *cfg);
    void getCMDInfo(container::UpdateRequest *req);
    void setCMDInfo(container::ContainerConfigs *cfg);

private:
    void initUI();

private:
    Ui::CmdConfTab *ui;
    ContainerSettingType m_type;
};

#endif  // CMDCONFTAB_H
