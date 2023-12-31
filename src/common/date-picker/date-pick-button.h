/**
 * @file          /ks-scmc-gui/src/common/date-picker/date-pick-button.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
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

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void initUI();
    QLabel *m_dateLabel;
};

#endif  // DATEPICKBUTTON_H
