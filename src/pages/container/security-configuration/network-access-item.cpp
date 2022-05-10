#include "network-access-item.h"
#include "ui_network-access-item.h"

NetworkAccessItem::NetworkAccessItem(QWidget *parent) : QWidget(parent),
                                                        ui(new Ui::NetworkAccessItem)
{
    ui->setupUi(this);
    initUI();
}

NetworkAccessItem::~NetworkAccessItem()
{
    delete ui;
}

void NetworkAccessItem::setDeleteBtnVisible(bool visible)
{
    ui->btn_delete->setVisible(visible);
}

void NetworkAccessItem::setNetworkInfo(int index)
{
    ui->lineEdit_address->setText(QString::number(index));
}

void NetworkAccessItem::ondelete()
{
    emit sigDelete();
}

void NetworkAccessItem::onAdd()
{
    emit sigAdd();
}

void NetworkAccessItem::initUI()
{
    ui->lineEdit_address->setPlaceholderText("192.168.1.1/24");
    ui->lineEdit_port->setText(0);
    connect(ui->btn_add, &QToolButton::clicked, this, &NetworkAccessItem::onAdd);
    connect(ui->btn_delete, &QToolButton::clicked, this, &NetworkAccessItem::ondelete);
}
