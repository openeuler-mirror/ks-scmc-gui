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
    void updateWarningSums();
    QString getApproveSums();
    QString getWarningSums();

protected:
    //    bool eventFilter(QObject *watched, QEvent *event);
    //    virtual void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void initUI();
    void initConnect();
    QWidget *getScrollCenterWidget();  //获取widget
    void setOutlineCellNode();         //初始化概述页面的cell
    void setOutlineCellContainer();
    void setOutlineCellImage();
    void setOutlineCellTemplateContainer();
    void setOutlineCellExamine();
    void setOutlineCellWarning();

private slots:
    void getOutlineCellNodeNums(const QString objId, const QPair<grpc::Status, node::ListReply> &reply);
    void getOutlineCellContainerNums(const QString objId, const QPair<grpc::Status, container::ListReply> &reply);
    void getOutlineCellImageNums(const QString objId, const QPair<grpc::Status, image::ListDBReply> &reply);
    void getOutlineCellTemplateContainerNums(const QString objId, const QPair<grpc::Status, container::ListTemplateReply> &reply);
    void getOutlineCellExamineNums(const QString objId, const QPair<grpc::Status, image::ListDBReply> &reply);  //待审核
    void getOutlineCellWarningNums(const QString objId, const QPair<grpc::Status, node::ListReply> &);

    void emitOutlineCellType();
    void getlNodeList();
    void getContainerList();
    void getImageList();
    void getlTemplateContainer();
    void getWarnNums();

signals:
    void outlineCellStepPages(OutlineCellType type);
    void sigWarnSumNums(int size);   // 传出告警总数

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
};

#endif  // OUTLINEVIEW_H
