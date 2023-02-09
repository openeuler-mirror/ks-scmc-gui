/**
 * @file          /ks-scmc-gui/src/pages/outline/outline-cell.cpp
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "outline-cell.h"
#include <QPainter>
#include <QPen>
#include "main-window.h"
#include "ui_outline-cell.h"

OutlineCell::OutlineCell(OutlineCellType type, QWidget *parent) : QWidget(parent),
                                                                  ui(new Ui::OutlineCell)
{
    ui->setupUi(this);
    initUI();
    m_type = type;
}

OutlineCell::~OutlineCell()
{
    delete ui;
}

void OutlineCell::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked(m_type);
}

void OutlineCell::initUI()
{
    ui->frame->setStyleSheet("background: #2d2d2d;"
                             "border-radius: 4px;");

    ui->cell_name->setStyleSheet("QLabel{"
                                 "background-image:url(:/images/dot.svg);"
                                 "background-position:left;"
                                 "background-origin:content;"
                                 "background-repeat:none;"
                                 "}");

    ui->label_online->setMinimumSize(10, 10);
    QHBoxLayout *hlay_online = new QHBoxLayout;
    hlay_online->setContentsMargins(0, 0, 0, 0);
    OutlineStatusIcon *statusIconGreen = new OutlineStatusIcon("#2EB3FF", 1, 1, this);
    hlay_online->addWidget(statusIconGreen);
    ui->label_online->setLayout(hlay_online);

    ui->label_offline->setMinimumSize(10, 10);
    QHBoxLayout *hlay_offline = new QHBoxLayout;
    hlay_offline->setContentsMargins(0, 0, 0, 0);
    OutlineStatusIcon *statusIconRed = new OutlineStatusIcon("#FF3A3A", 1, 1, this);
    hlay_offline->addWidget(statusIconRed);
    ui->label_offline->setLayout(hlay_offline);

    ui->outline_pix->setMinimumSize(78, 78);
}

void OutlineCell::setIcon(const QString icon)
{
    ui->outline_pix->setStyleSheet(QString("QLabel{"
                                           "background-image:url(%1);"
                                           "background-position:center;"
                                           "background-repeat:no-repeat;"
                                           "}")
                                       .arg(icon));
}
