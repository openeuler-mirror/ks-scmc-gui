#ifndef OUTLINECELL_H
#define OUTLINECELL_H

#include <QWidget>
#include <QVBoxLayout>
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

public:
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void outlineCellStep(OutlineCellType type); //鼠标被点击跳转信号
    void isPress(); //鼠标按下信号

private:
//    OutlineView *m_outline_view;

};

#endif // OUTLINECELL_H
