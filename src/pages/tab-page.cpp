#include "tab-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QVBoxLayout>
//#include "audit-apply-pending-page.h"
TabPage::TabPage(QWidget *parent) : Page(parent), m_tabWidget(nullptr)
{
    KLOG_INFO() << "TabPage";
    initUI();
}

void TabPage::addTabPage(QWidget *tabPage, QString key)
{
    //    ContentPage *contentPage = nullptr;
    //    switch (type)
    //    {
    //    case PAGE_TYPE_AUDIT_CENTER_APPLY_PENDING:
    //        contentPage = new AuditApplyPendingPage(this);
    //        break;
    //    default:
    //        break;
    //    }
    m_pageMap.insert(key, tabPage);
    m_tabWidget->addTab(tabPage, key);
    //    return contentPage;
}

void TabPage::updateCurrentPage()
{
    TablePage *page = qobject_cast<TablePage *>(m_tabWidget->currentWidget());
    page->updateInfo();
}

void TabPage::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    setLayout(mainLayout);

    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &TabPage::onCurrentPageChanged);
}

void TabPage::onCurrentPageChanged(int index)
{
    TablePage *page = qobject_cast<TablePage *>(m_tabWidget->currentWidget());
    if (page)
        page->updateInfo();
}
