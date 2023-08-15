#ifndef COMMONPAGE_H
#define COMMONPAGE_H

#include <QPushButton>
#include <QStandardItemModel>
#include <QToolButton>
#include <QWidget>
namespace Ui
{
class CommonPage;
}

enum OperatorButtonType
{
    OPERATOR_BUTTON_TYPE_SINGLE,
    OPERATOR_BUTTON_TYPE_BATCH
};

class MaskWidget;
class HeaderView;
class CommonPage : public QWidget
{
    Q_OBJECT

public:
    explicit CommonPage(QWidget *parent = 0);
    virtual ~CommonPage();
    virtual void updateInfo(QString keyword = "") = 0;
    void setBusy(bool status);
    void clearTable();
    void addSingleOperationButton(QAbstractButton *);
    void addBatchOperationButtons(QList<QPushButton *>);
    void setOpBtnEnabled(OperatorButtonType type, bool enabled);
    void setTableColNum(int num);
    void setTableRowNum(int num);
    void setTableItem(int row, int col, QStandardItem *item);
    void setTableItems(int row, int col, QList<QStandardItem *> items);
    void setTableActions(int col, QStringList actionIcons);
    void setSortableCol(QList<int> cols);
    void setHeaderSections(QStringList names);
    void setTableDefaultContent(QString text);
    void clearText();
    int getTableRowCount();
    QStandardItem *getItem(int row, int col);
    QList<QMap<QString, QVariant>> getCheckedItemInfo(int col);
    void sleep(int sec);

private:
    void initUI();
    void adjustTableSize();

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    virtual void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void sigMonitor(int row);
    void sigEdit(int row);
    void sigTerminal(int row);
    void sigRun(QModelIndex index);
    void sigStop(QModelIndex index);
    void sigRestart(QModelIndex index);
    void sigTableHeightChanged(int height);
    void sigItemClicked(const QModelIndex &index);

private slots:
    void onMonitor(int row);
    void onTerminal(int row);
    void onEdit(int row);
    void onActRun(QModelIndex index);
    void onActStop(QModelIndex index);
    void onActRestart(QModelIndex index);
    void onRefreshTimeout();
    void search();
    void refresh();
    void onItemChecked(QStandardItem *item);
    void onItemClicked(const QModelIndex &index);
    void onHeaderCkbTog(bool toggled);

private:
    Ui::CommonPage *ui;
    QString m_keyword;
    QStandardItemModel *m_model;
    HeaderView *m_headerView;
    QTimer *m_searchTimer;
    QTimer *m_refreshBtnTimer;
    MaskWidget *m_maskWidget;
    QList<QAbstractButton *> m_singleOpBtns;
    QList<QAbstractButton *> m_batchOpBtns;
    bool m_isItemChecked = false;
};

#endif  // COMMONPAGE_H
