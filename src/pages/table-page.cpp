#include "table-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QHBoxLayout>
#include <QPainter>
#include <QTableView>
#include <QTime>
#include <QTimer>
#include <iostream>
#include "common/button-delegate.h"
#include "common/header-view.h"
#include "ui_table-page.h"

using namespace std;

#define TIMEOUT 200
TablePage::TablePage(QWidget *parent) : Page(parent),
                                        ui(new Ui::TablePage),
                                        m_searchTimer(nullptr),
                                        m_refreshBtnTimer(nullptr),
                                        m_singleChoose(false)
{
    ui->setupUi(this);
    initUI();

    m_searchTimer = new QTimer(this);
    connect(m_searchTimer, &QTimer::timeout,
            [this] {
                search();
                m_searchTimer->stop();
            });
    m_refreshBtnTimer = new QTimer(this);
    connect(m_refreshBtnTimer, &QTimer::timeout, this, &TablePage::onRefreshTimeout);
}

TablePage::~TablePage()
{
    delete ui;
    if (m_searchTimer)
    {
        delete m_searchTimer;
        m_searchTimer = nullptr;
    }
    if (m_refreshBtnTimer)
    {
        delete m_refreshBtnTimer;
        m_refreshBtnTimer = nullptr;
    }
}

void TablePage::clearTable()
{
    KLOG_INFO() << "pre" << m_model->rowCount();
    m_model->removeRows(0, m_model->rowCount());
    KLOG_INFO() << "current" << m_model->rowCount();
}

void TablePage::addSingleOperationButton(QAbstractButton *btn)
{
    ui->hLayout_OpBtns->addWidget(btn, Qt::AlignLeft);
    m_singleOpBtns.append(btn);
}

void TablePage::addBatchOperationButtons(QList<QPushButton *> opBtns)
{
    foreach (QPushButton *btn, opBtns)
    {
        ui->hLayout_OpBtns->addWidget(btn, Qt::AlignLeft);
        m_batchOpBtns.append(btn);
    }
}

void TablePage::setOpBtnEnabled(OperatorButtonType type, bool enabled)
{
    if (type == OPERATOR_BUTTON_TYPE_BATCH)
    {
        foreach (QAbstractButton *btn, m_batchOpBtns)
        {
            btn->setEnabled(enabled);
        }
    }
    else if (type == OPERATOR_BUTTON_TYPE_SINGLE)
    {
        foreach (QAbstractButton *btn, m_singleOpBtns)
        {
            btn->setEnabled(enabled);
        }
    }
}

void TablePage::setTableRowNum(int num)
{
    m_model->setRowCount(num);
}

void TablePage::setTableColNum(int num)
{
    m_model->setColumnCount(num);
}

void TablePage::setTableItem(int row, int col, QStandardItem *item)
{
    m_model->setItem(row, col, item);
    adjustTableSize();
}

