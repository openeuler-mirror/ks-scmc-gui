#include "monitor.h"
#include <kiran-log/qt5-log-i.h>
#include <QTime>
#include "flowlayout.h"
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
        startDate = currDate.addDays(-1);
        m_xFormat = "dd hh:mm";
        m_xStart = startDate;
        m_xEnd = currDate;
        m_xInterval = 2 * 60;
        break;
    case CHART_CYCLE_ONE_WEEK:
        startDate = currDate.addDays(-7);
        m_xFormat = "MM-dd hh:mm";
        m_xStart = startDate;
        m_xEnd = currDate;
        m_xInterval = 12 * 60;
        break;
    case CHART_CYCLE_ONE_MONTH:
        startDate = currDate.addDays(-30);
        m_xFormat = "yy-MM-dd hh:mm";
        m_xStart = startDate;
        m_xEnd = currDate;
        m_xInterval = 2 * 24 * 60;
        break;
    case CHART_CYCLE_CUSTOM:
        //        startDate = currDate.addDays();
        //        m_xFormat = "yy-MM-dd hh:mm";
        //        m_xStart = startDate;
        //        m_xEnd = currDate;
        //        m_xInterval = 2 * 24 * 60;
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
        if (reply.second.cpu_usage_size() > 0)
        {
            ChartInfo cpuChartInfo = chartInfo;
            pointList.clear();
            cpuChartInfo.yStart = reply.second.cpu_usage(0).value();
            cpuChartInfo.yEnd = reply.second.cpu_usage(0).value();
            KLOG_INFO() << reply.second.cpu_usage_size();
            KLOG_INFO() << reply.second.cpu_usage(0).value();

            for (auto i : reply.second.cpu_usage())
            {
                //KLOG_INFO() << i.timestamp() << i.value();
                QDateTime stempToPos = QDateTime::fromTime_t(i.timestamp());
                QPointF point(stempToPos.toMSecsSinceEpoch(), i.value());
                pointList.append(point);

                cpuChartInfo.yStart = i.value() < cpuChartInfo.yStart ? i.value() : cpuChartInfo.yStart;
                cpuChartInfo.yEnd = i.value() > cpuChartInfo.yEnd ? i.value() : cpuChartInfo.yEnd;
            }
            cpuChartInfo.yEnd = cpuChartInfo.yStart == cpuChartInfo.yEnd ? (cpuChartInfo.yStart + 10) : cpuChartInfo.yEnd;
            m_cpuChartForm->updateChart(cpuChartInfo);
            m_cpuChartForm->setData(pointList, CHART_SERIES_NAME_CPU);
        }

        if (reply.second.memory_usage_size() > 0)
        {
            pointList.clear();
            ChartInfo memoryChartInfo = chartInfo;
            memoryChartInfo.yStart = reply.second.memory_usage(0).value();
            memoryChartInfo.yEnd = reply.second.memory_usage(0).value();
            KLOG_INFO() << reply.second.memory_usage_size();
            KLOG_INFO() << reply.second.memory_usage(0).value();

            for (auto i : reply.second.memory_usage())
            {
                //KLOG_INFO() << i.timestamp() << i.value();
                QDateTime stempToPos = QDateTime::fromTime_t(i.timestamp());
                QPointF point(stempToPos.toMSecsSinceEpoch(), i.value());
                pointList.append(point);

                memoryChartInfo.yStart = i.value() < memoryChartInfo.yStart ? i.value() : memoryChartInfo.yStart;
                memoryChartInfo.yEnd = i.value() > memoryChartInfo.yEnd ? i.value() : memoryChartInfo.yEnd;
            }
            memoryChartInfo.yEnd = memoryChartInfo.yStart == memoryChartInfo.yEnd ? (memoryChartInfo.yStart + 10) : memoryChartInfo.yEnd;
            m_memoryChartForm->updateChart(memoryChartInfo);
            m_memoryChartForm->setData(pointList, CHART_SERIES_NAME_MEMORY);
        }

        if (reply.second.disk_usage_size() > 0)
        {
            pointList.clear();
            ChartInfo diskChartInfo = chartInfo;
            diskChartInfo.yStart = reply.second.disk_usage(0).value();
            diskChartInfo.yEnd = reply.second.disk_usage(0).value();
            KLOG_INFO() << reply.second.disk_usage_size();
            KLOG_INFO() << reply.second.disk_usage(0).value();

            for (auto i : reply.second.disk_usage())
            {
                //KLOG_INFO() << i.timestamp() << i.value();
                QDateTime stempToPos = QDateTime::fromTime_t(i.timestamp());
                QPointF point(stempToPos.toMSecsSinceEpoch(), i.value());
                pointList.append(point);

                diskChartInfo.yStart = i.value() < diskChartInfo.yStart ? i.value() : diskChartInfo.yStart;
                diskChartInfo.yEnd = i.value() > diskChartInfo.yEnd ? i.value() : diskChartInfo.yEnd;
            }
            diskChartInfo.yEnd = diskChartInfo.yStart == diskChartInfo.yEnd ? (diskChartInfo.yStart + 10) : diskChartInfo.yEnd;
            m_diskChartForm->updateChart(diskChartInfo);
            m_diskChartForm->setData(pointList, CHART_SERIES_NAME_DISK);
        }

        if (reply.second.net_rx_size() > 0)
        {
            QList<QPointF> rxPointList;
            QList<QPointF> txPointList;
            ChartInfo netChartInfo = chartInfo;
            netChartInfo.yStart = reply.second.net_rx(0).value();
            netChartInfo.yEnd = reply.second.net_rx(0).value();
            KLOG_INFO() << reply.second.net_rx_size();
            KLOG_INFO() << reply.second.net_rx(0).value();

            for (auto i : reply.second.net_rx())
            {
                //KLOG_INFO() << i.timestamp() << i.value();
                QDateTime stempToPos = QDateTime::fromTime_t(i.timestamp());
                QPointF point(stempToPos.toMSecsSinceEpoch(), i.value());
                rxPointList.append(point);

                netChartInfo.yStart = i.value() < netChartInfo.yStart ? i.value() : netChartInfo.yStart;
                netChartInfo.yEnd = i.value() > netChartInfo.yEnd ? i.value() : netChartInfo.yEnd;
            }
            for (auto i : reply.second.net_tx())
            {
                //KLOG_INFO() << i.timestamp() << i.value();
                QDateTime stempToPos = QDateTime::fromTime_t(i.timestamp());
                QPointF point(stempToPos.toMSecsSinceEpoch(), i.value());
                txPointList.append(point);
            }
            netChartInfo.yEnd = netChartInfo.yStart == netChartInfo.yEnd ? (netChartInfo.yStart + 10) : netChartInfo.yEnd;
            m_netChartForm->updateChart(netChartInfo);
            m_netChartForm->setData(rxPointList, CHART_SERIES_NAME_NETWORK_RX);
            m_netChartForm->setData(txPointList, CHART_SERIES_NAME_NETWORK_TX);
        }
    }
    else
    {
        KLOG_INFO() << reply.first.error_message().data();
    }
}
