/**
 * @file          /ks-scmc-gui/src/pages/outline/outline-cell.h
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef OUTLINECELL_H
#define OUTLINECELL_H

#include <QVBoxLayout>
#include <QWidget>
#include "outline-status-icon.h"
//#include "outline-view.h"

namespace Ui
{
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
    explicit OutlineCell(OutlineCellType type, QWidget *parent = nullptr);
    ~OutlineCell() override;
    void setIcon(const QString icon);  // 设置左侧图片
    Ui::OutlineCell *ui;

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void initUI();

signals:
    void clicked(OutlineCellType type);

private:
    OutlineCellType m_type;
};

#endif  // OUTLINECELL_H
