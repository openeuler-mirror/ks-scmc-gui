#ifndef GRAPHICCONFTAB_H
#define GRAPHICCONFTAB_H

#include <QWidget>
#include "base/info-worker.h"
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
    void setGraphicInfo();
    void getGraphicInfo(container::CreateRequest *request);

private:
    Ui::GraphicConfTab *ui;
};

#endif  // GRAPHICCONFTAB_H
