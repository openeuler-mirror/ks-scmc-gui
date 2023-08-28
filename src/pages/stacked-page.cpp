#include "stacked-page.h"
#include <QVBoxLayout>
StackedPage::StackedPage(QWidget *parent) : Page(parent), m_stackedWidget(nullptr)
{
    initUI();
}

void StackedPage::addStackedPage(QWidget *w, QString key)
{
    m_stackedWidget->addWidget(w);
    //m_pageMap.insert(key, w);
}

void StackedPage::setCurrentPage(int index)
{
    m_stackedWidget->setCurrentIndex(index);
}

void StackedPage::updateCurrentPage()
{
    Page *page = qobject_cast<Page *>(m_stackedWidget->currentWidget());
    page->updateInfo();
}

void StackedPage::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    setLayout(mainLayout);

    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget);
    //connect(m_stackedWidget, &QStackedWidget::currentChanged, this, &StackedPage::onCurrentPageChanged);
}

void StackedPage::onCurrentPageChanged(int)
{
    Page *currPage = qobject_cast<Page *>(m_stackedWidget->currentWidget());
    currPage->updateInfo();
}