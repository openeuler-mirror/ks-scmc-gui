/**
 * @file          /ks-scmc-gui/src/pages/outline/outline-view.h
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef OUTLINEVIEW_H
#define OUTLINEVIEW_H

#include <QFrame>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QWidget>
#include "info-worker.h"
#include "outline-cell.h"
#include "outline-introduction.h"
#include "page.h"

class OutlineCell;
class OutlineIntroduction;
class OutlineView : public Page
{
    Q_OBJECT
public:
    explicit OutlineView(QWidget *parent = nullptr);
    ~OutlineView() override;
    void updateInfo(QString keyword = "");

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    void initUI();
    void initConnect();
    void setOutlineCellNode();  //初始化概述页面的cell
    void setOutlineCellContainer();
    void setOutlineCellImage();
    void setOutlineCellTemplateContainer();
    void setOutlineCellExamine();
    void setOutlineCellWarning();

private slots:
    void getDashboardResult(const QString objId, const QPair<grpc::Status, sys::DashboardReply> &reply);

signals:
    void outlineCellStepPages(OutlineCellType type);
    void sigApproveSumNums(int size);   // 传出待审核总数
    void sigWarnSumNums(int64_t size);  // 传出告警总数

private:
    QString m_objId;
    OutlineCell *m_outlineCell_node;
    OutlineCell *m_outlineCell_container;
    OutlineCell *m_outlineCell_image;
    OutlineCell *m_outlineCell_template_container;
    OutlineCell *m_outlineCell_examine;
    OutlineCell *m_outlineCell_warning;
    OutlineIntroduction *m_outlineIntroduction;
    QScrollArea *m_scrollArea;  //滚动区域
    QWidget *m_scrollWidget;    //滚动区域的窗口
    QHBoxLayout *m_layout;      //水平布局滚动区域
    QTimer *m_timer;
};

#endif  // OUTLINEVIEW_H
