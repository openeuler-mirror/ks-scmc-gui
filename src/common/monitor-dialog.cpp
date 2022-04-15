#include "monitor-dialog.h"
#include <kiran-log/qt5-log-i.h>
#include <QTime>
#include <cmath>
#include "date-picker/date-picker.h"
#include "flowlayout.h"
#include "ui_monitor-dialog.h"

#define CHART_SERIES_NAME_CPU "CPU Usage"
#define CHART_SERIES_NAME_MEMORY "Memory Usage"
#define CHART_SERIES_NAME_DISK "Disk Usage"
#define CHART_SERIES_NAME_NETWORK_RX "Network Receiving Speed"
#define CHART_SERIES_NAME_NETWORK_TX "Network Transmission Speed"

#define K_BITE 1024

MonitorDialog::MonitorDialog(int nodeId, std::string containerId, QWidget *parent) : QWidget(parent),
                                                                                     ui(new Ui::MonitorDialog),
                                                                                     m_flowLayout(nullptr),
                                                                                     m_nodeId(nodeId),
                                                                                     m_containerId(containerId),
                                                                                     m_cpuChartForm(nullptr),
                                                                                     m_memoryChartForm(nullptr),
                                                                                     m_diskChartForm(nullptr),
                                                                                     m_netChartForm(nullptr),
                                                                                     m_xInterval(1),
                                                                                     m_datePicker(nullptr)
{
    ui->setupUi(this);
    m_xFormat = "hh:mm";
    initUI();
    initChart();
}

MonitorDialog::~MonitorDialog()
{
    delete ui;
    if (m_datePicker)
    {
        delete m_datePicker;
        m_datePicker = nullptr;
    }
}

void MonitorDialog::initUI()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);

    ui->widget_date_selete->hide();
    ui->btn_apply->hide();
    QDate currDate = QDate::currentDate();
    ui->btn_end->setText(currDate.toString("yy-MM-dd"));
    ui->btn_start->setText(currDate.addDays(-7).toString("yy-MM-dd"));

    m_datePicker = new DatePicker;
    //m_datePicker->installEventFilter(this);
    connect(m_datePicker, &DatePicker::sigStartdateChange,
            [this](QString date) {
                ui->btn_start->setText(date);
            });
    connect(m_datePicker, &DatePicker::sigEndDateChange,
            [this](QString date) {
                ui->btn_end->setText(date);
            });

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
    ui->cb_select_cycle->addItem(tr("Custom"), CHART_CYCLE_CUSTOM);
    connect(ui->cb_select_cycle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MonitorDialog::onCycleChanged);

    connect(ui->btn_start, &DatePickButton::clicked, this, &MonitorDialog::popupStartDatePicker);
    connect(ui->btn_end, &DatePickButton::clicked, this, &MonitorDialog::popupEndDatePicker);
    connect(ui->btn_apply, &QPushButton::clicked, this, &MonitorDialog::applyDatePicker);
}

void MonitorDialog::initChart()
{
    BuildCharts(m_cpuChartForm, QStringList() << CHART_SERIES_NAME_CPU, tr("CPU usage (unit %)"));
    BuildCharts(m_memoryChartForm, QStringList() << CHART_SERIES_NAME_MEMORY, tr("Memory usage (unit %)"));
    BuildCharts(m_diskChartForm, QStringList() << CHART_SERIES_NAME_DISK, tr("Disk usage (unit M)"));
    BuildCharts(m_netChartForm, QStringList() << CHART_SERIES_NAME_NETWORK_RX << CHART_SERIES_NAME_NETWORK_TX, tr("Network usage (unit M)"));

    QDateTime currTime = QDateTime::currentDateTime();  //获取当前时间
    int currTimeStamp = currTime.toTime_t();            //将当前时间转为时间戳
    QDateTime startDate = currTime.addSecs(-(60 * 10));
    int startTimestamp = startDate.toTime_t();

    connect(&InfoWorker::getInstance(), &InfoWorker::monitorHistoryFinished, this, &MonitorDialog::getMonitorHistoryResult);
    InfoWorker::getInstance().monitorHistory(m_nodeId, startTimestamp, currTimeStamp, m_xInterval, m_containerId);  //10 minute
}

void MonitorDialog::BuildCharts(TrendChartForm *chartForm, QStringList seriesNames, QString yTitle)
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

int MonitorDialog::getbit(double num)
{
    if (num < 1)
        return 0;
    return 1 + getbit(num / 10);
}

