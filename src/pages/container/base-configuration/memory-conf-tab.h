/**
 * @file          /ks-scmc-gui/src/pages/container/base-configuration/memory-conf-tab.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
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
    void setTotalMemory(double memory);
    void setMemoryInfo(container::ResourceLimit *cfg);
    bool getMemoryInfo(container::ResourceLimit *cfg, QString &errMsg);

private:
    //MB->GB,GB->MB
    //stom: Small to Max
    double limitDataHandle(double originData, QComboBox *unitWidget);

private:
    Ui::MemoryConfTab *ui;
    double m_totalMemory;
};

#endif  // MEMORYCONFTAB_H
