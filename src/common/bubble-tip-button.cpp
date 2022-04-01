#include "bubble-tip-button.h"
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
    m_tipMsg = num;
    QFont font;
    font.setPixelSize(14);
    QFontMetrics fm(font);
    m_bubbleWidth = fm.width(QString::number(num)) + 10;
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
        painter.setPen(Qt::red);
        painter.setBrush(QBrush(Qt::red));
        QFont font;
        font.setPixelSize(14);

        painter.drawEllipse(rt1);
        //painter.drawRect(rt1);
        painter.setPen(Qt::white);
        painter.setFont(font);
        painter.drawText(rt1, Qt::AlignCenter, QString::number(m_tipMsg));
    }

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
    QPushButton::paintEvent(event);
}
