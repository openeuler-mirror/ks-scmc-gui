/**
 * @file          src/pages/node/IPC/table-widgets/ipc-table.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma

#include "ipc-table.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QStandardItemModel>
#include <QTableView>
#include <QToolTip>
#include "message-dialog.h"

enum IpcTableField
{
    IPC_TABLE_FIELD_CHECKBOX,
    IPC_TABLE_FIELD_NAME,
    IPC_TABLE_FIELD_CONTAINERS,
    IPC_TABLE_FIELD_CREATE_TIME,
    IPC_TABLE_FIELD_LAST
};

IpcDelegate::IpcDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

IpcDelegate::~IpcDelegate()
{
}

void IpcDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();

    if (index.column() == IpcTableField::IPC_TABLE_FIELD_CHECKBOX)
    {
        auto checkboxOption = option;
        initStyleOption(&checkboxOption, index);

        QStyleOptionButton checkboxStyle;
        QPixmap pixmap;
        auto value = index.model()->data(index, Qt::EditRole).toBool();
        pixmap.load(value ? ":images/checked.svg" : ":images/check-default.svg");
        checkboxStyle.state = value ? QStyle::State_On : QStyle::State_Off;
        checkboxStyle.state |= QStyle::State_Enabled;
        checkboxStyle.iconSize = QSize(16, 16);
        checkboxStyle.rect = option.rect;
        checkboxStyle.rect.setX(option.rect.x() + 13);
        checkboxStyle.rect.setY(option.rect.x() + 12);

        const_cast<IpcDelegate *>(this)->m_checkboxRects[index] = checkboxStyle.rect;

        style->drawItemPixmap(painter, option.rect, Qt::AlignCenter, pixmap);
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, widget);
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

bool IpcDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    auto docorationRect = option.rect;
    auto mouseEvent = static_cast<QMouseEvent *>(event);

    if (event->type() == QEvent::MouseButtonPress &&
        docorationRect.contains(mouseEvent->pos()) &&
        index.column() == IpcTableField::IPC_TABLE_FIELD_CHECKBOX &&
        m_checkboxRects.contains(index))
    {
        auto value = model->data(index, Qt::EditRole).toBool();
        model->setData(index, !value, Qt::EditRole);
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

IpcFilterModel::IpcFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

bool IpcFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    for (auto i = 0; i < IPC_TABLE_FIELD_LAST; ++i)
    {
        auto index = sourceModel()->index(sourceRow, i, sourceParent);
        auto text = sourceModel()->data(index).toString();
        if (text.contains(filterRegExp()))
            return true;
    }

    return false;
}

IpcModel::IpcModel(QObject *parent)
{
}

int IpcModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_ipcsInfo.size();
}

int IpcModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return IpcTableField::IPC_TABLE_FIELD_LAST;
}

QVariant IpcModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_ipcsInfo.size() || index.column() >= IpcTableField::IPC_TABLE_FIELD_LAST)
    {
        KLOG_DEBUG() << "The index exceeds range limit.";
        return QVariant();
    }

    auto ipcInfo = m_ipcsInfo[index.row()];

    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (index.column())
        {
        case IpcTableField::IPC_TABLE_FIELD_NAME:
            return ipcInfo.ipcName;
        case IpcTableField::IPC_TABLE_FIELD_CONTAINERS:
            return ipcInfo.containers;
        case IpcTableField::IPC_TABLE_FIELD_CREATE_TIME:
            return ipcInfo.createTime;
        default:
            break;
        }
    }
    case Qt::EditRole:
    {
        switch (index.column())
        {
        case IpcTableField::IPC_TABLE_FIELD_CHECKBOX:
        {
            return ipcInfo.selected;
        }
        default:
            break;
        }
    }
    default:
        break;
    }

    return QVariant();
}

QVariant IpcModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Vertical)
    {
        return QVariant();
    }
    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (section)
        {
        case IpcTableField::IPC_TABLE_FIELD_NAME:
            return tr("IPC Name");
        case IpcTableField::IPC_TABLE_FIELD_CONTAINERS:
            return tr("Usage Containers");
        case IpcTableField::IPC_TABLE_FIELD_CREATE_TIME:
            return tr("Create Time");
        default:
            break;
        }
    }
    case Qt::EditRole:
    {
        switch (section)
        {
        case IpcTableField::IPC_TABLE_FIELD_CHECKBOX:
            return QVariant();
        }
    }
    default:
        break;
    }
    return QVariant();
}

bool IpcModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column() != IpcTableField::IPC_TABLE_FIELD_CHECKBOX)
    {
        return false;
    }

    m_ipcsInfo[index.row()].selected = value.toBool();

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags IpcModel::flags(const QModelIndex &index) const
{
    if (index.column() == IpcTableField::IPC_TABLE_FIELD_CHECKBOX)
    {
        return Qt::ItemFlag::ItemIsEnabled;
    }
    return Qt::ItemFlag::NoItemFlags;
}

QList<IpcInfo> IpcModel::getIpcInfos()
{
    return m_ipcsInfo;
}

void IpcModel::updateRecord(QList<IpcInfo> icpInfos)
{
    beginResetModel();

    m_ipcsInfo.clear();
    foreach (auto ipcInfo, icpInfos)
    {
        m_ipcsInfo.push_back(ipcInfo);
    }
    endResetModel();
}

IpcTable::IpcTable(QWidget *parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);

    // 设置Model
    m_model = new IpcModel(this);
    m_headerViewProxy = new CheckableHeader(this);
    setHorizontalHeader(m_headerViewProxy);

    m_filterProxy = new IpcFilterModel(this);
    m_filterProxy->setSourceModel(qobject_cast<QAbstractItemModel *>(m_model));
    setModel(m_filterProxy);

    // 在代理模型更新时，更新表头选中状态
    connect(m_filterProxy, &IpcFilterModel::dataChanged, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight)
            {
                if (topLeft.column() > IpcTableField::IPC_TABLE_FIELD_CHECKBOX ||
                    bottomRight.column() < IpcTableField::IPC_TABLE_FIELD_CHECKBOX)
                    return;

                updateHeaderState(); });

    connect(m_filterProxy, &IpcFilterModel::modelReset, [this]()
            { m_headerViewProxy->setCheckState(Qt::Unchecked); });

    connect(m_filterProxy, &QSortFilterProxyModel::rowsInserted, [this]()
            { updateHeaderState(); });

    connect(m_filterProxy, &QSortFilterProxyModel::rowsRemoved, [this]()
            { updateHeaderState(); });

    // 设置Delegate
    auto ipcDelegate = new IpcDelegate(this);
    setItemDelegate(ipcDelegate);

    // 设置水平行表头
    m_headerViewProxy->resizeSection(IpcTableField::IPC_TABLE_FIELD_CHECKBOX, 50);
    m_headerViewProxy->resizeSection(IpcTableField::IPC_TABLE_FIELD_NAME, 400);
    m_headerViewProxy->resizeSection(IpcTableField::IPC_TABLE_FIELD_CONTAINERS, 200);
    m_headerViewProxy->resizeSection(IpcTableField::IPC_TABLE_FIELD_CREATE_TIME, 400);
    m_headerViewProxy->setSectionResizeMode(IpcTableField::IPC_TABLE_FIELD_CHECKBOX, QHeaderView::Fixed);
    m_headerViewProxy->setStretchLastSection(true);
    m_headerViewProxy->setSectionsMovable(false);
    m_headerViewProxy->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_headerViewProxy->setFixedHeight(40);
    connect(m_headerViewProxy, &CheckableHeader::toggled, this, &IpcTable::checkedAllItem);
    connect(m_headerViewProxy, &CheckableHeader::checkStateChanged, this, &IpcTable::headerCheckStateChanged);

    // 设置表的其他属性
    setSelectionMode(QAbstractItemView::NoSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setFocusPolicy(Qt::NoFocus);
    setShowGrid(false);
    setColumnWidth(0, 50);

    // 隐藏列表头
    this->verticalHeader()->setVisible(false);
    this->verticalHeader()->setDefaultSectionSize(60);

    connect(this, &IpcTable::entered, this, &IpcTable::mouseEnter);
}

void IpcTable::searchTextChanged(const QString &text)
{
    KLOG_DEBUG() << "The search text is change to " << text;

    m_filterProxy->setFilterFixedString(text);
}

void IpcTable::setIpcInfos(QList<IpcInfo> ipcInfos)
{
    m_model->updateRecord(ipcInfos);
}

QList<IpcInfo> IpcTable::getIpcInfos()
{
    return m_model->getIpcInfos();
}

QList<IpcInfo> IpcTable::getSelectedIpcInfos()
{
    QList<IpcInfo> selectedIpcInfos;
    for (int i = 0; i < m_filterProxy->rowCount(); i++)
    {
        auto proxyIndex = m_filterProxy->index(i, IpcTableField::IPC_TABLE_FIELD_CHECKBOX);
        auto isSelected = m_filterProxy->data(proxyIndex, Qt::EditRole).toBool();
        if (isSelected)
        {
            auto sourceIndex = m_filterProxy->mapToSource(proxyIndex);
            selectedIpcInfos.append(m_model->getIpcInfos().at(sourceIndex.row()));
        }
    }
    return selectedIpcInfos;
}

void IpcTable::clearTable()
{
    m_model->removeRows(0, m_model->rowCount());
}

void IpcTable::mouseEnter(const QModelIndex &index)
{
    if (index.column() != IpcTableField::IPC_TABLE_FIELD_NAME)
    {
        return;
    }
    auto mod = selectionModel()->model()->data(index);
    QToolTip::showText(QCursor::pos(), mod.toString(), this, rect(), 2000);
}

void IpcTable::checkedAllItem(Qt::CheckState checkState)
{
    for (int i = 0; i < m_filterProxy->rowCount(); i++)
    {
        auto proxyIndex = m_filterProxy->index(i, IpcTableField::IPC_TABLE_FIELD_CHECKBOX);
        auto sourceIndex = m_filterProxy->mapToSource(proxyIndex);

        m_model->setData(sourceIndex, checkState == Qt::Checked, Qt::EditRole);
    }
}

void IpcTable::updateHeaderState()
{
    int checkedCount = 0;
    int checkableCount = 0;
    auto state = Qt::Unchecked;

    for (int i = 0; i < m_filterProxy->rowCount(); ++i)
    {
        checkableCount++;  // 可选行计数

        QModelIndex proxyCheckBoxIndex = m_filterProxy->index(i, IpcTableField::IPC_TABLE_FIELD_CHECKBOX);
        bool isChecked = m_filterProxy->data(proxyCheckBoxIndex, Qt::EditRole).toBool();

        if (isChecked)
        {
            checkedCount++;
        }
    }

    if (checkedCount >= checkableCount && checkedCount != 0)
    {
        state = Qt::Checked;
    }
    else if (checkedCount > 0)
    {
        state = Qt::PartiallyChecked;
    }

    m_headerViewProxy->setCheckState(state);
}
