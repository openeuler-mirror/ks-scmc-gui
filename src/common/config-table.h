/**
 * @file          /ks-scmc-gui/src/common/config-table.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef CONFIGTABLE_H
#define CONFIGTABLE_H

#include <QSharedPointer>
#include <QWidget>
#include "config-model.h"
#include "config-view.h"

namespace Ui
{
class ConfigTable;
}

class ConfigTable : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigTable(ConfigTableType whichTable, QWidget *parent = nullptr);
    ~ConfigTable();
    void initwindow();
    void initTable();
    void paintEditor(int curRow);
    QList<QSharedPointer<ModelItem>> getAllData();
    void setData(QList<QSharedPointer<ModelItem> > itemList);

private slots:
    void saveRowSlot(int row);
    void editRowSlot(int row);
    void addRowSlot(int row);
    void removeRowSlot(int row);

private:
    Ui::ConfigTable *ui = nullptr;

    QSharedPointer<ConfigView> m_pHeaderView = nullptr;
    QSharedPointer<ConfigModel> m_pModel = nullptr;
    QSharedPointer<ConfigDelegate> m_pDelegate = nullptr;

    ConfigTableType m_ChooseTable;  //0:环境变量; 1:共享目录
};

#endif  // CONFIGTABLE_H
