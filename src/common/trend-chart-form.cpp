#include "trend-chart-form.h"
#include <kiran-log/qt5-log-i.h>
#include <QDateTimeAxis>
#include <QVBoxLayout>
#include <iostream>
TrendChartForm::TrendChartForm(QWidget *parent) : QWidget(parent), m_valueLabel(nullptr), m_chartView(nullptr), m_xAxis(nullptr), m_yAxis(nullptr)
{
    qRegisterMetaType<ChartInfo>("ChartInfo");
    initUI();
}

TrendChartForm::~TrendChartForm()
{
}

void TrendChartForm::initChart(ChartInfo chartInfo)
{
    QChart *chart = m_chartView->chart();
    //折线图
    int i = 1;
    foreach (auto name, chartInfo.seriesNames)
    {
        QLineSeries *series = new QLineSeries();
        QPen pen;
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(4);
        pen.setColor(QColor(21 * i, 100, 255));
        series->setPen(pen);    //折现序列的线条设置
        series->setName(name);  //legend中的文字
        chart->addSeries(series);
        i++;
        //series0->setPointLabelsVisible(true);
        connect(series, &QLineSeries::hovered, this, &TrendChartForm::slotPointHoverd);
    }
    //x轴
    m_xAxis = new QDateTimeAxis();
    m_xAxis->setRange(chartInfo.xStart, chartInfo.xEnd);
    m_xAxis->setTitleText(chartInfo.xTitle);

    m_xAxis->setTickCount(chartInfo.xTickCount);  //主分隔个数 //设置大刻度线的数目，默认是5，不能小于2
    m_xAxis->setGridLineVisible(true);
    m_xAxis->setMinorGridLineVisible(false);
    m_xAxis->setFormat(chartInfo.xFormat);

    //Y轴
    m_yAxis = new QValueAxis();
    m_yAxis->setRange(chartInfo.yStart, chartInfo.yEnd);
    m_yAxis->setTickCount(chartInfo.yTickCount);
    m_yAxis->setTitleText(chartInfo.yTitle);
    //m_yAxis->setLabelFormat("0.00000f%");
    m_yAxis->setMinorTickCount(0);  //设置小刻度线的数目，小刻度线就是没有刻度的线，这里要注意一下，如果你设成5，就是说明两个大刻度线之间有5条小刻度线，分成了6个小区间，而不是5个小区间。

    QPen axisPen;
    axisPen.setColor(QColor(231, 238, 251));
    axisPen.setStyle(Qt::DotLine);
    axisPen.setWidth(2);
    m_yAxis->setGridLinePen(axisPen);

    QList<QAbstractSeries *> series = m_chartView->chart()->series();
    foreach (auto serie, series)
    {
        chart->setAxisX(m_xAxis, serie);
        chart->setAxisY(m_yAxis, serie);
    }
}

void TrendChartForm::updateChart(ChartInfo chartInfo)
{
    KLOG_INFO() << "updateChart" << chartInfo.yStart << chartInfo.yEnd;
    //x轴
    m_xAxis->setRange(chartInfo.xStart, chartInfo.xEnd);
    m_xAxis->setTitleText(chartInfo.xTitle);
    m_xAxis->setTickCount(chartInfo.xTickCount);  //主分隔个数 //设置大刻度线的数目，默认是5，不能小于2
    m_xAxis->setFormat(chartInfo.xFormat);

    //Y轴
    m_yAxis->setRange(chartInfo.yStart, chartInfo.yEnd);
}

void TrendChartForm::setData(QList<QPointF> datas, QString seriesNames)
{
    KLOG_INFO() << "setDate" << /*datas <<*/ seriesNames;
    QList<QAbstractSeries *> serieses = m_chartView->chart()->series();
    foreach (auto series, serieses)
    {
        QLineSeries *ser = qobject_cast<QLineSeries *>(series);
        if (ser->name() == seriesNames)
        {
            KLOG_INFO() << ser->name();
            ser->clear();
            ser->append(datas);
            break;
        }
    }
}

void TrendChartForm::setLegendVisible(bool visible)
{
    QChart *chart = m_chartView->chart();
    chart->legend()->setVisible(visible);
}

void TrendChartForm::initUI()
{
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    ///TODO:方便测试，后续去掉
    setFixedSize(800, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    m_chartView = new QChartView(this);
    m_chartView->setObjectName(QStringLiteral("chartView"));
    mainLayout->addWidget(m_chartView);

    m_valueLabel = new QLabel(this);
    m_valueLabel->setStyleSheet(QString("QLabel{color:#1564FF; font-family:\"Microsoft Yahei\"; font-size:14px; font-weight:bold;"
                                        " background-color:rgba(21, 100, 255, 51); border-radius:4px; text-align:center;}"));
    m_valueLabel->setFixedSize(44, 24);
    m_valueLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_valueLabel->hide();

    QChart *chart = new QChart();
    m_chartView->setChart(chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    chart->setBackgroundBrush(QBrush(QColor(248, 251, 255)));
    setLegendVisible(true);
}

void TrendChartForm::slotPointHoverd(const QPointF &point, bool state)
{
    if (state)
    {
        QFont font;
        font.setPixelSize(14);
        QFontMetrics fm(font);
        auto width = fm.width(QString::number(point.y())) + 10;
        m_valueLabel->setText(QString::number(point.y()));
        m_valueLabel->setFixedWidth(width);

        QPoint curPos = mapFromGlobal(QCursor::pos());
        m_valueLabel->move(curPos.x() - m_valueLabel->width() / 2, curPos.y() - m_valueLabel->height() * 1.5);  //移动数值
        m_valueLabel->show();                                                                                   //显示出来
    }
    else
    {
        m_valueLabel->hide();
    }
}
