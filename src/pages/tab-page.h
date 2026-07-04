/**
 * @file          /ks-scmc-gui/src/pages/tab-page.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef TABPAGE_H
#define TABPAGE_H

#include <QMap>
#include <QObject>
#include <QTabWidget>
#include <QWidget>
#include "def.h"
#include "page.h"
#include "table-page.h"
class TabPage : public Page
{
    Q_OBJECT
public:
    explicit TabPage(QWidget *parent = nullptr);
    virtual ~TabPage() = default;
    virtual void updateInfo(QString keyword = "") = 0;
    void addTabPage(QWidget *tabPage, QString key);
    void updateCurrentPage();
    void setCurrentPage(int index);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void initUI();

private slots:
    void onCurrentPageChanged(int index);

signals:
    void sigTabBarClicked(int index);

private:
    QTabWidget *m_tabWidget;
    QMap<QString, QWidget *> m_pageMap;
};

#endif  // TABPAGE_H
