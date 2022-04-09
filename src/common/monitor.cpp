#include "monitor.h"
#include <kiran-log/qt5-log-i.h>
#include <QTime>
#include "flowlayout.h"
#include "trend-chart-form.h"
#include "ui_monitor.h"

#define CHART_SERIES_NAME_CPU "CPU Usage"
#define CHART_SERIES_NAME_MEMORY "Memory Usage"
#define CHART_SERIES_NAME_DISK "Disk Usage"
#define CHART_SERIES_NAME_NETWORK_RX "Network Receiving Speed"
#define CHART_SERIES_NAME_NETWORK_TX "Network Transmission Speed"

Monitor::Monitor(int nodeId, std::string containerId, QWidget *parent) : QWidget(parent),
                                                                         ui(new Ui::Monitor),
                                                                         m_flowLayout(nullptr),
                                                                         m_nodeId(nodeId),
                                                                         m_containerId(containerId),
                                                                         m_cpuChartForm(nullptr),
                                                                         m_memoryChartForm(nullptr),
                                                                         m_diskChartForm(nullptr),
                                                                         m_netChartForm(nullptr),
                                                                         m_xInterval(1)
{
    ui->setupUi(this);
    m_xFormat = "hh:mm";
    initUI();
    initChart();
}

Monitor::~Monitor()
{
    delete ui;
}

void Monitor::initUI()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    m_flowLayout = new FlowLayout(0, 30, 30);
    ui->widget_forms->setLayout(m_flowLayout);

    m_cpuChartForm = new TrendChartForm(this);
    m_flowLayout->addWidget(m_cpuChartForm);

    m_memoryChartForm = new TrendChartForm(this);
    m_flowLayout->addWidget(m_memoryChartForm);

    m_diskChartForm = new TrendChartForm(this);
    m_flowLayout->addWidget(m_diskChartForm);

    m_netChartForm = new TrendChartForm(this);
    m_flowLayout->addWidget(m_netChartForm);

    ui->cb_select_cycle->addItem(tr("10 minutes"), CHART_CYCLE_TEN_MINUTE);
    ui->cb_select_cycle->addItem(tr("1 hour"), CHART_CYCLE_ONE_HOUR);
    ui->cb_select_cycle->addItem(tr("1 day"), CHART_CYCLE_ONE_DAY);
    ui->cb_select_cycle->addItem(tr("1 week"), CHART_CYCLE_ONE_WEEK);
    ui->cb_select_cycle->addItem(tr("1 month"), CHART_CYCLE_ONE_MONTH);
    ui->cb_select_cycle->addItem(tr("Custom"), CHART_CYCLE_CUSTOM);
    connect(ui->cb_select_cycle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Monitor::onCycleChanged);
}

void Monitor::initChart()
{
    BuildCharts(m_cpuChartForm, QStringList() << CHART_SERIES_NAME_CPU, tr("CPU usage (unit M)"));
    BuildCharts(m_memoryChartForm, QStringList() << CHART_SERIES_NAME_MEMORY, tr("Memory usage (unit M)"));
    BuildCharts(m_diskChartForm, QStringList() << CHART_SERIES_NAME_DISK, tr("Disk usage (unit M)"));
    BuildCharts(m_netChartForm, QStringList() << CHART_SERIES_NAME_NETWORK_RX << CHART_SERIES_NAME_NETWORK_TX, tr("Network usage (unit M)"));

    QDateTime currTime = QDateTime::currentDateTime();  //获取当前时间
    int currTimeStamp = currTime.toTime_t();            //将当前时间转为时间戳
    QDateTime startDate = currTime.addSecs(-(60 * 10));
    int startTimestamp = startDate.toTime_t();

    connect(&InfoWorker::getInstance(), &InfoWorker::monitorHistoryFinished, this, &Monitor::getMonitorHistoryResult);
    InfoWorker::getInstance().monitorHistory(m_nodeId, startTimestamp, currTimeStamp, m_xInterval, m_containerId);  //10 minute
}

void Monitor::BuildCharts(TrendChartForm *chartForm, QStringList seriesNames, QString yTitle)
{
    ChartInfo chartInfo;
    chartInfo.seriesNames = seriesNames;

    //初始化记录前10分钟数据，1分钟间隔
    QDateTime currTime = QDateTime::currentDateTime();  //获取当前时间
    QDateTime startDate = currTime.addSecs(-(60 * 10));
    m_xEnd = currTime;
    m_xStart = startDate;
    chartInfo.xStart = startDate;
    chartInfo.xEnd = currTime;
    chartInfo.xTitle = tr("Time particle density(1 minute)");
    chartInfo.xFormat = "hh:mm";
    chartInfo.yStart = 0;
    chartInfo.yEnd = 100;
    chartInfo.yTitle = yTitle;
    chartForm->initChart(chartInfo);
}

void Monitor::setChartData()
{
}

