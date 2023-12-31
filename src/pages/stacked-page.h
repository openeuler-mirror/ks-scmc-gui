/**
 * @file          /ks-scmc-gui/src/pages/stacked-page.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef STACKEDPAGE_H
#define STACKEDPAGE_H

#include <QStackedWidget>
#include "page.h"
class StackedPage : public Page
{
    Q_OBJECT
public:
    explicit StackedPage(QWidget *parent = nullptr);
    virtual ~StackedPage();
    virtual void updateInfo(QString keyword = "") = 0;

    //TODO:若后面开发用不上key，则去掉该参数
    void addStackedPage(QWidget *, QString key);

    void setCurrentPage(int index);
    void updateCurrentPage(QString keyword = "");

private:
    void initUI();

private slots:
    void onCurrentPageChanged(int);

private:
    QStackedWidget *m_stackedWidget;
    //QMap<QString, QWidget *> m_pageMap;
};

#endif  // STACKEDPAGE_H
