#ifndef MEMORYCONFTAB_H
#define MEMORYCONFTAB_H

#include <QComboBox>
#include <QLineEdit>
#include <QWidget>
#include "def.h"
#include "info-worker.h"
namespace Ui
{
class MemoryConfTab;
}

class MemoryConfTab : public QWidget
{
    Q_OBJECT

public:
    explicit MemoryConfTab(QWidget *parent = nullptr);
    ~MemoryConfTab();
    void setMemoryInfo(container::HostConfig *cfg);
    ErrorCode getMemoryInfo(container::ResourceConfig *cfg);

private:
    //bites->M/GB,M/GB->bites
    //stom: Small to Max
    int limitDataHandle(bool stom, int originData, QComboBox *unitWidget);

private:
    Ui::MemoryConfTab *ui;
};

#endif  // MEMORYCONFTAB_H
