#include "date-pick-button.h"
#include <QHBoxLayout>

DatePickButton::DatePickButton(QWidget *parent) : QPushButton(parent), m_dateLabel(nullptr)
{
    initUI();
}

void DatePickButton::setText(QString date)
{
    m_dateLabel->setText(date);
}

void DatePickButton::initUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(10, 0, 10, 0);
    setLayout(mainLayout);

    QHBoxLayout *startDateLayout = new QHBoxLayout();
    QLabel *labIcon = new QLabel(this);
    m_dateLabel = new QLabel(this);
    startDateLayout->addWidget(labIcon);
    startDateLayout->addWidget(m_dateLabel);
}
