#include <QPainter>
#include <QStyleOption>
#include "bubble-tip-button.h"

BubbleTipButton::BubbleTipButton(QWidget *parent) : QPushButton(parent), m_tipMsg(0), m_bubbleWidth(0)
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
    m_bubbleWidth = fm.width(QString::number(num)) + 5;
}

void BubbleTipButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect rt = rect();
    QRect rt1 = QRect(rt.right() - m_bubbleWidth, rt.top(), m_bubbleWidth, 15);

    painter.setPen(Qt::red);
    painter.setBrush(QBrush(Qt::red));
    QFont font;
    font.setPixelSize(14);
    if (m_tipMsg > 0)
    {
        painter.drawEllipse(rt1);
        painter.setPen(Qt::white);
        painter.setFont(font);
        painter.drawText(rt1, Qt::AlignCenter, QString::number(m_tipMsg));
    }

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QPushButton::paintEvent(event);
}