void Monitor::onCycleChanged(int index)
{
    KLOG_INFO() << "onCycleChanged: " << index;
    int cycle = ui->cb_select_cycle->currentIndex();
    QDateTime currDate = QDateTime::currentDateTime();  //获取当前时间
    QDateTime startDate;
    switch (cycle)
    {
    case CHART_CYCLE_TEN_MINUTE:
        startDate = currDate.addSecs(-(60 * 10));
        m_xFormat = "hh:mm";
        m_xStart = startDate;
        m_xEnd = currDate;
        m_xInterval = 1;
        break;
    case CHART_CYCLE_ONE_HOUR:
        startDate = currDate.addSecs(-(60 * 60));
        m_xFormat = "hh:mm";
        m_xStart = startDate;
        m_xEnd = currDate;
        m_xInterval = 5;
        break;
    case CHART_CYCLE_ONE_DAY:
        startDate = currDate.addSecs(-(24 * 60 * 60));
        m_xFormat = "dd:hh:mm";
        m_xStart = startDate;
        m_xEnd = currDate;
        m_xInterval = 2 * 60;
        break;
    case CHART_CYCLE_ONE_WEEK:
        startDate = currDate.addSecs(-(7 * 24 * 60 * 60));
        m_xFormat = "MM:dd:hh:mm";
        m_xStart = startDate;
        m_xEnd = currDate;
        m_xInterval = 12 * 60;
        break;
    case CHART_CYCLE_ONE_MONTH:
        startDate = currDate.addSecs(-(30 * 24 * 60 * 60));
        m_xFormat = "yy:MM:dd:hh:mm";
        m_xStart = startDate;
        m_xEnd = currDate;
        m_xInterval = 2 * 24 * 60;
        break;
    case CHART_CYCLE_CUSTOM:
        startDate = currDate.addSecs(-(30 * 24 * 60 * 60));
        m_xFormat = "yy:MM:dd:hh:mm";
        m_xStart = startDate;
        m_xEnd = currDate;
        m_xInterval = 2 * 24 * 60;
        break;
    default:
        break;
    }

    InfoWorker::getInstance().monitorHistory(m_nodeId, startDate.toTime_t(), currDate.toTime_t(), m_xInterval, m_containerId);
}

void Monitor::getMonitorHistoryResult(const QPair<grpc::Status, container::MonitorHistoryReply> &reply)
{
    KLOG_INFO() << "getMonitorHistoryResult";
    if (reply.first.ok())
    {
        ChartInfo chartInfo;
        chartInfo.xFormat = m_xFormat;
        chartInfo.xTitle = tr("Time particle density(%1 minute)").arg(m_xInterval);
        chartInfo.xStart = m_xStart;
        chartInfo.xEnd = m_xEnd;

        QList<QPointF> pointList;
        pointList.clear();
        chartInfo.yStart = reply.second.cpu_usage(0).value();
        chartInfo.yEnd = reply.second.cpu_usage(0).value();
        KLOG_INFO() << reply.second.cpu_usage_size();
        KLOG_INFO() << reply.second.cpu_usage(0).value();

        for (auto i : reply.second.cpu_usage())
        {
            KLOG_INFO() << i.timestamp() << i.value();
            QDateTime stempToPos = QDateTime::fromTime_t(i.timestamp());
            QPointF point(stempToPos.toMSecsSinceEpoch(), i.value());
            pointList.append(point);

            chartInfo.yStart = i.value() < chartInfo.yStart ? i.value() : chartInfo.yStart;
            chartInfo.yEnd = i.value() > chartInfo.yEnd ? i.value() : chartInfo.yEnd;
        }
        m_cpuChartForm->updateChart(chartInfo);
        m_cpuChartForm->setData(pointList, CHART_SERIES_NAME_CPU);

        pointList.clear();
        chartInfo.yStart = reply.second.memory_usage(0).value();
        chartInfo.yEnd = reply.second.memory_usage(0).value();
        KLOG_INFO() << reply.second.memory_usage_size();
        KLOG_INFO() << reply.second.memory_usage(0).value();

        for (auto i : reply.second.memory_usage())
        {
            KLOG_INFO() << i.timestamp() << i.value();
            QDateTime stempToPos = QDateTime::fromTime_t(i.timestamp());
            QPointF point(stempToPos.toMSecsSinceEpoch(), i.value());
            pointList.append(point);

            chartInfo.yStart = i.value() < chartInfo.yStart ? i.value() : chartInfo.yStart;
            chartInfo.yEnd = i.value() > chartInfo.yEnd ? i.value() : chartInfo.yEnd;
        }
        m_memoryChartForm->updateChart(chartInfo);
        m_memoryChartForm->setData(pointList, CHART_SERIES_NAME_MEMORY);

        //        for (auto i : reply.second.memory_usage())
        //        {
        //            KLOG_INFO() << i.timestamp() << i.value();
        //        }
        //        for (auto i : reply.second.disk_usage())
        //        {
        //            KLOG_INFO() << i.timestamp() << i.value();
        //        }
        //        for (auto i : reply.second.net_rx())
        //        {
        //            KLOG_INFO() << i.timestamp() << i.value();
        //        }
        //        for (auto i : reply.second.net_tx())
        //        {
        //            KLOG_INFO() << i.timestamp() << i.value();
        //        }
    }
    else
    {
        KLOG_INFO() << reply.first.error_message().data();
    }
}
