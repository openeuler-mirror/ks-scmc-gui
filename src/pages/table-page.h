#ifndef TABLEPAGE_H
#define TABLEPAGE_H

#include <QMouseEvent>
#include <QPushButton>
#include <QStandardItemModel>
#include <QToolButton>
#include <QWidget>
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
    explicit TablePage(QWidget *parent = 0);
    virtual ~TablePage();
    virtual void updateInfo(QString keyword = "") = 0;
    void clearTable();
    void setOperateAreaVisible(bool visible);
    void setOperateBtnsVisible(bool visible);
    void setOperateSearchVisible(bool visible);
    void addSingleOperationButton(QAbstractButton *);
    void addBatchOperationButtons(QList<QPushButton *>);
    void setOpBtnEnabled(OperatorButtonType type, bool enabled);
    void setTableColNum(int num);
    void setTableRowNum(int num);
    void setTableItem(int row, int col, QStandardItem *item);
    void setTableItems(int row, int col, QList<QStandardItem *> items);
    void setTableActions(int col, QMap<ACTION_BUTTON_TYPE, QString> btnInfo);
    void setTableSingleChoose(bool isSingleChoose);
    void setSortableCol(QList<int> cols);
    void setHeaderSections(QStringList names);
    void setHeaderCheckable(bool checkable);
    void setTableDefaultContent(QString text);
    void clearText();
    int getTableRowCount();
    QStandardItem *getItem(int row, int col);
    QList<QMap<QString, QVariant>> getCheckedItemInfo(int col);
    void sleep(int sec);

private:
    void initUI();
    void adjustTableSize();
    int getCheckedItemNum();

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void sigMonitor(int row);
    void sigEdit(int row);
    void sigTerminal(int row);
    void sigDelete(int row);
    void sigRun(QModelIndex index);
    void sigStop(QModelIndex index);
    void sigRestart(QModelIndex index);
    void sigTableHeightChanged(int height);
    void sigItemClicked(const QModelIndex &index);
    void sigItemEntered(const QModelIndex &index);  //鼠标进入item

private slots:
    void onMonitor(int row);
    void onTerminal(int row);
    void onEdit(int row);
    void onDelete(int row);
    void onActRun(QModelIndex index);
    void onActStop(QModelIndex index);
    void onActRestart(QModelIndex index);
    void onRefreshTimeout();
    void search();
    void refresh();
    void onItemChecked(QStandardItem *changeItem);
    void onItemClicked(const QModelIndex &index);
    void onItemEntered(const QModelIndex &index);
    void onHeaderCkbTog(bool toggled);

private:
    Ui::TablePage *ui;
    QString m_keyword;
    QStandardItemModel *m_model;
    HeaderView *m_headerView;
    QTimer *m_searchTimer;
    QTimer *m_refreshBtnTimer;
    QList<QAbstractButton *> m_singleOpBtns;
    QList<QAbstractButton *> m_batchOpBtns;
    bool m_singleChoose;
    bool m_isSetTableActions;
};

#endif  // TABLEPAGE_H
