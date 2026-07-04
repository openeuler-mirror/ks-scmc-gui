/**
 * @file          /ks-scmc-gui/src/pages/outline/outline-cell.cpp
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "outline-cell.h"
#include "ui_outline-cell.h"
#include "main-window.h"
#include <QPainter>
#include <QPen>

OutlineCell::OutlineCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutlineCell)
{
    is_mouse_Press = false;
//    m_outline_view = new OutlineView(this);
    ui->setupUi(this);
    this->setIcon();
    ui->frame->setStyleSheet("background: #2d2d2d;"
                             "border-radius: 4px;");
}

OutlineCell::~OutlineCell()
{
    delete ui;
}

void OutlineCell::setOutlineCellName(const QString name)
{
    ui->label->setText(name);
}

void OutlineCell::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    is_mouse_Press = true;
//    emit outlineCellStep(ONUTLINE);
    emit isPress();
}

void OutlineCell::setIcon()
{
    ui->cell_name->setStyleSheet("QLabel{"
                               "background-image:url(:/images/dot.svg);"
                               "background-position:left;"
                               "background-origin:content;"
                               "background-repeat:none;"
                               "}");
    ui->label_online->setMinimumSize(10,10);
    QHBoxLayout * hlay_online = new QHBoxLayout;
    hlay_online->setContentsMargins(0,0,0,0);
    m_status_icon_green = new OutlineStatusIcon("blue",1,1);
    hlay_online->addWidget(m_status_icon_green);
    ui->label_online->setLayout(hlay_online);

    ui->label_offline->setMinimumSize(10,10);
    QHBoxLayout * hlay_offline = new QHBoxLayout;
    hlay_offline->setContentsMargins(0,0,0,0);
    m_status_icon_red = new OutlineStatusIcon("red",1,1);
    hlay_offline->addWidget(m_status_icon_red);
    ui->label_offline->setLayout(hlay_offline);
}
