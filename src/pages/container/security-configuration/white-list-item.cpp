#include "white-list-item.h"
#include "ui_white-list-item.h"

WhiteListItem::WhiteListItem(int index, QWidget *parent) : QWidget(parent),
                                                           ui(new Ui::WhiteListItem)
{
    ui->setupUi(this);
    ui->label_name->setText(tr("Process%1").arg(index));
    connect(ui->btn_add, &QToolButton::clicked,
            [this] {
                emit sigAdd();
            });
    connect(ui->btn_delete, &QToolButton::clicked,
            [this] {
                emit sigDelete();
            });
}

WhiteListItem::~WhiteListItem()
{
    delete ui;
}

void WhiteListItem::setDeleteBtnVisible(bool visible)
{
    ui->btn_delete->setVisible(visible);
}

void WhiteListItem::updateItemName(int index)
{
    ui->label_name->setText(tr("Process%1").arg(index));
}

void WhiteListItem::setProcessInfo()
{
}
