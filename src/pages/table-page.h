/**
 * @file          /ks-scmc-gui/src/pages/table-page.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef TABLEPAGE_H
#define TABLEPAGE_H

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QStandardItemModel>
#include <QToolButton>
#include <QWidget>
#include "common/button-delegate.h"
#include "def.h"
#include "page.h"
namespace Ui
{
class TablePage;
}

enum OperatorButtonType
{
    OPERATOR_BUTTON_TYPE_SINGLE,
    OPERATOR_BUTTON_TYPE_BATCH
};

class HeaderView;
class TablePage : public Page
{
    Q_OBJECT

public:
    explicit TablePage(QWidget *parent = nullptr, bool is_open = false);
    virtual ~TablePage();
    virtual void updateInfo(QString keyword = "") = 0;
    void clearTable();
    void setOperateAreaVisible(bool visible);
    void setOperateBtnsVisible(bool visible);
    void setOperateSearchVisible(bool visible);
    void addSingleOperationButton(QAbstractButton *);
    void addBatchOperationButtons(QList<QPushButton *>);
    void addSingleWidgetButton(QWidget *);
    void setOpBtnEnabled(OperatorButtonType type, bool enabled);
    void setTableColNum(int num);
    void setTableRowNum(int num);
    void setTableItem(int row, int col, QStandardItem *item);
    void setTableItems(int row, int col, QList<QStandardItem *> items);
    void setTableActions(int col, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>> btnInfo);
    void setTableSingleChoose(bool isSingleChoose);
    void setSortableCol(QList<int> cols);
    void setHeaderSections(QStringList names);
    void setHeaderCheckable(bool checkable);
    void setTableDefaultContent(QString text);
    void setSearchableCol(int col);
    void setTips(QString text);
    void clearText();
    int getTableRowCount();
    QStandardItem *getItem(int row, int col);
    QList<QMap<QString, QVariant>> getCheckedItemInfo(int col);
    void sleep(int sec);
    void clearCheckState();
    void setPaging(int totalPages = 0);  // 设置分页
    void updatePaging(int page_no = 1);
    //    QString getSearchKey();
    void setSeachPageNone();  // 搜索结果为空
    void hideSearchLine();    // 隐藏搜索框

private:
    void initUI();
    void initPaging();
    void adjustTableSize();
    int getCheckedItemNum();
    QString tooptipWordWrap(const QString &org);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    virtual void timerEvent(QTimerEvent *event);

signals:
    void sigApp(int row);
    void sigAppRun(int row);
    void sigAppStop(int row);
    void sigMonitor(int row);
    void sigEdit(int row);
    void sigTerminal(int row);
    void sigDelete(int row);
    void sigRun(QModelIndex index);
    void sigStop(QModelIndex index);
    void sigRestart(QModelIndex index);
    void sigGenerateTemp(QModelIndex index);
    void sigImagePass(int row);
    void sigImageRefuse(int row);
    void sigBackupResume(int row);
    void sigBackupUpdate(int row);
    void sigBackupRemove(int row);
    void sigBackupExport(int row);
    void sigWarnRead(int row);
    void sigWarnIgnore(int row);
    void sigTableHeightChanged(int height);
    void sigItemClicked(const QModelIndex &index);
    void sigItemEntered(const QModelIndex &index);  //鼠标进入item
    void sigUpdatePaging(int);                      //更新页面
    void sigHasRunningCtn(bool);
    void sigPagingSearch(QString);
    void sigRefreshSearchResult();  // 刷新搜索结果

private slots:
    void onMonitor(int row);
    void onTerminal(int row);
    void onEdit(int row);
    void onDelete(int row);
    void onActRun(QModelIndex index);
    void onActStop(QModelIndex index);
    void onActRestart(QModelIndex index);
    //void onActImagePass(int row);
    //void onActImageRefuse(int row);
    void onActBackupResume(int row);
    void onActBackupUpdate(int row);
    void onActBackupRemove(int row);
    void onRefreshTimeout();
    void search();
    void refresh();
    void onItemChecked(QStandardItem *changeItem);
    void onItemClicked(const QModelIndex &index);
    void onItemEntered(const QModelIndex &index);
    void onHeaderCkbTog(bool toggled);
    void lastBtnClick();
    void nextBtnClick();
    void pageEditChage();  //输入框回车按下

private:
    Ui::TablePage *ui;
    QString m_keyword;
    QStandardItemModel *m_model;
    HeaderView *m_headerView;
    QTimer *m_searchTimer;
    QTimer *m_refreshBtnTimer;
    int m_timerID;
    ButtonDelegate *m_btnDelegate;

    QList<QAbstractButton *> m_singleOpBtns;
    QList<QAbstractButton *> m_batchOpBtns;
    bool m_isHeadCheckable;
    bool m_singleChoose;
    bool m_isSetTableActions;
    QLineEdit *m_pageEdit;  // 分页编辑框
    int m_totalPages = 1;
    bool m_isOpenPaging;
    QHBoxLayout *m_pagingHlayout;
    QLabel *m_totalPageLabel;
    int m_searchCol;
};

#endif  // TABLEPAGE_H
