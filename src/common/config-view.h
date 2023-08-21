#ifndef CONFIGVIEW_H
#define CONFIGVIEW_H

#include <QComboBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QStyledItemDelegate>

enum ConfigTableType
{
    CONFIG_TABLE_TYPE_ENV,
    CONFIG_TABLE_TYPE_VOLUMES
};

class ConfigView : public QHeaderView
{
    Q_OBJECT
public:
    ConfigView(Qt::Orientation orientation, QWidget *parent = nullptr);
};

class ConfigOperateWidget;
class ConfigDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ConfigDelegate(ConfigTableType whichTable, QWidget *parent = nullptr);
    ~ConfigDelegate();
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    void dealOperateSig(const ConfigOperateWidget *pWidget) const;
    void dealMemberVar();

signals:
    void sendSaveSig(int);
    void sendEditSig(int);
    void sendAddSig(int);
    void sendDeleteSig(int);

private slots:
    void sendSaveSlot(ConfigOperateWidget *pCurWidget);
    void sendEditSlot(ConfigOperateWidget *pCurWidget);
    void sendAddSlot(ConfigOperateWidget *pCurWidget);
    void sendDeleteSlot(ConfigOperateWidget *pCurWidget);

private:
    mutable QList<QSharedPointer<QLineEdit>> m_pEditFirst;
    mutable QList<QSharedPointer<QLineEdit>> m_pEditSecond;
    mutable QList<QSharedPointer<QComboBox>> m_pComboBoxMode;
    QList<int> m_listPage;
    QStringList m_listMode;
    int m_nWidth;                   // 按钮宽度
    int m_nHeight;                  // 按钮高度
    ConfigTableType m_ChooseTable;  //0:环境变量; 1:共享目录
};

#endif  // CONFIGVIEW_H
