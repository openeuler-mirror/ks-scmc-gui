/**
 * @file          /ks-scmc-gui/src/error-message-box.cpp
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "error-message-box.h"

#include <kiran-log/qt5-log-i.h>

#define BORDER_RADIUS 15

#pragma execution_character_set("utf-8")

const int begin_width_spacing = 20;
const int begin_height_spacing = 10;
const int text_width_spacing = 12;
const int text_height_spacing = 10;
const int triangle_width = 6;
const int triangle_height = 10;
const int triangle_height_spacing = 10;

ErrorMessageBox::ErrorMessageBox(QWidget *parent) : QWidget(parent)
{
    this->setMouseTracking(true);
    this->setAttribute((Qt::WA_TranslucentBackground));
    this->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
    this->resize(350, 500);

    QFont labelFont;
    labelFont.setPixelSize(12);
    labelFont.setFamily("NotoSansCJKsc-Regular");
    this->setFont(labelFont);
}

ErrorMessageBox::~ErrorMessageBox()
{
}

void ErrorMessageBox::setLabelText(QString str)
{
    label_str = str;
}

double ErrorMessageBox::getLabelTextLength()
{
    QFontMetrics metrics = QFontMetrics(this->font());
    return metrics.width(label_str);
}

void ErrorMessageBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);  //消锯齿
    painter.setFont(this->font());

    this->calcTextSize();
    QColor border_color = QColor(195, 195, 195);
    QColor background_color = QColor(255, 255, 255);

    QRect bubbleRect = QRect(begin_width_spacing, begin_height_spacing, triangle_width + text_max_width, text_height_spacing + text_height + text_height_spacing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(border_color));
    painter.drawRoundedRect(bubbleRect.x() + triangle_width, bubbleRect.y(), bubbleRect.width() - triangle_width, bubbleRect.height(), 8, 8);

    //框,重叠后取框内
    painter.setBrush(QBrush(background_color));
    painter.drawRoundedRect(bubbleRect.x() + triangle_width + 1, bubbleRect.y() + 1, bubbleRect.width() - triangle_width - 2, bubbleRect.height() - 2, 8, 8);

    //三角，重叠后取框内
    QPointF points[3] = {
        QPointF(bubbleRect.x(), bubbleRect.y() + triangle_height_spacing + triangle_height / 2),
        QPointF(bubbleRect.x() + triangle_width + 1, bubbleRect.y() + triangle_height_spacing),
        QPointF(bubbleRect.x() + triangle_width + 1, bubbleRect.y() + triangle_height_spacing + triangle_height),
    };
    painter.setPen(QPen(background_color));
    painter.drawPolygon(points, 3);

    //画三角的两个边
    painter.setPen(QPen(border_color));
    painter.drawLine(QPointF(bubbleRect.x(), bubbleRect.y() + triangle_height_spacing + triangle_height / 2), QPointF(bubbleRect.x() + triangle_width, bubbleRect.y() + triangle_height_spacing));
    painter.drawLine(QPointF(bubbleRect.x(), bubbleRect.y() + triangle_height_spacing + triangle_height / 2), QPointF(bubbleRect.x() + triangle_width, bubbleRect.y() + triangle_height_spacing + triangle_height));
    //text
    QPen penText;
    penText.setColor(QColor(0, 0, 0));
    painter.setPen(penText);
    QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    painter.drawText(QRect(bubbleRect.x() + triangle_width + text_width_spacing, bubbleRect.y() + text_height_spacing, text_max_width, text_height), label_str, option);
}

void ErrorMessageBox::calcTextSize()
{
    QFontMetrics metrics = QFontMetrics(this->font());

    text_min_width = metrics.width("A") * 2;

    qint32 min_width = begin_width_spacing + triangle_width + begin_width_spacing;
    if (350 < (min_width + text_min_width))
    {
        this->setMinimumWidth(min_width + text_min_width);
    }

    text_max_width = 350 - min_width;  //框架的最大文字宽度
    qint32 real_width = metrics.width(label_str);
    if (real_width < text_max_width)
    {
        text_max_width = real_width;
        text_height = text_height_spacing + metrics.height() + text_height_spacing > triangle_height_spacing + triangle_height + triangle_height_spacing ? metrics.height() : triangle_height_spacing + triangle_height + triangle_height_spacing;
    }
    else
    {
        KLOG_DEBUG() << "2222" << real_width;
        QRect textRect = QRect(0, 0, text_max_width, 0);
        int flags = Qt::TextWordWrap;  // 自动换行
        textRect = metrics.boundingRect(textRect, flags, label_str);
        text_height = textRect.height();
    }
}

void ErrorMessageBox::leaveEvent(QEvent *event)
{
    this->hide();
    event->accept();
}
