/**
 * @file          /ks-scmc-gui/src/common/bubble-tip-button.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "bubble-tip-button.h"
#include <kiran-log/qt5-log-i.h>
#include <QPainter>
#include <QStyleOption>

#define ICON_WIDTH 16
#define ICON_HEIGHT 16

BubbleTipButton::BubbleTipButton(QString icon, QWidget *parent) : QPushButton(parent), m_tipMsg(0), m_bubbleWidth(0), m_icon(icon)
{
}

BubbleTipButton::~BubbleTipButton()
{
}

void BubbleTipButton::setTipMsg(int num)
{
    KLOG_INFO() << "setTipMsg:" << num;
    m_tipMsg = num;
    QString text;
    QFont font;
    font.setPixelSize(12);
    QFontMetrics fm(font);
    if (m_tipMsg > 99)
        text = "99+";
    else
        text = QString::number(m_tipMsg);
    m_bubbleWidth = fm.width(text) + 10;
}

int BubbleTipButton::getTipMsgNum()
{
    return m_tipMsg;
}

void BubbleTipButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect rt = rect();
    QRect rt1 = QRect(rt.x() + ICON_WIDTH / 2, rt.y(), m_bubbleWidth, 16);         // draw bubble
    QRect rt2 = QRect(rt.x(), rt.y() + rt.height() / 2, ICON_WIDTH, ICON_HEIGHT);  //draw icon

    QPixmap iconPix(m_icon);
    painter.drawPixmap(rt2, iconPix);

    if (m_tipMsg > 0)
    {
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(Qt::red);
        painter.setBrush(QBrush(Qt::red));
        QFont font;
        font.setPixelSize(12);

        painter.drawEllipse(rt1);
        //painter.drawRect(rt1);
        painter.setPen(Qt::white);
        painter.setFont(font);
        if (m_tipMsg < 100)
            painter.drawText(rt1, Qt::AlignCenter, QString::number(m_tipMsg));
        else
            painter.drawText(rt1, Qt::AlignCenter, "99+");
    }

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
    QPushButton::paintEvent(event);
}
