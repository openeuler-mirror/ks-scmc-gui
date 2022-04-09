#ifndef MONITOR_H
#define MONITOR_H

#include <QDateTime>
#include <QWidget>
#include "info-worker.h"
namespace Ui
{
class Monitor;
}
enum ChartDataType
{
    CHART_DATA_TYPE_CPU = 0,
    CHART_DATA_TYPE_MEMORY,
    CHART_DATA_TYPE_DISK,
    CHART_DATA_TYPE_NETWORK
};

//不同的周期，所取的数据个数
enum ChartCycleNum
{
    CHART_CYCLE_TEN_MINUTE = 0,  //10  //1
    CHART_CYCLE_ONE_HOUR,        //12  //5
    CHART_CYCLE_ONE_DAY,         //12  //2 * 60
    CHART_CYCLE_ONE_WEEK,        //14  //12 * 60,
    CHART_CYCLE_ONE_MONTH,       //15  //2 * 24 * 60
    CHART_CYCLE_CUSTOM           //10  //24 * 60
};

class FlowLayout;
class TrendChartForm;
class Monitor : public QWidget
{
    Q_OBJECT

public:
    explicit Monitor(int nodeId, std::string containerId = "", QWidget *parent = nullptr);
    ~Monitor();

private:
    void initUI();
    void initChart();
    void BuildCharts(TrendChartForm *chartForm, QStringList seriesName, QString yTitle);
    void setChartData();

private slots:
    void onCycleChanged(int index);
    void getMonitorHistoryResult(const QPair<grpc::Status, container::MonitorHistoryReply> &reply);

private:
    Ui::Monitor *ui;
    FlowLayout *m_flowLayout;
    TrendChartForm *m_cpuChartForm;
    TrendChartForm *m_memoryChartForm;
    TrendChartForm *m_diskChartForm;
    TrendChartForm *m_netChartForm;
    int m_nodeId;
    std::string m_containerId;
    QDateTime m_xStart;
    QDateTime m_xEnd;
    int m_xInterval;
    QString m_xFormat;
};

#endif  // MONITOR_H