void MonitorDialog::handleYValue(double &start, double &end, QString &unit)
{
    if (end < 0)
        unit = "M";
    else if (end <= 1)  //<1M
    {
        end = end * K_BITE;
        unit = "KB";
    }
    else if (end <= 1024)  //<1G
        unit = "M";
    else if (end < 1024 * 1024)  // <1t
    {
        end = end / K_BITE;
        unit = "G";
    }
    else
        unit = "G";

    if (end > 0)
    {
        int bit = getbit(end);
        KLOG_INFO() << "********" << bit;
        end = pow(10, bit);  //10 bit
        start = 0;
    }
    else
    {
        auto positiveStart = fabs(start);
        int startBit = getbit(positiveStart);
        positiveStart = pow(10, startBit);
        start = -positiveStart;
        end = 0;
    }
}

void MonitorDialog::onCycleChanged(int index)
{
    KLOG_INFO() << "onCycleChanged: " << index;
    ui->widget_date_selete->hide();
    ui->btn_apply->hide();
    int cycle = ui->cb_select_cycle->currentIndex();
    QDateTime currDate = QDateTime::currentDateTime();  //获取当前时间
    QDateTime startDate;
    if (cycle == CHART_CYCLE_CUSTOM)
    {
        ui->widget_date_selete->show();
        ui->btn_apply->show();
    }
    else
    {
        switch (cycle)
        {
        case CHART_CYCLE_TEN_MINUTE:
            startDate = currDate.addSecs(-(60 * 10));
            m_xFormat = "hh:mm";
            m_xStart = startDate;
            m_xEnd = currDate;
            m_xInterval = 1;
            m_xTitle = tr("Time particle density(%1 minute)").arg(m_xInterval);
            break;
        case CHART_CYCLE_ONE_HOUR:
            startDate = currDate.addSecs(-(60 * 60));
            m_xFormat = "hh:mm";
            m_xStart = startDate;
            m_xEnd = currDate;
            m_xInterval = 1;
            m_xTitle = tr("Time particle density(%1 minute)").arg(m_xInterval);
            break;
        case CHART_CYCLE_ONE_DAY:
            startDate = currDate.addDays(-1);
            m_xFormat = "dd hh:mm";
            m_xStart = startDate;
            m_xEnd = currDate;
            m_xInterval = 1 * 60;
            m_xTitle = tr("Time particle density(%1 hour)").arg(m_xInterval / 60);
            break;
        case CHART_CYCLE_ONE_WEEK:
            startDate = currDate.addDays(-7);
            m_xFormat = "dd hh:mm";
            m_xStart = startDate;
            m_xEnd = currDate;
            m_xInterval = 1 * 60;
            m_xTitle = tr("Time particle density(%1 hour)").arg(m_xInterval / 60);
            break;
        default:
            break;
        }
        InfoWorker::getInstance().monitorHistory(m_nodeId, startDate.toSecsSinceEpoch(), currDate.toSecsSinceEpoch(), m_xInterval, m_containerId);
    }
}

void MonitorDialog::popupStartDatePicker()
{
    QPoint point = ui->btn_start->mapToGlobal(QPoint(0, 0));
    m_datePicker->move(QPoint(point.x(), point.y() + 35));
    m_datePicker->showDatePicker(CALENDAR_TYPE_START);
    m_datePicker->show();
}

void MonitorDialog::popupEndDatePicker()
{
    QPoint point = ui->btn_end->mapToGlobal(QPoint(0, 0));
    m_datePicker->move(QPoint(point.x(), point.y() + 35));
    m_datePicker->showDatePicker(CALENDAR_TYPE_END);
    m_datePicker->show();
}

void MonitorDialog::applyDatePicker()
{
    m_xFormat = "dd hh:mm";
    m_xStart = m_datePicker->getStartDate();
    m_xEnd = m_datePicker->getEndDate();
    m_xInterval = 1 * 60;
    m_xTitle = tr("Time particle density(%1 hour)").arg(m_xInterval / 60);
    InfoWorker::getInstance().monitorHistory(m_nodeId, m_xStart.toSecsSinceEpoch(), m_xEnd.toSecsSinceEpoch(), m_xInterval, m_containerId);
}

