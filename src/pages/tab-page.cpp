#include "tab-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>
TabPage::TabPage(QWidget *parent) : Page(parent), m_tabWidget(nullptr)
{
    KLOG_INFO() << "TabPage";
    initUI();
}

void TabPage::addTabPage(QWidget *tabPage, QString key)
{
    m_tabWidget->addTab(tabPage, key);
}

void TabPage::updateCurrentPage()
{
    TablePage *page = qobject_cast<TablePage *>(m_tabWidget->currentWidget());
    page->updateInfo();
}

void TabPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void TabPage::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    setLayout(mainLayout);

    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget);
    connect(m_tabWidget, &QTabWidget::tabBarClicked, this, &TabPage::onCurrentPageChanged);
}

void TabPage::onCurrentPageChanged(int index)
{
    KLOG_INFO() << "tabBarClicked: " << index;
    emit sigTabBarClicked(index);
    //    TablePage *page = qobject_cast<TablePage *>(m_tabWidget->currentWidget());
    //    if (page)
    //        page->updateInfo();
}
