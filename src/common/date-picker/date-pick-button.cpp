/**
 * @file          /ks-scmc-gui/src/common/date-picker/date-pick-button.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "date-pick-button.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QStyleOption>
DatePickButton::DatePickButton(QWidget *parent) : QPushButton(parent), m_dateLabel(nullptr)
{
    initUI();
}

void DatePickButton::setText(QString date)
{
    m_dateLabel->setText(date);
}

void DatePickButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void DatePickButton::initUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(10, 0, 0, 0);
    mainLayout->setSpacing(10);
    setLayout(mainLayout);

    QLabel *labIcon = new QLabel(this);
    labIcon->setObjectName("lab_icon");
    labIcon->setFixedSize(16, 16);
    labIcon->setPixmap(QPixmap(":/images/icon_calendar.png"));
    //labIcon->setStyleSheet("#lab_icon{image: url(:/images/icon_calendar.png);");

    m_dateLabel = new QLabel(this);

    mainLayout->addWidget(labIcon);
    mainLayout->addWidget(m_dateLabel);
}
