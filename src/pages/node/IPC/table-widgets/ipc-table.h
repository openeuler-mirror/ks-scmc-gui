/**
 * @file          src/pages/node/IPC/table-widgets/ipc-table.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include <QAbstractTableModel>
#include <QList>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QWidget>
#include "common/table/checkable-header.h"
#include "def.h"

struct IpcInfo
{
    // 是否被选中
    bool selected;
    // ipc
    QString ipcName;
    // 使用容器
    QString containers;
    // 创建时间
    QString createTime;
};

class IpcDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    IpcDelegate(QObject *parent = 0);
    virtual ~IpcDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    QMap<QModelIndex, QRect> m_checkboxRects;
};

class IpcFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    IpcFilterModel(QObject *parent = nullptr);
    virtual ~IpcFilterModel(){};

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

class IpcModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    IpcModel(QObject *parent = nullptr);
    virtual ~IpcModel(){};

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QList<IpcInfo> getIpcInfos();

    void addIpc(const IpcInfo &ipc);
    void removeIpc(QModelIndex index);
    void updateRecord(QList<IpcInfo> ipcInfos);

signals:
    void ipcUpdate(int total);

private:
    QList<IpcInfo> m_ipcsInfo;
};

class IpcTable : public QTableView
{
    Q_OBJECT
public:
    IpcTable(QWidget *parent = nullptr);
    virtual ~IpcTable(){};

    IpcFilterModel *getFilterProxy() { return m_filterProxy; };
    void searchTextChanged(const QString &text);

    void setIpcInfos(QList<IpcInfo> ipcInfos);
    QList<IpcInfo> getIpcInfos();

    QList<IpcInfo> getSelectedIpcInfos();

    void clearTable();

private:
    void mouseEnter(const QModelIndex &index);

signals:
    void headerCheckStateChanged(Qt::CheckState checkState);
    void ipcDeleted(qint64 ipcID);
    void ipcEdited(qint64 ipcID, QString ipcName, qint64 roleID);

private slots:
    void checkedAllItem(Qt::CheckState checkState);
    void updateHeaderState();

private:
    IpcFilterModel *m_filterProxy;
    IpcModel *m_model;
    CheckableHeader *m_headerViewProxy;
};
