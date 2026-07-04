/**
 * @file          /ks-scmc-gui/src/pages/container/advanced-configuration/graphic-conf-tab.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef GRAPHICCONFTAB_H
#define GRAPHICCONFTAB_H

#include <QWidget>
#include "info-worker.h"
namespace Ui
{
class GraphicConfTab;
}

class GraphicConfTab : public QWidget
{
    Q_OBJECT

public:
    explicit GraphicConfTab(QWidget *parent = nullptr);
    ~GraphicConfTab();
    void setGraphicInfo(const container::ContainerConfigs *cfg);
    void getGraphicInfo(container::CreateRequest *request);
    bool isGraphic();

private:
    Ui::GraphicConfTab *ui;
};

#endif  // GRAPHICCONFTAB_H
