#ifndef DATEPICKBUTTON_H
#define DATEPICKBUTTON_H

#include <QLabel>
#include <QPushButton>
class DatePickButton : public QPushButton
{
    Q_OBJECT
public:
    explicit DatePickButton(QWidget *parent = nullptr);
    void setText(QString);

private:
    void initUI();
    QLabel *m_dateLabel;
};

#endif  // DATEPICKBUTTON_H
