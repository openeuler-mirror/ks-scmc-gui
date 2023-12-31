/**
 * @file          /ks-scmc-gui/src/common/date-picker/date-picker.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "date-picker.h"
#include <QHBoxLayout>

DatePicker::DatePicker(QWidget *parent) : QWidget(parent), m_startCalendar(nullptr), m_endCalendar(nullptr), m_stackedWidget(nullptr)
{
    initUI();
}

DatePicker::~DatePicker()
{
}

QDateTime DatePicker::getStartDate()
{
    QDate startDate = m_startCalendar->getSelectDate();
    QDate endDate = m_endCalendar->getSelectDate();
    QDateTime start = QDateTime(startDate);
    if (startDate == endDate)
    {
        start.setTime(QTime(0, 0, 0));
    }
    return start;
}

QDateTime DatePicker::getEndDate()
{
    QDate startDate = m_startCalendar->getSelectDate();
    QDate endDate = m_endCalendar->getSelectDate();
    QDateTime end = QDateTime(endDate);
    if (startDate == endDate)
    {
        end.setTime(QTime(23, 59, 59));
    }
    return end;
}

void DatePicker::showDatePicker(int type)
{
    m_stackedWidget->setCurrentIndex(type);
}

void DatePicker::startDateChanged(QDate date)
{
    m_endCalendar->setMinimumDate(date);
    m_endCalendar->setSelectableStart(date);
    m_endCalendar->setSelectableEnd(m_endCalendar->selectedDate());
    m_startCalendar->setSelectableStart(date);
    m_startCalendar->setSelectableEnd(m_endCalendar->selectedDate());

    emit sigStartdateChange(date.toString("yy-MM-dd"));
}

void DatePicker::endDateChanged(QDate date)
{
    m_endCalendar->setSelectableStart(m_startCalendar->selectedDate());
    m_endCalendar->setSelectableEnd(date);
    m_startCalendar->setSelectableStart(m_startCalendar->selectedDate());
    m_startCalendar->setSelectableEnd(date);

    emit sigEndDateChange(date.toString("yy-MM-dd"));
}

void DatePicker::initUI()
{
    setWindowFlags(Qt::Widget | Qt::Popup | Qt::FramelessWindowHint);
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(10);
    setLayout(mainLayout);

    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget);

    QDate currDate = QDate::currentDate();

    m_startCalendar = new CalendarWidget(this);
    //m_startCalendar->hideNextButton();
    m_startCalendar->setSelectableStart(currDate.addDays(-7));
    m_startCalendar->setSelectableEnd(currDate);
    m_startCalendar->setMinimumDate(currDate.addDays(-7));
    m_startCalendar->setMaximumDate(currDate);
    m_startCalendar->setSelectedDate(currDate.addDays(-7));
    connect(m_startCalendar, &CalendarWidget::clicked, this, &DatePicker::startDateChanged);

    m_endCalendar = new CalendarWidget(this);
    //m_endCalendar->hidePreButton();
    m_endCalendar->setSelectableStart(currDate.addDays(-7));
    m_endCalendar->setSelectableEnd(currDate);
    m_endCalendar->setMinimumDate(currDate.addDays(-7));
    m_endCalendar->setMaximumDate(currDate);
    m_endCalendar->setSelectedDate(currDate);
    connect(m_endCalendar, &CalendarWidget::clicked, this, &DatePicker::endDateChanged);

    m_stackedWidget->addWidget(m_startCalendar);
    m_stackedWidget->addWidget(m_endCalendar);
}
