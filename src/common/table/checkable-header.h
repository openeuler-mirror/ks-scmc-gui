/**
 * @file          src/common/table/checkable-header.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#ifndef CHECKABLEHEADER_H
#define CHECKABLEHEADER_H

#include <QCheckBox>
#include <QHeaderView>

class CheckableHeader : public QHeaderView
{
    Q_OBJECT

public:
    explicit CheckableHeader(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void paintSection(QPainter *painter,
                      const QRect &rect,
                      int logicalIndex) const override;

signals:
    void toggled(Qt::CheckState checkState);
    void checkStateChanged(Qt::CheckState checkState);

public slots:
    void setCheckState(Qt::CheckState checkState);

private:
    bool m_stateChanged;
    Qt::CheckState m_checkState;
    QRect *m_rect;
};

#endif  // CHECKABLEHEADER_H
