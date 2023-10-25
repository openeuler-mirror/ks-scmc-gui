/**
 * @file          /ks-scmc-gui/src/pages/outline/outline-status-icon.cpp
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "outline-status-icon.h"

OutlineStatusIcon::OutlineStatusIcon(QString color, int x, int y, QWidget *parent ) : QWidget(parent)
{
    m_color = color;
    m_x = x;
    m_y = y;
    this->setMaximumSize(10,10);
    this->setContentsMargins(0,0,0,0);
    setGeometry(0,0,10,10);
}

OutlineStatusIcon::~OutlineStatusIcon()
{

}

void OutlineStatusIcon::paintEvent(QPaintEvent*)
{
     m_paint=new QPainter;
     QColor *color = new QColor;

     if(m_color == "blue")
        color->setRgb(46,179,255);
     else
        color->setRgb(255,58,58);

     m_paint->begin(this);
//     QPen * pen = new QPen(m_color,0,Qt::DashLine);
//     m_paint->setPen(QPen(m_color,2,Qt::SolidLine,Qt::RoundCap,Qt::MiterJoin));//设置画笔形式
     m_paint->setPen(QPen(*color));
     m_paint->setRenderHint(QPainter::Antialiasing);//平滑直线
     m_paint->setBrush(QBrush(*color));//设置画刷形式
     m_paint->drawEllipse(m_x,m_y,8,8);
     m_paint->end();
}
