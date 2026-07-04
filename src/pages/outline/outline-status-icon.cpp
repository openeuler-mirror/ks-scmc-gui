/**
 * @file          /ks-scmc-gui/src/pages/outline/outline-status-icon.cpp
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "outline-status-icon.h"

OutlineStatusIcon::OutlineStatusIcon(QString color, int x, int y, QWidget *parent) : QWidget(parent), m_color(color), m_x(x), m_y(y)
{
    this->setMaximumSize(10, 10);
    this->setContentsMargins(0, 0, 0, 0);
    setGeometry(0, 0, 10, 10);
}

OutlineStatusIcon::~OutlineStatusIcon()
{
}

void OutlineStatusIcon::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();
    QColor color(m_color);
    painter.setPen(QPen(color));
    painter.setRenderHint(QPainter::Antialiasing);  //平滑直线
    painter.setBrush(QBrush(color));                //设置画刷形式
    painter.drawEllipse(m_x, m_y, 8, 8);
    painter.restore();
    QWidget::paintEvent(event);
}
