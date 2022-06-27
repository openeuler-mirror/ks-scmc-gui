/**
 * @file          /ks-scmc-gui/src/common/date-picker/calendar-widget.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef CUSTOMCALENDARWIDGET_H
#define CUSTOMCALENDARWIDGET_H

#include <QCalendarWidget>
#include <QWidget>

class QPushButton;
class QLabel;
class CalendarWidget : public QCalendarWidget
{
    Q_OBJECT

public:
    CalendarWidget(QWidget* parent = nullptr);
    ~CalendarWidget();
    QDate getSelectDate();
    void setSelectableStart(QDate start);
    void setSelectableEnd(QDate end);
    QDate getSelectableStart();
    QDate getSelectableEnd();
    void hideNextButton();
    void hidePreButton();

private:
    void initControl();
    void initTopWidget();
    void initBottomWidget();
    void setDataLabelTimeText(int year, int month);

signals:
    void signalSetCalendarTime(const QDate& data);

private slots:
    void onbtnClicked();

protected:
    void paintCell(QPainter* painter, const QRect& rect, const QDate& date) const;
    void paintEvent(QPaintEvent* event) override;

private:
    QPushButton* m_leftYearBtn;
    QPushButton* m_leftMonthBtn;

    QPushButton* m_rightYearBtn;
    QPushButton* m_rightMonthBtn;

    QPushButton* m_ensureBtn;
    QPushButton* m_toDayBtn;

    QLabel* m_dataLabel;

    QDate m_selectableStart;
    QDate m_selectableEnd;
};

#endif  // CUSTOMCALENDARWIDGET_H
