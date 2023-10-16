#include "graphic-conf-tab.h"
#include "ui_graphic-conf-tab.h"

GraphicConfTab::GraphicConfTab(QWidget* parent) : QWidget(parent),
                                                  ui(new Ui::GraphicConfTab)
{
    ui->setupUi(this);
}

GraphicConfTab::~GraphicConfTab()
{
    delete ui;
}

void GraphicConfTab::setGraphicInfo(const container::ContainerConfigs* cfg)
{
    bool enableGraphic = cfg->enable_graphic();
    ui->graphic_checkBox->setChecked(enableGraphic);
}

bool GraphicConfTab::isGraphic()
{
    return ui->graphic_checkBox->isChecked();
}
