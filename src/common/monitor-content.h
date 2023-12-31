/**
 * @file          /ks-scmc-gui/src/common/monitor-content.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef MONITOR_CONTENT_H
#define MONITOR_CONTENT_H

#include <kiran-titlebar-window.h>
#include <QDateTime>
#include <QWidget>
#include "info-worker.h"
#include "trend-chart-form.h"
namespace Ui
{
class MonitorContent;
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
    CHART_CYCLE_CUSTOM           //10  //24 * 60
};

enum CalendarType
{
    CALENDAR_TYPE_START = 0,
    CALENDAR_TYPE_END
};

class FlowLayout;
class TrendChartForm;
class DatePicker;
class MonitorContent : public QWidget
{
    Q_OBJECT

public:
    explicit MonitorContent(QWidget *parent = nullptr, int nodeId = -1, std::string containerId = "");
    ~MonitorContent();
    void updateMonitorInfo(qint64 nodeId, std::string containerId = "");
    virtual QSize sizeHint() const override;

private:
    void initUI();
    void initChart();
    void clearChartPoint();
    void BuildCharts(TrendChartForm *chartForm, QMap<QString, QString> seriesinfo, QString yTitle, QString yformate);
    int getbit(double num);
    void handleYValue(double &start, double &end, QString &unit);

private slots:
    void onCycleChanged(int index);
    void popupStartDatePicker();
    void popupEndDatePicker();
    void applyDatePicker();
    void getMonitorHistoryResult(const QString objID, const QPair<grpc::Status, container::MonitorHistoryReply> &reply);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::MonitorContent *ui;
    QString m_objId;
    FlowLayout *m_flowLayout;
    int m_nodeId;
    std::string m_containerId;
    TrendChartForm *m_cpuChartForm;
    TrendChartForm *m_memoryChartForm;
    TrendChartForm *m_diskChartForm;
    TrendChartForm *m_netChartForm;
    QDateTime m_xStart;
    QDateTime m_xEnd;
    int m_xInterval;
    QString m_xFormat;
    QString m_xTitle;
    DatePicker *m_datePicker;
};

#endif  // MONITOR_CONTENT_H
