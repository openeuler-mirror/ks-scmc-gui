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

void GraphicConfTab::getGraphicInfo(container::CreateRequest* request)
{
    request->set_enable_graphic(ui->graphic_checkBox->isChecked());
}
