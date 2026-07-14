
/**
 * @file          src/common/refresh-button.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */

#include "refresh-button.h"

#include <QPainter>
#include <QTimer>

RefreshButton::RefreshButton(QWidget *parent) : QToolButton(parent),
                                                m_timer(nullptr)
{
    this->setFixedSize(16, 16);
    setIcon(QIcon(":/images/refresh.svg"));

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &RefreshButton::refresh);
    connect(this, &RefreshButton::clicked, this, &RefreshButton::startTimer);
}

void RefreshButton::enterEvent(QEvent *e)
{
    setIcon(QIcon(":/images/refresh-hover.svg"));
}

void RefreshButton::leaveEvent(QEvent *e)
{
    setIcon(QIcon(":/images/refresh.svg"));
}

void RefreshButton::refresh()
{
    static int count = 0;
    count++;
    QPixmap pix(":/images/refresh-hover.svg");
    static int rat = 0;
    rat = rat >= 180 ? 30 : rat + 30;
    int imageWidth = pix.width();
    int imageHeight = pix.height();
    QPixmap temp(pix.size());
    temp.fill(Qt::transparent);
    QPainter painter(&temp);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.translate(imageWidth / 2, imageHeight / 2);        // 让图片的中心作为旋转的中心
    painter.rotate(rat);                                       // 顺时针旋转90度
    painter.translate(-(imageWidth / 2), -(imageHeight / 2));  // 使原点复原
    painter.drawPixmap(0, 0, pix);
    painter.end();
    setIcon(QIcon(temp));

    if (count == 6)
    {
        m_timer->stop();
        setIcon(QIcon(":/images/refresh.svg"));
        count = 0;
    }
}

void RefreshButton::startTimer()
{
    m_timer->start(100);
}