void TablePage::setTableItems(int row, int col, QList<QStandardItem *> items)
{
    for (int i = 0; i < items.size(); i++)
    {
        m_model->setItem(row, i, items.at(i));
        items.at(i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    }
    adjustTableSize();
}

void TablePage::setTableActions(int col, QStringList actionIcons)
{
    //设置表中操作按钮代理
    QMap<int, QString> btnInfo;
    for (int i = 0; i < actionIcons.size(); i++)
    {
        btnInfo.insert(i, actionIcons.at(i));
    }
    ButtonDelegate *btnDelegate = new ButtonDelegate(btnInfo, this);
    ui->tableView->setItemDelegateForColumn(col, btnDelegate);

    connect(btnDelegate, &ButtonDelegate::sigMonitor, this, &TablePage::onMonitor);
    connect(btnDelegate, &ButtonDelegate::sigEdit, this, &TablePage::onEdit);
    connect(btnDelegate, &ButtonDelegate::sigTerminal, this, &TablePage::onTerminal);
    connect(btnDelegate, &ButtonDelegate::sigActRun, this, &TablePage::onActRun);
    connect(btnDelegate, &ButtonDelegate::sigActStop, this, &TablePage::onActStop);
    connect(btnDelegate, &ButtonDelegate::sigActRestart, this, &TablePage::onActRestart);
}

void TablePage::setTableSingleChoose(bool isSingleChoose)
{
    m_singleChoose = isSingleChoose;
}

void TablePage::setSortableCol(QList<int> cols)
{
    m_headerView->setSortableCols(cols);
}

void TablePage::setHeaderSections(QStringList names)
{
    //插入表头数据
    for (int i = 0; i < names.size(); i++)
    {
        QStandardItem *headItem = new QStandardItem(names.at(i));
        headItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        m_model->setHorizontalHeaderItem(i, headItem);
    }
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(names.size() - 1, QHeaderView::Fixed);

    //设置列宽度
    for (int i = 0; i < names.size(); i++)
    {
        ui->tableView->setColumnWidth(i + 1, 150);
    }
    ui->tableView->setColumnWidth(0, 35);
}

void TablePage::setHeaderCheckable(bool checkable)
{
    m_headerView->setCheckable(checkable);
}

void TablePage::setTableDefaultContent(QString text)
{
    m_model->removeRows(0, m_model->rowCount());
    for (int i = 1; i < m_model->columnCount(); i++)
    {
        QStandardItem *item = new QStandardItem(text);
        item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        m_model->setItem(0, i, item);
    }
}

void TablePage::clearText()
{
    ui->label_search_tips->clear();
    ui->lineEdit_search->clear();
}

int TablePage::getTableRowCount()
{
    return m_model->rowCount();
}

QStandardItem *TablePage::getItem(int row, int col)
{
    return m_model->item(row, col);
}

QList<QMap<QString, QVariant>> TablePage::getCheckedItemInfo(int col)
{
    QList<QMap<QString, QVariant>> checkedItemInfo;  //containerId,nodeId
    for (int i = 0; i < m_model->rowCount(); i++)
    {
        auto item = m_model->item(i, col);
        if (item->checkState() == Qt::CheckState::Checked)
        {
            auto infoItem = m_model->item(i, col + 1);
            QMap<QString, QVariant> infoMap = infoItem->data().value<QMap<QString, QVariant>>();
            checkedItemInfo.append(infoMap);
        }
    }
    return checkedItemInfo;
}

void TablePage::sleep(int sec)
{
    QTime dieTime = QTime::currentTime().addSecs(sec);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void TablePage::initUI()
{
    ui->lineEdit_search->setPlaceholderText(tr("Please enter the keyword"));
    ui->btn_refresh->setIcon(QIcon(":/images/refresh.svg"));
    ui->btn_refresh->installEventFilter(this);

    QHBoxLayout *layout = new QHBoxLayout(ui->lineEdit_search);
    layout->setMargin(0);
    layout->setContentsMargins(10, 0, 10, 0);

    QPushButton *btn_search = new QPushButton(ui->lineEdit_search);
    btn_search->setObjectName("btn_search");
    btn_search->setFixedSize(QSize(16, 16));
    btn_search->setIcon(QIcon(":/images/search.svg"));
    btn_search->setStyleSheet("#btn_search{background:transparent;border:none;}"
                              "#btn_search:focus{outline:none;}");
    btn_search->setCursor(Qt::PointingHandCursor);
    layout->addStretch();
    layout->addWidget(btn_search);
    ui->lineEdit_search->setTextMargins(10, 0, btn_search->width() + 10, 0);

    m_model = new QStandardItemModel(this);
    ui->tableView->setModel(m_model);

    //设置表头
    m_headerView = new HeaderView(true, ui->tableView);
    m_headerView->setStretchLastSection(true);
    ui->tableView->setHorizontalHeader(m_headerView);

    //隐藏列表头
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setDefaultSectionSize(60);

    //设置表的其他属性
    ui->tableView->setMouseTracking(true);
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setFocusPolicy(Qt::NoFocus);
    ui->tableView->setShowGrid(false);

    connect(ui->tableView, &QTableView::clicked, this, &TablePage::onItemClicked);
    connect(m_model, &QStandardItemModel::itemChanged, this, &TablePage::onItemChecked);
    connect(btn_search, &QPushButton::clicked, this, &TablePage::search);
    connect(m_headerView, &HeaderView::ckbToggled, this, &TablePage::onHeaderCkbTog);
    connect(ui->btn_refresh, &QToolButton::clicked, this, &TablePage::refresh);
    connect(ui->lineEdit_search, &QLineEdit::textChanged,
            [this](QString text) {
                m_searchTimer->start(TIMEOUT);
            });
}

void TablePage::adjustTableSize()
{
    int height = 0;
    height = m_model->rowCount() * 60 + 40 + 20;  // row height+ header height + space
    ui->tableView->setFixedHeight(height);
    emit sigTableHeightChanged(height);
}

int TablePage::getCheckedItemNum()
{
    int count = 0;
    for (int i = 0; i < m_model->rowCount(); i++)
    {
        auto item = m_model->item(i, 0);
        if (item->checkState() == Qt::CheckState::Checked)
        {
            count++;
        }
    }
    return count;
}

bool TablePage::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->btn_refresh && event->type() == QEvent::HoverEnter)
    {
        ui->btn_refresh->setIcon(QIcon(":/images/refresh-hover.svg"));
        return true;
    }
    else if (watched == ui->btn_refresh && event->type() == QEvent::HoverLeave)
    {
        ui->btn_refresh->setIcon(QIcon(":/images/refresh.svg"));
        return true;
    }
    return false;
}

