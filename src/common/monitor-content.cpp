#include "monitor-content.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QResizeEvent>
#include <QTime>
#include <cmath>
#include "date-picker/date-picker.h"
#include "flowlayout.h"
#include "ui_monitor-content.h"

#define CHART_SERIES_NAME_CPU QObject::tr("CPU Usage")
#define CHART_SERIES_NAME_MEMORY QObject::tr("Memory Usage")
#define CHART_SERIES_NAME_DISK QObject::tr("Disk Usage")
#define CHART_SERIES_NAME_NETWORK_RX QObject::tr("Network Receiving Speed")
#define CHART_SERIES_NAME_NETWORK_TX QObject::tr("Network Transmission Speed")

#define K_BITE 1024

MonitorContent::MonitorContent(QWidget *parent, int nodeId, std::string containerId) : QWidget(parent),
                                                                                       ui(new Ui::MonitorContent),
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
    initUI();
    initChart();

    m_objId = InfoWorker::getInstance().generateId(this);

    connect(&InfoWorker::getInstance(), &InfoWorker::monitorHistoryFinished, this, &MonitorContent::getMonitorHistoryResult, Qt::UniqueConnection);
}

MonitorContent::~MonitorContent()
{
    delete ui;
    if (m_datePicker)
    {
        delete m_datePicker;
        m_datePicker = nullptr;
    }
}

void MonitorContent::updateMonitorInfo(qint64 nodeId, std::string containerId)
{
    if (nodeId >= 0 && !QString::fromStdString(containerId).isEmpty())
    {
        m_nodeId = nodeId;
        m_containerId = containerId;
        InfoWorker::getInstance().monitorHistory(m_objId, m_nodeId, m_xStart.toSecsSinceEpoch(), m_xEnd.toSecsSinceEpoch(), m_xInterval, m_containerId);  //10 minute
    }
}

QSize MonitorContent::sizeHint() const
{
    return QSize(1630, 820);
}

void MonitorContent::initUI()
{
    setWindowModality(Qt::WindowModal);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_apply, Kiran::BUTTON_Default);
    ui->widget_forms->installEventFilter(this);
    ui->widget_date_selete->hide();
    ui->btn_apply->hide();
    ui->widget_forms->resize(QSize(1634, 832));
    QDate currDate = QDate::currentDate();
    ui->btn_end->setText(currDate.toString("yy-MM-dd"));
    ui->btn_start->setText(currDate.addDays(-7).toString("yy-MM-dd"));

    m_datePicker = new DatePicker;
    connect(m_datePicker, &DatePicker::sigStartdateChange,
            [this](QString date) {
                ui->btn_start->setText(date);
            });
    connect(m_datePicker, &DatePicker::sigEndDateChange,
            [this](QString date) {
                ui->btn_end->setText(date);
            });

    m_flowLayout = new FlowLayout(0, 10, 10);
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
    connect(ui->cb_select_cycle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MonitorContent::onCycleChanged);

    connect(ui->btn_start, &DatePickButton::clicked, this, &MonitorContent::popupStartDatePicker);
    connect(ui->btn_end, &DatePickButton::clicked, this, &MonitorContent::popupEndDatePicker);
    connect(ui->btn_apply, &QPushButton::clicked, this, &MonitorContent::applyDatePicker);
}

void MonitorContent::initChart()
{
    QMap<QString, QString> cpuSeriesInfo = {{CHART_SERIES_NAME_CPU, "#2eb3ff"}};
    BuildCharts(m_cpuChartForm, cpuSeriesInfo, tr("CPU usage (%)"));
    m_cpuChartForm->setLegendVisible(false);

    QMap<QString, QString> memorySeriesInfo = {{CHART_SERIES_NAME_MEMORY, "#2eb3ff"}};
    BuildCharts(m_memoryChartForm, memorySeriesInfo, tr("Memory usage (%)"));
    m_memoryChartForm->setLegendVisible(false);

    QMap<QString, QString> diskSeriesInfo = {{CHART_SERIES_NAME_DISK, "#2eb3ff"}};
    BuildCharts(m_diskChartForm, diskSeriesInfo, tr("Disk usage (unit M)"));
    m_diskChartForm->setLegendVisible(false);

    QMap<QString, QString> netSeriesInfo = {{CHART_SERIES_NAME_NETWORK_RX, "#2eb3ff"}, {CHART_SERIES_NAME_NETWORK_TX, "#F57900"}};
    BuildCharts(m_netChartForm, netSeriesInfo, tr("Network throughput (unit M)"));

    QDateTime currTime = QDateTime::currentDateTime();  //获取当前时间
    int currTimeStamp = currTime.toTime_t();            //将当前时间转为时间戳
    QDateTime startDate = currTime.addSecs(-(60 * 10));
    int startTimestamp = startDate.toTime_t();

    if (m_nodeId > 0)
        InfoWorker::getInstance().monitorHistory(m_objId, m_nodeId, startTimestamp, currTimeStamp, m_xInterval, m_containerId);  //10 minute
}

