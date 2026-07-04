/**
 * @file          /ks-scmc-gui/src/pages/table-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "table-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QHBoxLayout>
#include <QPainter>
#include <QProxyStyle>
#include <QTableView>
#include <QTime>
#include <QTimer>
#include <QToolTip>
#include <iostream>
#include "common/button-delegate.h"
#include "common/header-view.h"
#include "info-worker.h"
#include "ui_table-page.h"

using namespace std;

#define TIMEOUT 200

TablePage::TablePage(QWidget *parent, bool is_open) : Page(parent),
                                                      ui(new Ui::TablePage),
                                                      m_searchTimer(nullptr),
                                                      m_isHeadCheckable(false),
                                                      m_singleChoose(false),
                                                      m_isSetTableActions(false),
                                                      m_pageEdit(nullptr),
                                                      m_searchCol(1)
{
    ui->setupUi(this);
    m_isOpenPaging = is_open;
    initUI();
    m_searchTimer = new QTimer(this);
    connect(m_searchTimer, &QTimer::timeout,
            [this] {
                search();
                m_searchTimer->stop();
            });
    if (m_isOpenPaging)
        initPaging();
    //    setPaging(1,is_open);
}

TablePage::~TablePage()
{
    delete ui;
    if (m_searchTimer)
    {
        delete m_searchTimer;
        m_searchTimer = nullptr;
    }
}

void TablePage::clearTable()
{
    m_model->removeRows(0, m_model->rowCount());
}

void TablePage::setOperateAreaVisible(bool visible)
{
    ui->widget_operate->setVisible(visible);
}

void TablePage::setOperateBtnsVisible(bool visible)
{
    ui->operate_btns->setVisible(visible);
}

void TablePage::setOperateSearchVisible(bool visible)
{
    ui->lineEdit_search->setVisible(visible);
    ui->btn_refresh->setVisible(visible);
}

void TablePage::addSingleOperationButton(QAbstractButton *btn)
{
    ui->operate_btns->layout()->addWidget(btn);
    m_singleOpBtns.append(btn);
}

void TablePage::addSingleOperationWidget(QWidget *btnwidget)
{
    ui->operate_btns->layout()->addWidget(btnwidget);
}

void TablePage::addBatchOperationButtons(QList<QPushButton *> opBtns)
{
    foreach (QPushButton *btn, opBtns)
    {
        ui->operate_btns->layout()->addWidget(btn);
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
}

void TablePage::setTableItems(int row, int col, QList<QStandardItem *> items)
{
    for (int i = 0; i < items.size(); i++)
    {
        m_model->setItem(row, i, items.at(i));
        items.at(i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    }
    //adjustTableSize();
}

void TablePage::setTableActions(int col, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>> btnInfo)
{
    //设置表中操作按钮代理
    m_btnDelegate = new ButtonDelegate(btnInfo, this);
    m_btnDelegate->isSetDelegateDefault(false);
    ui->tableView->setItemDelegateForColumn(col, m_btnDelegate);
    m_isSetTableActions = true;

    connect(m_btnDelegate, &ButtonDelegate::sigBackupResume, this, &TablePage::onActBackupResume);
    connect(m_btnDelegate, &ButtonDelegate::sigBackupUpdate, this, &TablePage::onActBackupUpdate);
    connect(m_btnDelegate, &ButtonDelegate::sigBackupRemove, this, &TablePage::onActBackupRemove);
    connect(m_btnDelegate, &ButtonDelegate::sigBackupExport, this, &TablePage::sigBackupExport);
    connect(m_btnDelegate, &ButtonDelegate::sigApp, this, &TablePage::sigApp);
    connect(m_btnDelegate, &ButtonDelegate::sigAppRun, this, &TablePage::sigAppRun);
    connect(m_btnDelegate, &ButtonDelegate::sigAppStop, this, &TablePage::sigAppStop);
    connect(m_btnDelegate, &ButtonDelegate::sigMonitor, this, &TablePage::onMonitor);
    connect(m_btnDelegate, &ButtonDelegate::sigEdit, this, &TablePage::onEdit);
    connect(m_btnDelegate, &ButtonDelegate::sigTerminal, this, &TablePage::onTerminal);
    connect(m_btnDelegate, &ButtonDelegate::sigdelete, this, &TablePage::onDelete);
    connect(m_btnDelegate, &ButtonDelegate::sigActRun, this, &TablePage::onActRun);
    connect(m_btnDelegate, &ButtonDelegate::sigActStop, this, &TablePage::onActStop);
    connect(m_btnDelegate, &ButtonDelegate::sigActGenerateTemp, this, &TablePage::sigGenerateTemp);
    connect(m_btnDelegate, &ButtonDelegate::sigActRestart, this, &TablePage::onActRestart);
    connect(m_btnDelegate, &ButtonDelegate::sigImagePass, this, &TablePage::sigImagePass);
    connect(m_btnDelegate, &ButtonDelegate::sigImageRefuse, this, &TablePage::sigImageRefuse);
    connect(m_btnDelegate, &ButtonDelegate::sigWarnRead, this, &TablePage::sigWarnRead);
    connect(m_btnDelegate, &ButtonDelegate::sigWarnIgnore, this, &TablePage::sigWarnIgnore);
}

void TablePage::setTableSingleChoose(bool isSingleChoose)
{
    m_singleChoose = isSingleChoose;
}

void TablePage::setSortableCols(QList<int> cols)
{
    m_headerView->setSortableCols(cols);
}

void TablePage::setSortableCol(int col, Qt::SortOrder order)
{
    ui->tableView->sortByColumn(col, order);
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
    ui->tableView->setColumnWidth(0, 30);
}

void TablePage::setHeaderCheckable(bool checkable)
{
    m_isHeadCheckable = checkable;
    m_headerView->setCheckable(checkable);
}

void TablePage::setTableDefaultContent(QString text)
{
    m_model->removeRows(0, m_model->rowCount());
    //    auto colCount = m_isSetTableActions ? m_model->columnCount() - 1 : m_model->columnCount();
    //    //    if(m_isSetTableActions == true)
    //    //        m_btnDelegate->isSetDelegateDefault(true);

    //    for (int i = 1; i < colCount; i++)
    //    {
    //        QStandardItem *item = new QStandardItem(text);
    //        item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    //        m_model->setItem(0, i, item);
    //    }
}

void TablePage::setSearchableCol(int col)
{
    m_searchCol = col;
}

void TablePage::setTips(QString text)
{
    if (isBusy())
        setBusy(false);
    ui->label_tips->setText(text);
    m_timerID = this->startTimer(10000);  //10秒后提示消失
}

void TablePage::clearText()
{
    ui->label_tips->clear();
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
        auto item = m_model->item(i, 0);
        if (item)
        {
            if (item->checkState() == Qt::CheckState::Checked)
            {
                auto infoItem = m_model->item(i, col);
                QMap<QString, QVariant> infoMap = infoItem->data().value<QMap<QString, QVariant>>();
                checkedItemInfo.append(infoMap);
            }
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

void TablePage::clearCheckState()
{
    if (m_isHeadCheckable)
    {
        m_headerView->setCheckState(false);
        onHeaderCkbTog(false);
    }
}

void TablePage::setPaging(int totalPages)
{
    if (m_isOpenPaging)
    {
        //        initPaging(totalPages);
        m_totalPages = totalPages;
        m_totalPageLabel->setText(QString("/ ") + QString::number(m_totalPages));
        if (m_pageEdit->text().toInt() > m_totalPages)
            m_pageEdit->setText(QString::number(m_totalPages));

        if (m_totalPages <= 1)
        {
            for (int i = 0; i < m_pagingHlayout->count(); ++i)
            {
                QWidget *w = m_pagingHlayout->itemAt(i)->widget();
                if (w != nullptr)
                    w->setVisible(false);
            }
        }
        else
        {
            for (int i = 0; i < m_pagingHlayout->count(); ++i)
            {
                QWidget *w = m_pagingHlayout->itemAt(i)->widget();
                if (w != nullptr)
                    w->setVisible(true);
            }
        }
        //        updatePaging();
    }
}

void TablePage::updatePaging(int page_no)
{
    emit sigUpdatePaging(page_no);
    m_pageEdit->setText(QString::number(page_no));
}

//QString TablePage::getSearchKey()
//{
//    return ui->lineEdit_search->text();
//}

void TablePage::setSeachPageNone()
{
    ui->label_tips->setText(tr("No search results were found!"));
    //ui->tableView->setFixedHeight(120);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
    if (m_isHeadCheckable)
        m_headerView->setCheckable(false);
}

void TablePage::hideSearchLine()
{
    ui->lineEdit_search->setVisible(false);
}

void TablePage::initUI()
{
    setMaskParent(this);

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
    //    ui->tableView->setMouseTracking(true);
    //对鼠标进行监控
    this->setMouseTracking(true);

    connect(ui->tableView, &QTableView::clicked, this, &TablePage::onItemClicked);
    connect(ui->tableView, &QTableView::entered, this, &TablePage::onItemEntered);
    connect(m_model, &QStandardItemModel::itemChanged, this, &TablePage::onItemChecked);
    connect(btn_search, &QPushButton::clicked, this, &TablePage::search);
    connect(m_headerView, &HeaderView::ckbToggled, this, &TablePage::onHeaderCkbTog);
    connect(ui->btn_refresh, &QToolButton::clicked, this, &TablePage::refresh);
    connect(ui->lineEdit_search, &QLineEdit::returnPressed, this, &TablePage::search);
    if (!m_isOpenPaging)
        connect(ui->lineEdit_search, &QLineEdit::textChanged,
                [this](QString text) {
                    m_searchTimer->start(TIMEOUT);
                });
}

void TablePage::initPaging()
{
    //上一页
    QPushButton *last_btn = new QPushButton(tr("last page"), this);
    //    last_btn->setStyleSheet("QPushButton{ border-width:1px;border-radius:8px;font-size:14px;\
//                            border:1px solid #393939;}"
    //                            "QPushButton:focus{border:1px solid #3EB3FF;}");
    connect(last_btn, &QPushButton::clicked, this, &TablePage::lastBtnClick);
    //编辑框
    m_pageEdit = new QLineEdit(QString::number(1), this);
    m_pageEdit->setFixedSize(40, 30);
    m_pageEdit->setAlignment(Qt::AlignCenter);
    //    m_pageEdit->setContentsMargins(10,0,0,0);
    m_pageEdit->setStyleSheet("QLineEdit{ border-width:1px;border-radius:10px;font-size:12px;\
                              border:1px solid #393939;}"
                              "QLineEdit:focus{border:1px solid #3EB3FF;}");

    connect(m_pageEdit, &QLineEdit::returnPressed, this, &TablePage::pageEditChage);

    //总页数
    m_totalPageLabel = new QLabel(QString("/ ") + QString::number(m_totalPages), this);
    m_totalPageLabel->setFixedSize(50, 30);
    m_totalPageLabel->setContentsMargins(10, 0, 0, 0);
    //    m_totalPageLabel->setStyleSheet("QLabel{ border-width:1px;border-radius:8px;font-size:14px;\
//                                    border:1px solid #393939;}"
    //                                    "QLabel:focus{border:1px solid #3EB3FF;}");
    //下一页
    QPushButton *next_btn = new QPushButton(tr("next page"), this);
    //    next_btn->setStyleSheet("QPushButton{ border-width:1px;border-radius:8px;font-size:14px;\
//                            border:1px solid #393939;}"
    //                            "QPushButton:focus{border:1px solid #3EB3FF;}");
    connect(next_btn, &QPushButton::clicked, this, &TablePage::nextBtnClick);
    m_pagingHlayout = new QHBoxLayout();
    m_pagingHlayout->setSpacing(3);
    m_pagingHlayout->addStretch(1);
    m_pagingHlayout->addWidget(last_btn);
    m_pagingHlayout->addWidget(m_pageEdit);
    m_pagingHlayout->addWidget(m_totalPageLabel);
    m_pagingHlayout->addWidget(next_btn);
    m_pagingHlayout->addStretch(1);
    m_pagingHlayout->setContentsMargins(0, 0, 0, 0);

    ui->verticalLayout->addLayout(m_pagingHlayout);
}

///FIXME:暂时注销，后续需要再修改
void TablePage::adjustTableSize()
{
    //    int tableHeight = m_model->rowCount() * 60 + 40;
    //    int tableAreaHeight = this->height() - 20 - 32 - ui->label_tips->height();

    //    KLOG_INFO() << "this->height= " << this->height() << "tips->height = " << ui->label_tips->height() << "tableAreaHeight  = " << tableAreaHeight;
    //    KLOG_INFO() << "tableHeight  = " << tableHeight;

    //    //height = m_model->rowCount() * 60 + 40 + 20 + 32;  // row height+ header height + space

    //    if (tableHeight <= tableAreaHeight)
    //        ui->tableView->viewport()->setFixedHeight(tableHeight);
    //    else
    //        ui->tableView->viewport()->setFixedHeight(tableAreaHeight);
    //emit sigTableHeightChanged(height);
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

QString TablePage::tooptipWordWrap(const QString &org)
{
    QString result;
    QFontMetrics fm(fontMetrics());
    int textWidthInPxs = fm.width(org);
    const int rear = org.length();
    int pre = 0, vernier = 1;
    unsigned int pickUpWidthPxs = 0;
    QString pickUp;
    unsigned int curLen = 0;
    double maxLength = width() / 3;

    if (textWidthInPxs <= maxLength)
    {
        result = org;
        return result;
    }

    while (vernier <= rear)
    {
        curLen = vernier - pre;
        pickUp = org.mid(pre, curLen);
        pickUpWidthPxs = fm.width(pickUp);
        if (pickUpWidthPxs >= maxLength)
        {
            result += pickUp + "\n";  // 插入换行符，或者使用<br/>标签
            pre = vernier;
            pickUpWidthPxs = 0;
        }
        ++vernier;
    }

    if (pickUpWidthPxs < maxLength && !pickUp.isEmpty())
    {
        result += pickUp;
    }

    return result;
}

void TablePage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void TablePage::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    QCursor cur = this->cursor();
    if (cur.shape() != Qt::ArrowCursor)
        this->setCursor(Qt::ArrowCursor);
}

void TablePage::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerID)
    {
        ui->label_tips->clear();
        killTimer(m_timerID);
    }
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

void TablePage::onDelete(int row)
{
    KLOG_INFO() << "TablePage::onDelete" << row;
    emit sigDelete(row);
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

void TablePage::onActBackupResume(int row)
{
    KLOG_INFO() << "TablePage::onActBackupResume" << row;
    emit sigBackupResume(row);
}

void TablePage::onActBackupUpdate(int row)
{
    KLOG_INFO() << "TablePage::onActBackupUpdate" << row;
    emit sigBackupUpdate(row);
}

void TablePage::onActBackupRemove(int row)
{
    KLOG_INFO() << "TablePage::onActBackupRemove" << row;
    emit sigBackupRemove(row);
}

void TablePage::search()
{
    KLOG_INFO() << "search....";
    auto resultCount = 0;
    QString text = ui->lineEdit_search->text();
    if (text.isEmpty())
    {
        if (m_isOpenPaging)
        {
            clearText();
            emit sigPagingSearch(text);
        }
        else
            updateInfo();
    }
    else
    {
        if (!m_isOpenPaging)
        {
            //show keyword row
            int rowCounts = m_model->rowCount();
            for (int i = 0; i < rowCounts; i++)
            {
                QStandardItem *item = m_model->item(i, m_searchCol);
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
                setSeachPageNone();
                return;
            }
            //sort
            ui->label_tips->clear();
            if (m_isHeadCheckable)
                m_headerView->setCheckable(true);
            //adjustTableSize();
        }
        else
        {
            // 分页搜索
            emit sigPagingSearch(text);
        }
    }
}

void TablePage::refresh()
{
    //刷新搜索结果
    if (m_isOpenPaging)
        emit sigRefreshSearchResult();
    //更新列表信息
    updateInfo();
}

void TablePage::onItemChecked(QStandardItem *changeItem)
{
    if (!changeItem)
        return;

    if (changeItem->isCheckable())
    {
        bool hasRunningCtn = false;
        for (int i = 0; i < m_model->rowCount(); i++)
        {
            //针对容器列表：获取列表中是否有正在运行的容器
            auto item = m_model->item(i, 0);
            auto nameItem = m_model->item(i, 1);
            if (nameItem)
            {
                auto infoMap = nameItem->data().value<QMap<QString, QVariant>>();
                if ((infoMap.value(CONTAINER_STATUS).toString() == "running" && item->checkState() == Qt::CheckState::Checked) ||
                    (infoMap.value(CONTAIENR_APP_IS_RUNNING).toBool() && item->checkState() == Qt::CheckState::Checked))
                {
                    hasRunningCtn = true;
                }
            }
            //若为单选，将其他选中复选框置为未选中
            if (m_singleChoose)
            {
                if (changeItem->checkState() == Qt::Checked)
                {
                    if (changeItem != item && item->checkState() == Qt::CheckState::Checked)
                    {
                        item->setCheckState(Qt::Unchecked);
                    }
                }
            }
        }

        //更新表头复选框状态、更新批量处理按钮状态
        int num = getCheckedItemNum();
        if (num > 0)
        {
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, true);
            emit sigHasRunningCtn(hasRunningCtn);

            if (num == m_model->rowCount())
                m_headerView->setCheckState(true);
            else
                m_headerView->setCheckState(false);
        }
        else
        {
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
            m_headerView->setCheckState(false);
        }
    }
}

void TablePage::onItemClicked(const QModelIndex &index)
{
    emit sigItemClicked(index);
}

void TablePage::onItemEntered(const QModelIndex &index)
{
    auto item = m_model->itemFromIndex(index);
    if (item)
    {
        QFontMetrics fm(fontMetrics());
        int textWidthInPxs = fm.width(item->text());
        if (textWidthInPxs > ui->tableView->columnWidth(index.column()))
        {
            QPoint point = QCursor::pos();
            QRect rect = QRect(point.x(), point.y(), 30, 10);
            QToolTip::showText(point, tooptipWordWrap(item->text()), ui->tableView, rect);
        }
        else
            QToolTip::hideText();
    }
    emit sigItemEntered(index);
}

void TablePage::onHeaderCkbTog(bool toggled)
{
    int rowCounts = m_model->rowCount();

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

            auto nameItem = m_model->item(item->row(), 1);
            auto infoMap = nameItem->data().value<QMap<QString, QVariant>>();
            if (infoMap.value(CONTAINER_STATUS).toString() == "running")
            {
                if (item->checkState() == Qt::Checked)
                    emit sigHasRunningCtn(true);
            }
        }
    }
}

void TablePage::lastBtnClick()
{
    int page = m_pageEdit->text().toInt();
    if (page <= 1)
        return;

    m_pageEdit->setText(QString::number(page - 1));
    emit sigUpdatePaging(page - 1);
}

void TablePage::nextBtnClick()
{
    int page = m_pageEdit->text().toInt();

    if (page + 1 <= m_totalPages)
    {
        m_pageEdit->setText(QString::number(page + 1));
        emit sigUpdatePaging(page + 1);
    }
}

void TablePage::pageEditChage()
{
    int page = m_pageEdit->text().toInt();
    if (page <= m_totalPages && page >= 1)
        updatePaging(page);
    else
    //        clearTable();
    {
        // 跳转至最后一页
        updatePaging(m_totalPages);
        // 设置输入框文本为最后一页
        m_pageEdit->setText(QString::number(m_totalPages));
    }
}
