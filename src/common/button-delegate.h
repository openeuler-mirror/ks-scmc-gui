/**
 * @file          /ks-scmc-gui/src/common/button-delegate.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
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
    explicit ButtonDelegate(QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>> btnInfo, QObject* parent = nullptr);
    ~ButtonDelegate();
    void isSetDelegateDefault(bool key = false);

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

signals:
    void sigApp(int row);
    void sigAppRun(int row);
    void sigAppStop(int row);
    void sigMonitor(int row);
    void sigEdit(int row);
    void sigTerminal(int row);
    void sigdelete(int row);
    void sigActRun(QModelIndex);
    void sigActStop(QModelIndex);
    void sigActRestart(QModelIndex);
    void sigActGenerateTemp(QModelIndex);
    void sigImagePass(int row);
    void sigImageRefuse(int row);
    void sigBackupResume(int row);
    void sigBackupUpdate(int row);
    void sigBackupRemove(int row);
    void sigBackupExport(int row);
    void sigWarnRead(int row);
    void sigWarnIgnore(int row);

private slots:
    void onActTriggered(QAction* act);

private:
    QPoint m_mousePoint;
    QModelIndex m_index;
    QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>> m_btnInfo;
    int m_nType;  // 按钮状态：0-划过 1-按下
    QMenu* m_menu;
    bool m_isSetDelegateDefault;
};

#endif  // BUTTONDELEGATE_H
