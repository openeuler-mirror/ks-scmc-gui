/**
 * @file          /ks-scmc-gui/src/pages/tab-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "tab-page.h"
#include <kiran-log/qt5-log-i.h>
#include <table-page.h>
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>
TabPage::TabPage(QWidget *parent) : Page(parent), m_tabWidget(nullptr)
{
    initUI();
}

void TabPage::addTabPage(QWidget *tabPage, QString key)
{
    m_tabWidget->addTab(tabPage, key);
    m_pages.append(tabPage);
}

void TabPage::updateCurrentPage()
{
    TablePage *page = qobject_cast<TablePage *>(m_tabWidget->currentWidget());
    page->updateInfo();
}

void TabPage::setCurrentPage(int index)
{
    m_tabWidget->setCurrentIndex(index);
}

void TabPage::setCurrentPage(QWidget *widget)
{
    m_tabWidget->setCurrentWidget(widget);
}

QWidget *TabPage::currentPage()
{
    return m_tabWidget->currentWidget();
}

QWidget *TabPage::firstPage()
{
    return m_pages.first();
}

void TabPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    Page::paintEvent(event);
}

void TabPage::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    setLayout(mainLayout);

    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget);

    // 连接currentChanged，获取最新的当前页面索引。
    // 若连接tabBarClicked信号，传入的index是点击用户点击的tab索引，但是currentPage没有改变，仍是上一个页面。不适用于调用currentPage()来更新tab页的情况。
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &TabPage::onCurrentPageChanged);
}

void TabPage::onCurrentPageChanged(int index)
{
    emit sigTabBarClicked(index);
}
