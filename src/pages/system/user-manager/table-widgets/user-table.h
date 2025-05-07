#ifndef USERTABLE_H
#define USERTABLE_H

#include <QAbstractTableModel>
#include <QList>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QWidget>
#include "common/table/checkable-header.h"
#include "def.h"

struct UserInfo
{
    // 是否被选中
    bool selected;
    // 用户名
    QString userName;
    // 用户ID
    qint64 userID;
    // 角色
    QString role;
    // 角色ID
    qint64 roleID;
    // 创建时间
    QString createTime;
};

class UserDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    UserDelegate(QObject *parent = 0);
    virtual ~UserDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

signals:
    void editClicked(const QModelIndex &index);
    void deleteClicked(const QModelIndex &index);

private:
    bool isSystemUser(const QString &role) const;

private:
    QMap<QModelIndex, QPair<QRect, QRect>> m_buttonRects;
    QMap<QModelIndex, QRect> m_checkboxRects;
};

class UserFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    UserFilterModel(QObject *parent = nullptr);
    virtual ~UserFilterModel(){};

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

class UserModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    UserModel(QObject *parent = nullptr);
    virtual ~UserModel(){};

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QList<UserInfo> getUserInfos();

    void addUser(const UserInfo &user);
    void removeUser(QModelIndex index);
    void updateRecord(QList<UserInfo> userInfos);

signals:
    void stateChanged(Qt::CheckState checkState);
    void usersUpdate(int total);

private:
    void checkSelectStatus();
    bool isSystemUser(const QString &role) const;

private:
    QList<UserInfo> m_usersInfo;
    int m_checkableRowNum = 0;
};

class UserTable : public QTableView
{
    Q_OBJECT
public:
    UserTable(QWidget *parent = nullptr);
    virtual ~UserTable(){};

    UserFilterModel *getFilterProxy() { return m_filterProxy; };
    void searchTextChanged(const QString &text);

    void setUserInfos(QList<UserInfo> userInfos);
    QList<UserInfo> getUserInfos();

    QList<UserInfo> getSelectedUserInfos();
    void removeUsers(QList<UserInfo> userInfos);

    void clearTable();

private:
    void mouseEnter(const QModelIndex &index);

signals:
    void checkStateChanged(Qt::CheckState checkState);
    void usersUpdate(int total);
    void userDeleted(qint64 userID);
    void userEdited(qint64 userID, QString userName, qint64 roleID);

private slots:
    void checkedAllItem(Qt::CheckState checkState);
    void onEditClicked(const QModelIndex &index);
    void onDeleteClicke(const QModelIndex &index);

private:
    UserFilterModel *m_filterProxy;
    UserModel *m_model;
    CheckableHeader *m_headerViewProxy;
};

#endif  // USERTABLE_H
