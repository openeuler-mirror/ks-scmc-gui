#include "calendar-widget.h"

#include <QLabel>
#include <QLayout>
#include <QLocale>
#include <QPainter>
#include <QProxyStyle>
#include <QPushButton>
#include <QStyleOption>
#include <QTableView>
#include <QTextCharFormat>

class QCustomStyle : public QProxyStyle
{
public:
    QCustomStyle(QWidget *parent)
    {
        setParent(parent);
    };

private:
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                       QPainter *painter, const QWidget *widget) const
    {
        if (element == PE_FrameFocusRect)
        {
            return;
        }
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};

CalendarWidget::CalendarWidget(QWidget *parent)
    : QCalendarWidget(parent)
{
    m_selectableStart = minimumDate();
    m_selectableEnd = maximumDate();
    initControl();
}

CalendarWidget::~CalendarWidget()
{
}

QDate CalendarWidget::getSelectDate()
{
    return selectedDate();
}

void CalendarWidget::setSelectableStart(QDate start)
{
    m_selectableStart = start;
    updateCells();
}

void CalendarWidget::setSelectableEnd(QDate end)
{
    m_selectableEnd = end;
    updateCells();
}

QDate CalendarWidget::getSelectableStart()
{
    return m_selectableStart;
    updateCells();
}

QDate CalendarWidget::getSelectableEnd()
{
    return m_selectableEnd;
    updateCells();
}

void CalendarWidget::hideNextButton()
{
    m_rightMonthBtn->hide();
    m_rightYearBtn->hide();
}

void CalendarWidget::hidePreButton()
{
    m_leftMonthBtn->hide();
    m_leftYearBtn->hide();
}

void CalendarWidget::initControl()
{
    setObjectName("CalendarWidget");
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    setLocale(QLocale(QLocale::Chinese));
    setNavigationBarVisible(false);
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
    setStyle(new QCustomStyle(this));

    QFont font;
    font.setPixelSize(14);
    QTextCharFormat singleFormat;
    singleFormat.setForeground(QColor(0, 0, 0));
    singleFormat.setBackground(QColor(218, 226, 239));
    singleFormat.setFont(font);
    QTextCharFormat doubleFormat;
    doubleFormat.setForeground(QColor(0, 0, 0));
    //doubleFormat.setBackground(QColor(210, 218, 232));
    doubleFormat.setFont(font);

    //    QTextCharFormat dayFormat;
    //    dayFormat.setForeground(QColor(0, 0, 0));
    //    dayFormat.setBackground(QColor(255, 255, 255));
    //    dayFormat.setFont(font);

    setHeaderTextFormat(singleFormat);
    setWeekdayTextFormat(Qt::Saturday, doubleFormat);
    setWeekdayTextFormat(Qt::Sunday, doubleFormat);
    //    setWeekdayTextFormat(Qt::Monday, doubleFormat);
    //    setWeekdayTextFormat(Qt::Tuesday, singleFormat);
    //    setWeekdayTextFormat(Qt::Wednesday, doubleFormat);
    //    setWeekdayTextFormat(Qt::Thursday, singleFormat);
    //    setWeekdayTextFormat(Qt::Friday, doubleFormat);

    //setStyleSheet("QAbstractItemView{color:black;background:#888A85}");

    initTopWidget();
    //initBottomWidget();

    connect(this, &QCalendarWidget::currentPageChanged, [this](int year, int month) {
        setDataLabelTimeText(year, month);
    });
}

void CalendarWidget::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    if (date == selectedDate())
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(46, 179, 255));
        painter->drawRect(rect.x(), rect.y(), rect.width(), rect.height());
        //painter->drawRoundedRect(rect.x(), rect.y(), rect.width(), rect.height(), 3, 3);
        painter->setPen(QColor(255, 255, 255));

        painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));
        painter->restore();
    }
    //    else if (date == QDate::currentDate())
    //    {
    //        //        painter->save();
    //        //        painter->setRenderHint(QPainter::Antialiasing);
    //        //        painter->setPen(Qt::NoPen);
    //        //        painter->setBrush(QColor(0, 161, 255));
    //        //        painter->drawRoundedRect(rect.x(), rect.y(), rect.width(), rect.height(), 3, 3);
    //        //        painter->setBrush(QColor(255, 255, 255));
    //        //        painter->drawRoundedRect(rect.x() + 1, rect.y() + 4, rect.width() - 2, rect.height() - 8, 2, 2);
    //        //        painter->setPen(QColor(46, 179, 255));

    //        //        painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));
    //        //        painter->restore();
    //    }
    //    else if (date >= minimumDate() && date <= maximumDate())
    //    {
    //        painter->save();
    //        painter->setRenderHint(QPainter::Antialiasing);
    //        painter->setPen(Qt::NoPen);
    //        painter->setBrush(QColor(255, 255, 255));

    //        painter->drawRect(rect.x(), rect.y() + 3, rect.width(), rect.height() - 6);
    //        painter->setPen(QColor(0, 0, 0));
    //        // painter->setPen(QColor(255, 0, 0));

    //        painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));
    //        painter->restore();
    //    }
    else if (date >= m_selectableStart && date <= m_selectableEnd)
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(218, 226, 239));

        painter->drawRect(rect.x(), rect.y(), rect.width(), rect.height());
        painter->setPen(QColor(255, 255, 255));

        painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));
        painter->restore();
    }
    else if (date < m_selectableStart && date > m_selectableEnd)
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(255, 255, 255));
        painter->drawRect(rect.x(), rect.y(), rect.width(), rect.height());

        painter->setPen(QColor(0, 0, 0));
        painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));
        painter->restore();
    }
    else
    {
        QCalendarWidget::paintCell(painter, rect, date);
    }
}

void CalendarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CalendarWidget::initTopWidget()
{
    QWidget *topWidget = new QWidget(this);
    topWidget->setObjectName("CalendarTopWidget");
    topWidget->setFixedHeight(40);
    topWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->setContentsMargins(12, 0, 12, 0);
    hboxLayout->setSpacing(4);

    m_leftYearBtn = new QPushButton(this);
    m_leftMonthBtn = new QPushButton(this);
    m_rightYearBtn = new QPushButton(this);
    m_rightMonthBtn = new QPushButton(this);
    m_dataLabel = new QLabel(this);

    m_leftYearBtn->setObjectName("CalendarLeftYearBtn");
    m_leftMonthBtn->setObjectName("CalendarLeftMonthBtn");
    m_rightYearBtn->setObjectName("CalendarRightYearBtn");
    m_rightMonthBtn->setObjectName("CalendarRightMonthBtn");
    m_dataLabel->setObjectName("CalendarDataLabel");

    m_leftYearBtn->setFixedSize(16, 16);
    m_leftMonthBtn->setFixedSize(16, 16);
    m_rightYearBtn->setFixedSize(16, 16);
    m_rightMonthBtn->setFixedSize(16, 16);

    hboxLayout->addWidget(m_leftYearBtn);
    hboxLayout->addWidget(m_leftMonthBtn);
    hboxLayout->addStretch();
    hboxLayout->addWidget(m_dataLabel);
    hboxLayout->addStretch();
    hboxLayout->addWidget(m_rightMonthBtn);
    hboxLayout->addWidget(m_rightYearBtn);
    topWidget->setLayout(hboxLayout);

    //这里见下图1
    QVBoxLayout *vBodyLayout = qobject_cast<QVBoxLayout *>(layout());
    vBodyLayout->insertWidget(0, topWidget);

    connect(m_leftYearBtn, SIGNAL(clicked()), this, SLOT(onbtnClicked()));
    connect(m_leftMonthBtn, SIGNAL(clicked()), this, SLOT(onbtnClicked()));
    connect(m_rightYearBtn, SIGNAL(clicked()), this, SLOT(onbtnClicked()));
    connect(m_rightMonthBtn, SIGNAL(clicked()), this, SLOT(onbtnClicked()));

    setDataLabelTimeText(selectedDate().year(), selectedDate().month());
}

void CalendarWidget::initBottomWidget()
{
    QWidget *bottomWidget = new QWidget(this);
    bottomWidget->setObjectName("CalendarBottomWidget");
    bottomWidget->setFixedHeight(40);
    bottomWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->setContentsMargins(12, 0, 12, 0);
    hboxLayout->setSpacing(6);

    m_ensureBtn = new QPushButton(this);
    m_ensureBtn->setObjectName("CalendarEnsureBtn");
    m_ensureBtn->setFixedSize(40, 22);
    m_ensureBtn->setText(QStringLiteral("确定"));

    m_toDayBtn = new QPushButton(this);
    m_toDayBtn->setObjectName("CalendarTodayBtn");
    m_toDayBtn->setFixedSize(40, 22);
    m_toDayBtn->setText(QStringLiteral("现在"));

    hboxLayout->addStretch();
    hboxLayout->addWidget(m_toDayBtn);
    hboxLayout->addWidget(m_ensureBtn);
    bottomWidget->setLayout(hboxLayout);

    //这里见下图1
    QVBoxLayout *vBodyLayout = qobject_cast<QVBoxLayout *>(layout());
    vBodyLayout->addWidget(bottomWidget);

    connect(m_ensureBtn, &QPushButton::clicked, [this]() {
        emit signalSetCalendarTime(selectedDate());
        emit activated(selectedDate());
    });

    connect(m_toDayBtn, &QPushButton::clicked, [this]() {
        showToday();
    });
}

void CalendarWidget::setDataLabelTimeText(int year, int month)
{
    m_dataLabel->setText(QStringLiteral("%1年%2月").arg(year).arg(month));
}

void CalendarWidget::onbtnClicked()
{
    QPushButton *senderBtn = qobject_cast<QPushButton *>(sender());
    if (senderBtn == m_leftYearBtn)
    {
        showPreviousYear();
    }
    else if (senderBtn == m_leftMonthBtn)
    {
        showPreviousMonth();
    }
    else if (senderBtn == m_rightYearBtn)
    {
        showNextYear();
    }
    else if (senderBtn == m_rightMonthBtn)
    {
        showNextMonth();
    }
}
