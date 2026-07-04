#ifndef TRENDCHARTFORM_H
#define TRENDCHARTFORM_H

#include <qchartview.h>
#include <QDateTime>
#include <QLabel>
#include <QMetaType>
#include <QWidget>
#include <QtCharts>
struct ChartInfo
{
    QStringList seriesNames = {0};
    QString xTitle = "";
    QDateTime xStart;
    QDateTime xEnd;
    QString xFormat;
    int xTickCount = 10;
    QString yTitle;
    double yStart;
    double yEnd;
    QString yFormat;
    int yTickCount = 11;
};
Q_DECLARE_METATYPE(ChartInfo)

class TrendChartForm : public QWidget
{
    Q_OBJECT
public:
    explicit TrendChartForm(QWidget *parent = nullptr);
    ~TrendChartForm();
    void initChart(ChartInfo chartInfo);
    void updateChart(ChartInfo chartInfo);
    void setData(QList<QPointF> datas, QString seriesNames);
    void setLegendVisible(bool visible);

private:
    void
    initUI();
    /**
     * @brief initChart             初始化图表
     */
    //void initChart();

    /**
     * @brief bulidChart
     */
    //void bulidChart();

    /**
     * @brief prepareData
     */
    void prepareData();

private slots:
    void slotPointHoverd(const QPointF &point, bool state);

private:
    QLabel *m_valueLabel;
    QChartView *m_chartView;
    QDateTimeAxis *m_xAxis;
    QValueAxis *m_yAxis;
};

#endif  // TRENDCHARTFORM_H
