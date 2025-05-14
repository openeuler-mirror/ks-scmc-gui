/**
 * @file          src/common/table/checkable-header.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "checkable-header.h"
#include <kiran-log/qt5-log-i.h>
#include <QMouseEvent>
#include <QPainter>

CheckableHeader::CheckableHeader(QWidget *parent) : QHeaderView(Qt::Horizontal, parent),
                                                    m_stateChanged(false),
                                                    m_checkState(Qt::Unchecked)
{
    m_rect = new QRect(16, 12, 16, 16);
    // 做下拉筛选功能时可能会用到这个属性，暂设置为false
    setSectionsClickable(false);
    setMouseTracking(true);
    // TODO: 使用setIndexWidget需要调整位置
    // setIndexWidget(indexAt(QPoint(0, 0)), new QCheckBox(this));
}

void CheckableHeader::paintSection(QPainter *painter,
                                   const QRect &rect,
                                   int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    if (logicalIndex == 0)
    {
        QPixmap pixmap;
        switch (m_checkState)
        {
        case Qt::CheckState::Checked:
            pixmap.load(":images/checked.svg");
            break;
        case Qt::CheckState::Unchecked:
            pixmap.load(":images/check-default.svg");
            break;
        case Qt::CheckState::PartiallyChecked:
            pixmap.load(":images/check-default.svg");
            break;
        default:
            break;
        }

        style()->drawItemPixmap(painter, *m_rect, Qt::AlignCenter, pixmap);
    }
}

void CheckableHeader::mousePressEvent(QMouseEvent *e)
{
    auto column = logicalIndexAt(e->pos());
    if (column == 0)
    {
        if ((e->pos().x() > m_rect->x()) && (e->pos().y() > m_rect->y()) && (e->pos().x() < m_rect->x() + 16) && (e->pos().y() < m_rect->y() + 16))
        {
            m_checkState = m_checkState == Qt::Unchecked ? Qt::Checked : Qt::Unchecked;
            emit toggled(m_checkState);
            emit checkStateChanged(m_checkState);
            viewport()->update();
        }
    }
    QHeaderView::mousePressEvent(e);
}

void CheckableHeader::setCheckState(Qt::CheckState checkState)
{
    m_stateChanged = true;
    m_checkState = checkState;

    emit checkStateChanged(checkState);
    viewport()->update();
}
