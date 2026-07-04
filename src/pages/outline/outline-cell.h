/**
 * @file          /ks-scmc-gui/src/pages/outline/outline-cell.h
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef OUTLINECELL_H
#define OUTLINECELL_H

#include <QWidget>
#include <QVBoxLayout>
#include "outline-status-icon.h"
//#include "outline-view.h"

namespace Ui {
class OutlineCell;
}

enum OutlineCellType
{
    ONUTLINE_CELL_NODE,
    ONUTLINE_CELL_CONTAINER,
    ONUTLINE_CELL_IMAGE,
    ONUTLINE_CELL_TEMPLATE_CONTAINER,
    ONUTLINE_CELL_EXAMINE,
    ONUTLINE_CELL_NODE_WARNING,
    ONUTLINE
};

class OutlineStatusIcon;

class OutlineCell : public QWidget
{
    Q_OBJECT

public:
    explicit OutlineCell(QWidget *parent = nullptr);
    ~OutlineCell() override;
    void setOutlineCellName(const QString name); //设置概述模块名字
    void setIcon(); // 设置左上角图片
    bool is_mouse_Press; // 鼠标是否按下
    Ui::OutlineCell *ui;

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void outlineCellStep(OutlineCellType type); //鼠标被点击跳转信号
    void isPress(); //鼠标按下信号

private:
    OutlineStatusIcon *m_status_icon_green;
    OutlineStatusIcon *m_status_icon_red;

};

#endif // OUTLINECELL_H