void TablePage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void TablePage::onMonitor(int row)
{
    KLOG_INFO() << "TablePage::onMonitor" << row;
    emit sigMonitor(row);
}

void TablePage::onTerminal(int row)
{
    KLOG_INFO() << "TablePage::onTerminal" << row;
    emit sigTerminal(row);
}

void TablePage::onEdit(int row)
{
    KLOG_INFO() << "TablePage::onEdit" << row;
    emit sigEdit(row);
}

void TablePage::onActRun(QModelIndex index)
{
    KLOG_INFO() << index.row();
    emit sigRun(index);
}

void TablePage::onActStop(QModelIndex index)
{
    KLOG_INFO() << index.row();
    emit sigStop(index);
}

void TablePage::onActRestart(QModelIndex index)
{
    KLOG_INFO() << index.row();
    emit sigRestart(index);
}

void TablePage::onRefreshTimeout()
{
    static int count = 0;
    count++;
    QPixmap pix(":/images/refresh-hover.svg");
    static int rat = 0;
    rat = rat >= 180 ? 30 : rat + 30;
    int imageWidth = pix.width();
    int imageHeight = pix.height();
    QPixmap temp(pix.size());
    temp.fill(Qt::transparent);
    QPainter painter(&temp);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.translate(imageWidth / 2, imageHeight / 2);        //让图片的中心作为旋转的中心
    painter.rotate(rat);                                       //顺时针旋转90度
    painter.translate(-(imageWidth / 2), -(imageHeight / 2));  //使原点复原
    painter.drawPixmap(0, 0, pix);
    painter.end();
    ui->btn_refresh->setIcon(QIcon(temp));

    if (count == 6)
    {
        m_refreshBtnTimer->stop();
        ui->btn_refresh->setIcon(QIcon(":/images/refresh.svg"));
        count = 0;
    }
}

void TablePage::search()
{
    KLOG_INFO() << "search....";
    auto resultCount = 0;
    QString text = ui->lineEdit_search->text();
    if (text.isEmpty())
        updateInfo();

    else
    {
        //show keyword row
        int rowCounts = m_model->rowCount();
        for (int i = 0; i < rowCounts; i++)
        {
            QStandardItem *item = m_model->item(i, 1);
            if (!item->text().contains(text))
            {
                ui->tableView->setRowHidden(i, true);
            }
            else
            {
                ui->tableView->showRow(i);
                resultCount++;
            }
        }
        if (resultCount == 0)
        {
            ui->label_search_tips->setText(tr("No search results were found!"));
            ui->tableView->setFixedHeight(120);
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
            return;
        }
        //sort
        ui->tableView->sortByColumn(0);
        ui->label_search_tips->clear();
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, true);
        adjustTableSize();
    }
}

void TablePage::refresh()
{
    m_refreshBtnTimer->start(100);
    //更新列表信息
    updateInfo();
}

void TablePage::onItemChecked(QStandardItem *changeItem)
{
    if (changeItem)
    {
        if (changeItem->isCheckable())
        {
            if (m_singleChoose)
            {
                if (changeItem->checkState() == Qt::Checked)
                {
                    for (int i = 0; i < m_model->rowCount(); i++)
                    {
                        auto item = m_model->item(i, 0);
                        if (changeItem != item && item->checkState() == Qt::CheckState::Checked)
                        {
                            item->setCheckState(Qt::Unchecked);
                        }
                    }
                }
            }

            if (getCheckedItemNum() > 0)
                setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, true);
            else
                setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
        }
    }
}

void TablePage::onItemClicked(const QModelIndex &index)
{
    emit sigItemClicked(index);
}

void TablePage::onHeaderCkbTog(bool toggled)
{
    int rowCounts = m_model->rowCount();
    KLOG_INFO() << "onHeaderCkbTog" << rowCounts;

    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, toggled);

    for (int i = 0; i < rowCounts; i++)
    {
        QStandardItem *item = m_model->item(i, 0);
        if (item)
        {
            if (toggled)
                item->setCheckState(Qt::Checked);
            else
                item->setCheckState(Qt::Unchecked);
        }
    }
}
