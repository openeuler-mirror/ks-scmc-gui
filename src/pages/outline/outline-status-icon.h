/**
 * @file          /ks-scmc-gui/src/pages/outline/outline-status-icon.h
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef OUTLINESTATUSICON_H
#define OUTLINESTATUSICON_H

#include <QObject>
#include <QWidget>
#include <QPainter>

class OutlineStatusIcon : public QWidget
{
    Q_OBJECT
public:
    explicit OutlineStatusIcon(QString color = "",int x = 0,int y = 0,QWidget *parent = nullptr);
    ~OutlineStatusIcon() override;
private:
    QPainter *m_paint;
    QString m_color;
    int m_x,m_y;
    void paintEvent(QPaintEvent*) override;

public slots:
};

#endif // OUTLINESTATUSICON_H
