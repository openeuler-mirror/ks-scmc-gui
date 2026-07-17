/**
 * @file          src/common/security-widgets/security-list.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "security-list.h"
#include <kiran-log/qt5-log-i.h>
#include "security-list-item.h"
SecurityList::SecurityList(QString itemTitle, QWidget *parent) : QWidget(parent),
                                                                 m_listWidget(nullptr),
                                                                 m_itemTitle(itemTitle)
{
    initUI();
    createItem(0);
}

SecurityList::~SecurityList()
{
}

void SecurityList::setSecurityInfos(QStringList infos)
{
    int count = 0;
    for (auto file : infos)
    {
        if (count > 0)
            createItem(count);

        auto listItem = m_listWidget->item(count);
        auto item = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(listItem));
        if (!item)
        {
            KLOG_ERROR() << "Failed to cast item widget at index: " << count;
            continue;
        }

        item->setInfo(file);
        count++;
    }
}

QStringList SecurityList::getSecurityInfos()
{
    QStringList infos;
    int row = 0;
    while (row < m_listWidget->count())
    {
        auto listItem = m_listWidget->item(row);
        auto item = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(listItem));
        if (item)
        {
            if (!item->getPathCorrect())
            {
                KLOG_INFO() << "There is error in path of file protection!";
                row++;
                continue;
            }

            auto path = item->getInfo();
            if (!path.isEmpty())
                infos.append(path);
        }

        row++;
    }
    return infos;
}

void SecurityList::setItemTitle(QString title)
{
    m_itemTitle = title;
}

void SecurityList::initUI()
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_listWidget = new QListWidget(this);
    m_listWidget->setFocusPolicy(Qt::NoFocus);
    m_listWidget->setFrameShape(QFrame::NoFrame);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout->addWidget(m_listWidget);
}

void SecurityList::createItem(int index)
{
    QListWidgetItem *newItem = nullptr;
    SecurityListItem *customItem = nullptr;
    QString itemText;

    newItem = new QListWidgetItem();

    int itemIndex = index + 1;
    itemText = tr("%1 %2").arg(m_itemTitle).arg(itemIndex);

    customItem = new SecurityListItem(itemText, m_listWidget);
    if (index == 0)
        customItem->setDeleteBtnVisible(false);

    connect(customItem, &SecurityListItem::sigAdd, this, &SecurityList::addItem);
    connect(customItem, &SecurityListItem::sigDelete, this, &SecurityList::deleteItem);

    m_listWidget->insertItem(index, newItem);
    m_listWidget->setItemWidget(newItem, customItem);

    newItem->setSizeHint(QSize(340, 62));
    m_listWidget->setGridSize(QSize(340, 68));
}

void SecurityList::addItem()
{
    auto item = qobject_cast<SecurityListItem *>(sender());
    if (!item)
        return;

    int row = 0;
    while (row < m_listWidget->count())
    {
        auto listItem = m_listWidget->item(row);
        if (m_listWidget->itemWidget(listItem) == item)
        {
            KLOG_DEBUG() << "Insert item in index:" << row;
            createItem(row + 1);
            break;
        }
        row++;
    }
    updateItem();
}

void SecurityList::deleteItem()
{
    // 第0行不能删除
    if (m_listWidget->count() == 1)
        return;

    auto item = qobject_cast<SecurityListItem *>(sender());
    if (!item)
        return;

    int row = 0;
    while (row < m_listWidget->count() && m_listWidget->count() > 1)
    {
        auto listItem = m_listWidget->item(row);
        if (m_listWidget->itemWidget(listItem) == item)
        {
            deleteOneItem(listItem);
            break;
        }
        row++;
    }
    updateItem();
}

void SecurityList::deleteOneItem(QListWidgetItem *listItem)
{
    if (listItem)
    {
        auto widget = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(listItem));
        if (widget)
        {
            // 删除前先断开信号连接
            disconnect(widget, &SecurityListItem::sigAdd, this, &SecurityList::addItem);
            disconnect(widget, &SecurityListItem::sigDelete, this, &SecurityList::deleteItem);
            delete widget;
            widget = nullptr;
        }

        delete listItem;
        listItem = nullptr;
    }
}

void SecurityList::updateItem()
{
    int row = 0;
    while (row < m_listWidget->count())
    {
        QString itemName;
        itemName = tr("%1 %2").arg(m_itemTitle).arg(row + 1);

        auto listItem = m_listWidget->item(row);
        auto item = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(listItem));
        if (item)
            item->updateItemName(itemName);

        row++;
    }
}

void SecurityList::clearItems()
{
    int count = m_listWidget->count();

    // 从最后一项开始删除，保留第 0 项（即第一项）,并清空其数据
    for (int i = count - 1; i >= 0; --i)
    {
        if (i == 0)
        {
            auto listItem = m_listWidget->item(i);
            auto item = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(listItem));
            item->setInfo("");
            continue;
        }

        auto deleteItem = m_listWidget->takeItem(i);
        deleteOneItem(deleteItem);
    }
}