void MonitorContent::BuildCharts(TrendChartForm *chartForm, QMap<QString, QString> seriesinfo, QString yTitle)
{
    ChartInfo chartInfo;
    chartInfo.seriesInfo = seriesinfo;

    //初始化记录前10分钟数据，1分钟间隔
    QDateTime currTime = QDateTime::currentDateTime();  //获取当前时间
    QDateTime startDate = currTime.addSecs(-(60 * 10));
    m_xEnd = currTime;
    m_xStart = startDate;
    m_xTitle = tr("Time particle density(1 minute)");
    m_xFormat = "hh:mm";
    chartInfo.xStart = startDate;
    chartInfo.xEnd = currTime;
    chartInfo.xTitle = m_xTitle;
    chartInfo.xFormat = m_xFormat;
    chartInfo.yStart = 0;
    chartInfo.yEnd = 100;
    chartInfo.yTitle = yTitle;
    KLOG_INFO() << "BuildCharts: " << chartInfo.seriesInfo.firstKey();

    chartForm->initChart(chartInfo);
}

int MonitorContent::getbit(double num)
{
    if (num < 1)
        return 0;
    return 1 + getbit(num / 10);
}

void MonitorContent::handleYValue(double &start, double &end, QString &unit)
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
        start = -1 * positiveStart;
        end = 0;
    }
}

void MonitorContent::onCycleChanged(int index)
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
        InfoWorker::getInstance().monitorHistory(m_objId, m_nodeId, startDate.toSecsSinceEpoch(), currDate.toSecsSinceEpoch(), m_xInterval, m_containerId);
    }
}

void MonitorContent::popupStartDatePicker()
{
    QPoint point = ui->btn_start->mapToGlobal(QPoint(0, 0));
    m_datePicker->move(QPoint(point.x(), point.y() + 35));
    m_datePicker->showDatePicker(CALENDAR_TYPE_START);
    m_datePicker->show();
}

void MonitorContent::popupEndDatePicker()
{
    QPoint point = ui->btn_end->mapToGlobal(QPoint(0, 0));
    m_datePicker->move(QPoint(point.x(), point.y() + 35));
    m_datePicker->showDatePicker(CALENDAR_TYPE_END);
    m_datePicker->show();
}

void MonitorContent::applyDatePicker()
{
    m_xFormat = "dd hh:mm";
    m_xStart = m_datePicker->getStartDate();
    m_xEnd = m_datePicker->getEndDate();
    m_xInterval = 1 * 60;
    m_xTitle = tr("Time particle density(%1 hour)").arg(m_xInterval / 60);
    InfoWorker::getInstance().monitorHistory(m_objId, m_nodeId, m_xStart.toSecsSinceEpoch(), m_xEnd.toSecsSinceEpoch(), m_xInterval, m_containerId);
}

void MonitorContent::getMonitorHistoryResult(const QString objID, const QPair<grpc::Status, container::MonitorHistoryReply> &reply)
{
    KLOG_INFO() << "getMonitorHistoryResult" << m_objId << objID;
    if (m_objId == objID)
    {
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
                cpuChartInfo.yTitle = tr("CPU usage (%)");
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
                memoryChartInfo.yTitle = tr("Memory usage (%)");
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
                diskChartInfo.yFormat = "%d";
                diskChartInfo.yTitle = tr("Disk usage(unit %1)").arg(unit);
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
                netChartInfo.yFormat = "%d";
                netChartInfo.yTitle = tr("Network throughput (unit %1)").arg(unit);
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
}

bool MonitorContent::eventFilter(QObject *watched, QEvent *event)
{
    QResizeEvent *e = static_cast<QResizeEvent *>(event);
    if (watched == ui->widget_forms && e->type() == QEvent::Resize)
    {
        //KLOG_INFO() << e->size().width() << e->size().height();
        int height = m_flowLayout->heightForWidth(e->size().width());
        int w = e->size().width();
        if (height <= this->height())
            ui->widget_forms->resize(w, height);
        return true;
    }
    return false;
}
