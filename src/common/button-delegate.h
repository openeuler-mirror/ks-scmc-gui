#ifndef BUTTONDELEGATE_H
#define BUTTONDELEGATE_H

#include <QMenu>
#include <QObject>
#include <QStyledItemDelegate>
#include "def.h"
class ButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ButtonDelegate(QMap<ACTION_BUTTON_TYPE, QString> btnInfo, QObject* parent = nullptr);
    ~ButtonDelegate();
    void isSetDelegateDefault(bool key = false);

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

signals:
    void sigMonitor(int row);
    void sigEdit(int row);
    void sigTerminal(int row);
    void sigdelete(int row);
    void sigActRun(QModelIndex);
    void sigActStop(QModelIndex);
    void sigActRestart(QModelIndex);
    void sigImagePass(int row);
    void sigImageRefuse(int row);

private slots:
    void onActTriggered(QAction* act);

private:
    QPoint m_mousePoint;
    QModelIndex m_index;
    QMap<ACTION_BUTTON_TYPE, QString> m_btnInfo;
    QStringList m_btnIcons;
    int m_nType;  // 按钮状态：0-划过 1-按下
    QMenu* m_menu;
    bool m_isSetDelegateDefault;
};

#endif  // BUTTONDELEGATE_H