void MonitorDialog::getMonitorHistoryResult(const QPair<grpc::Status, container::MonitorHistoryReply> &reply)
{
    KLOG_INFO() << "getMonitorHistoryResult";
    if (reply.first.ok())
    {
        auto cpuLimit = reply.second.rsc_limit().cpu_limit();
        auto memoryLimit = reply.second.rsc_limit().memory_limit();
        ChartInfo chartInfo;
        chartInfo.xFormat = m_xFormat;
        chartInfo.xTitle = m_xTitle;
        chartInfo.xStart = m_xStart;
        chartInfo.xEnd = m_xEnd;

        QList<QPointF> pointList;
        if (reply.second.cpu_usage_size() > 0)
        {
            ChartInfo cpuChartInfo = chartInfo;
            pointList.clear();

            for (auto i : reply.second.cpu_usage())
            {
                //KLOG_INFO() << i.timestamp() << i.value();
                QDateTime stempToPos = QDateTime::fromTime_t(i.timestamp());
                auto value = i.value() * 100;
                QPointF point(stempToPos.toMSecsSinceEpoch(), value);
                pointList.append(point);
            }
            cpuChartInfo.yStart = 0;
            cpuChartInfo.yEnd = 100 * cpuLimit;
            cpuChartInfo.yFormat = "%d%%";
            m_cpuChartForm->updateChart(cpuChartInfo);
            m_cpuChartForm->setData(pointList, CHART_SERIES_NAME_CPU);
        }

        if (reply.second.memory_usage_size() > 0)
        {
            pointList.clear();
            ChartInfo memoryChartInfo = chartInfo;

            for (auto i : reply.second.memory_usage())
            {
                //KLOG_INFO() << i.timestamp() << i.value();
                QDateTime stempToPos = QDateTime::fromTime_t(i.timestamp());
                auto value = i.value() / memoryLimit * 100;
                QPointF point(stempToPos.toMSecsSinceEpoch(), value);
                pointList.append(point);
            }
            memoryChartInfo.yStart = 0;
            memoryChartInfo.yEnd = 100;
            memoryChartInfo.yFormat = "%d%%";
            m_memoryChartForm->updateChart(memoryChartInfo);
            m_memoryChartForm->setData(pointList, CHART_SERIES_NAME_MEMORY);
        }

        if (reply.second.disk_usage_size() > 0)
        {
            pointList.clear();
            ChartInfo diskChartInfo = chartInfo;
            KLOG_INFO() << reply.second.disk_usage_size();
            KLOG_INFO() << reply.second.disk_usage(0).value();

            auto start = reply.second.disk_usage(0).value();
            auto end = start;
            for (auto i : reply.second.disk_usage())
            {
                start = i.value() < start ? i.value() : start;
                end = i.value() > end ? i.value() : end;
            }
            QString unit;
            handleYValue(start, end, unit);
            KLOG_INFO() << "******disk" << start << end << unit;
            diskChartInfo.yStart = start;
            diskChartInfo.yEnd = end;
            diskChartInfo.yTitle = tr("Disk IO(unit %1)").arg(unit);
            for (auto i : reply.second.disk_usage())
            {
                QDateTime stempToPos = QDateTime::fromTime_t(i.timestamp());
                auto value = i.value();
                if (unit == "KB")
                    value = value * K_BITE;
                else if (unit == "G")
                    value = value / K_BITE;
                QPointF point(stempToPos.toMSecsSinceEpoch(), value);
                pointList.append(point);
            }
            m_diskChartForm->updateChart(diskChartInfo);
            m_diskChartForm->setData(pointList, CHART_SERIES_NAME_DISK);
        }

        if (reply.second.net_rx_size() > 0 || reply.second.net_tx_size() > 0)
        {
            QList<QPointF> rxPointList;
            QList<QPointF> txPointList;
            ChartInfo netChartInfo = chartInfo;
            KLOG_INFO() << reply.second.net_rx_size();
            KLOG_INFO() << reply.second.net_rx(0).value();
            auto start = reply.second.net_rx(0).value();
            auto end = start;

            for (auto i : reply.second.net_rx())
            {
                start = i.value() < start ? i.value() : start;
                end = i.value() > end ? i.value() : end;
            }
            for (auto i : reply.second.net_tx())
            {
                start = i.value() < start ? i.value() : start;
                end = i.value() > end ? i.value() : end;
            }
            QString unit;
            handleYValue(start, end, unit);
            KLOG_INFO() << "******net" << start << end << unit;
            netChartInfo.yStart = start;
            netChartInfo.yEnd = end;
            netChartInfo.yTitle = tr("Network usage (unit %1)").arg(unit);
            for (auto i : reply.second.net_rx())
            {
                QDateTime stempToPos = QDateTime::fromTime_t(i.timestamp());
                auto value = i.value();
                if (unit == "KB")
                    value = value * K_BITE;
                else if (unit == "G")
                    value = value / K_BITE;
                QPointF point(stempToPos.toMSecsSinceEpoch(), value);
                rxPointList.append(point);
            }
            for (auto i : reply.second.net_tx())
            {
                QDateTime stempToPos = QDateTime::fromTime_t(i.timestamp());
                auto value = i.value();
                if (unit == "KB")
                    value = value * K_BITE;
                else if (unit == "G")
                    value = value / K_BITE;
                QPointF point(stempToPos.toMSecsSinceEpoch(), value);
                txPointList.append(point);
            }

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
