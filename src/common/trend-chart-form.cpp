/**
 * @file          /ks-scmc-gui/src/common/trend-chart-form.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "trend-chart-form.h"
#include <kiran-log/qt5-log-i.h>
#include <QDateTimeAxis>
#include <QStyleOption>
#include <QVBoxLayout>
#include <iostream>

TrendChartForm::TrendChartForm(QWidget *parent) : QWidget(parent),
                                                  m_valueLabel(nullptr),
                                                  m_chartView(nullptr),
                                                  m_xAxis(nullptr),
                                                  m_yAxis(nullptr)
{
    qRegisterMetaType<ChartInfo>("ChartInfo");
    initUI();
}

TrendChartForm::~TrendChartForm()
{
}

void TrendChartForm::initChart(ChartInfo chartInfo)
{
    KLOG_INFO() << "initChart";
    QChart *chart = m_chartView->chart();
    //折线图
    for (auto iter = chartInfo.seriesInfo.begin(); iter != chartInfo.seriesInfo.end(); iter++)
    {
        QSplineSeries *series = new QSplineSeries();
        QPen pen;
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(2);
        pen.setColor(QColor(iter.value()));
        series->setPen(pen);          //折现序列的线条设置
        series->setName(iter.key());  //legend中的文字
        chart->addSeries(series);
        connect(series, &QSplineSeries::hovered, this, &TrendChartForm::slotPointHoverd);
    }

    QFont font;
    font.setPixelSize(12);
    font.setStyle(QFont::StyleNormal);
    //x轴
    m_xAxis = new QDateTimeAxis(m_chartView);
    m_xAxis->setRange(chartInfo.xStart, chartInfo.xEnd);
    m_xAxis->setTitleVisible(true);
    m_xAxis->setTitleText(chartInfo.xTitle);
    m_xAxis->setTitleBrush(QBrush(QColor(255, 255, 255)));
    m_xAxis->setTickCount(chartInfo.xTickCount);  //主分隔个数 //设置大刻度线的数目，默认是5，不能小于2
    m_xAxis->setGridLineVisible(true);
    m_xAxis->setMinorGridLineVisible(false);
    m_xAxis->setFormat(chartInfo.xFormat);
    m_xAxis->setLabelsFont(font);
    m_xAxis->setLabelsColor(QColor("#919191"));
    m_xAxis->setLabelsAngle(30);

    //Y轴
    m_yAxis = new QValueAxis(m_chartView);
    m_yAxis->setRange(chartInfo.yStart, chartInfo.yEnd);
    m_yAxis->setTickCount(chartInfo.yTickCount);
    m_yAxis->setTitleText(chartInfo.yTitle);
    m_yAxis->setTitleBrush(QBrush(QColor(255, 255, 255)));
    m_yAxis->setLabelFormat(chartInfo.yFormat);
    m_yAxis->setLabelsFont(font);
    m_yAxis->setLabelsColor(QColor("#919191"));
    m_yAxis->setMinorTickCount(0);  //设置小刻度线的数目，小刻度线就是没有刻度的线，这里要注意一下，如果你设成5，就是说明两个大刻度线之间有5条小刻度线，分成了6个小区间，而不是5个小区间。

    QPen axisPen;
    axisPen.setColor(QColor(57, 57, 57));
    axisPen.setStyle(Qt::SolidLine);
    axisPen.setWidth(1);
    m_yAxis->setGridLinePen(axisPen);
    m_xAxis->setGridLinePen(axisPen);

    QList<QAbstractSeries *> series = m_chartView->chart()->series();
    foreach (auto serie, series)
    {
        chart->setAxisX(m_xAxis, serie);
        chart->setAxisY(m_yAxis, serie);
    }
}

void TrendChartForm::clearChart()
{
    QList<QAbstractSeries *> serieses = m_chartView->chart()->series();
    foreach (auto series, serieses)
    {
        QSplineSeries *ser = qobject_cast<QSplineSeries *>(series);
        ser->clear();
    }
}

void TrendChartForm::updateChart(ChartInfo chartInfo, QList<QPointF> datas, QString seriesNames)
{
    KLOG_INFO() << "updateChart" << chartInfo.yStart << chartInfo.yEnd;

    KLOG_INFO() << "setDate" << seriesNames;

    QChart *chart = m_chartView->chart();
    //x轴
    chart->axisX()->setRange(chartInfo.xStart, chartInfo.xEnd);
    chart->axisX()->setTitleText(chartInfo.xTitle);
    qobject_cast<QDateTimeAxis *>(chart->axisX())->setFormat(chartInfo.xFormat);

    //Y轴
    chart->axisY()->setRange(chartInfo.yStart, chartInfo.yEnd);
    chart->axisY()->setTitleText(chartInfo.yTitle);
    qobject_cast<QValueAxis *>(chart->axisY())->setLabelFormat(chartInfo.yFormat);

    QList<QAbstractSeries *> serieses = m_chartView->chart()->series();
    foreach (auto series, serieses)
    {
        QSplineSeries *ser = qobject_cast<QSplineSeries *>(series);
        if (ser->name() == seriesNames)
        {
            KLOG_INFO() << ser->name();
            ser->clear();
            ser->append(datas);
            break;
        }
    }

    chart->update();
}

void TrendChartForm::setLegendVisible(bool visible)
{
    QChart *chart = m_chartView->chart();
    chart->legend()->setVisible(visible);
}

QSize TrendChartForm::sizeHint() const
{
    return QSize(800, 400);
}

void TrendChartForm::initUI()
{
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    m_chartView = new QChartView(this);
    m_chartView->setObjectName(QStringLiteral("chartView"));
    mainLayout->addWidget(m_chartView);

    m_valueLabel = new QLabel(this);
    m_valueLabel->setStyleSheet(QString("QLabel{color:#1564FF;font-size:12px; font-weight:normal;"
                                        " background-color:rgb(255, 255, 255); border-radius:4px; text-align:center;}"));
    m_valueLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_valueLabel->hide();

    QChart *chart = new QChart();
    m_chartView->setChart(chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    chart->setBackgroundVisible(false);
    //chart->setBackgroundBrush(QBrush(QColor(45, 45, 45, 0)));

    setLegendVisible(true);
    chart->legend()->setLabelColor(QColor(255, 255, 255));
}

void TrendChartForm::slotPointHoverd(const QPointF &point, bool state)
{
    if (state)
    {
        QFont font;
        font.setPixelSize(14);
        QFontMetrics fm(font);
        auto width = fm.width(QString::number(point.y()));
        auto yValue = point.y();
        yValue = point.y() > 0 ? yValue : 0;
        if (m_yAxis->labelFormat().contains("%d%%"))
        {
            m_valueLabel->setText(QString::number(yValue) + "%");
        }
        else
            m_valueLabel->setText(QString::number(yValue));
        m_valueLabel->setFixedWidth(width + 16);

        QPoint curPos = mapFromGlobal(QCursor::pos());
        m_valueLabel->move(curPos.x() - m_valueLabel->width() / 2, curPos.y() - m_valueLabel->height() * 1.5);  //移动数值
        m_valueLabel->show();                                                                                   //显示出来
    }
    else
    {
        m_valueLabel->hide();
    }
}

void TrendChartForm::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
