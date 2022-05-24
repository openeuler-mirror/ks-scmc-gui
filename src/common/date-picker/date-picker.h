#ifndef DATEPICKER_H
#define DATEPICKER_H

#include <QDateTime>
#include <QObject>
#include <QStackedWidget>
#include <QWidget>
#include "calendar-widget.h"
class DatePicker : public QWidget
{
    Q_OBJECT
public:
    explicit DatePicker(QWidget *parent = nullptr);
    ~DatePicker();
    //void SetQuickValue(QDatePanel::QuickPick pick);
    QDateTime getStartDate();
    QDateTime getEndDate();
    void showDatePicker(int type);  //start:0  end:1

private slots:
    void startDateChanged(QDate date);
    void endDateChanged(QDate date);

signals:
    void sigStartdateChange(QString);
    void sigEndDateChange(QString);

private:
    void initUI();

private:
    CalendarWidget *m_startCalendar;
    CalendarWidget *m_endCalendar;
    QString m_startDate;
    QString m_endDate;
    QStackedWidget *m_stackedWidget;
};

#endif  // DATEPICKER_H
