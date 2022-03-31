#include "transmission-item.h"
#include <QPainter>
#include <QStyleOption>
#include "ui_transmission-item.h"
TransmissionItem::TransmissionItem(QWidget *parent) : QWidget(parent),
                                                      ui(new Ui::TransmissionItem)
{
    ui->setupUi(this);
    ui->btn_close->setIcon(QIcon(""));
}

TransmissionItem::~TransmissionItem()
{
    delete ui;
}

void TransmissionItem::setName(QString name)
{
    ui->lab_name->setText(name);
}

void TransmissionItem::setVersion(QString version)
{
    ui->lab_version->setText(version);
}

void TransmissionItem::setStatus(QString status)
{
    ui->lab_status->setText(status);
}

void TransmissionItem::setRate(int rate)
{
    ui->lab_rate->setText(QString("%1%").arg(rate));
}

void TransmissionItem::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
