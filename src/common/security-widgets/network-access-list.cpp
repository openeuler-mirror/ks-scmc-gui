/**
 * @file          src/common/security-widgets/network-access-list.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "network-access-list.h"
#include "common/security-widgets/network-access-item.h"

#include <kiran-log/qt5-log-i.h>

NetworkAccessList::NetworkAccessList(QWidget* parent) : QWidget(parent),
                                                        m_listWidget(nullptr)
{
    initUI();
    createItem(0);
}

NetworkAccessList::~NetworkAccessList()
{
}

void NetworkAccessList::setNeteworkInfos(QList<NetworkAccessList::NetworkAccessInfo> infos)
{
    int count = 0;
    for (auto info : infos)
    {
        if (count > 0)
            createItem(count);

        auto listItem = m_listWidget->item(count);
        NetworkAccessItem* item = qobject_cast<NetworkAccessItem*>(m_listWidget->itemWidget(listItem));
        if (item)
        {
            item->setInfo(info.protocols, info.addr, info.port);
            item->setDefaultPort(info.defaultPort);
            // 如果系统端口，则用户不能修改
            if (info.defaultPort)
                item->setEnabled(false);
        }

        count++;
    }
}

QList<NetworkAccessList::NetworkAccessInfo> NetworkAccessList::getNeteworkInfos()
{
    QList<NetworkAccessInfo> infos;

    int row = 0;
    while (row < m_listWidget->count())
    {
        auto listItem = m_listWidget->item(row);
        auto item = qobject_cast<NetworkAccessItem*>(m_listWidget->itemWidget(listItem));
        if (item)
        {
            NetworkAccessInfo info;
            item->getInfo(info.protocols, info.addr, info.port);
            info.defaultPort = item->isDefaultPort();

            infos.append(info);
        }

        row++;
    }
    return infos;
}

void NetworkAccessList::clearItems()
{
    int count = m_listWidget->count();

    // 从最后一项开始删除，保留第 0 项（即第一项）,并清空其数据
    for (int i = count - 1; i >= 0; --i)
    {
        if (i == 0)
        {
            auto listItem = m_listWidget->item(i);
            auto item = qobject_cast<NetworkAccessItem*>(m_listWidget->itemWidget(listItem));
            if (item)
            {
                item->setInfo(QStringList() << "tcp", "", 0);
            }
            continue;
        }

        auto deleteItem = m_listWidget->takeItem(i);
        deleteOneItem(deleteItem);
    }
}

void NetworkAccessList::setItemsEnable(bool enable)
{
    int row = 0;
    while (row < m_listWidget->count())
    {
        auto listItem = m_listWidget->item(row);
        auto item = qobject_cast<NetworkAccessItem*>(m_listWidget->itemWidget(listItem));
        if (item)
        {
            item->isDefaultPort() ? item->setEnabled(false) : item->setEnabled(enable);
        }
        row++;
    }
}

void NetworkAccessList::initUI()
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

void NetworkAccessList::createItem(int index)
{
    QListWidgetItem* newItem = nullptr;
    NetworkAccessItem* customItem = nullptr;

    newItem = new QListWidgetItem();

    customItem = new NetworkAccessItem(m_listWidget);
    connect(customItem, &NetworkAccessItem::sigAdd, this, &NetworkAccessList::addItem);
    connect(customItem, &NetworkAccessItem::sigDelete, this, &NetworkAccessList::deleteItem);

    if (index == 0)
        customItem->setDeleteBtnVisible(false);

    m_listWidget->insertItem(index, newItem);
    m_listWidget->setItemWidget(newItem, customItem);

    newItem->setSizeHint(QSize(500, 68));
    m_listWidget->setGridSize(QSize(500, 78));
}

void NetworkAccessList::addItem()
{
    auto item = qobject_cast<NetworkAccessItem*>(sender());
    if (!item)
        return;

    int row = 0;
    while (row < m_listWidget->count())
    {
        auto listItem = m_listWidget->item(row);
        if (m_listWidget->itemWidget(listItem) == item)
        {
            KLOG_DEBUG() << "Insert network Access item in index:" << row;
            createItem(row + 1);
            break;
        }
        row++;
    }
}

void NetworkAccessList::deleteItem()
{
    // 第0行不能删除
    if (m_listWidget->count() == 1)
        return;

    auto item = qobject_cast<NetworkAccessItem*>(sender());
    if (!item)
        return;

    int row = 0;
    while (row < m_listWidget->count() && m_listWidget->count() > 1)
    {
        auto listItem = m_listWidget->item(row);
        if (m_listWidget->itemWidget(listItem) == item)
        {
            KLOG_DEBUG() << "Delete network access item index:" << row;
            deleteOneItem(listItem);
            break;
        }
        row++;
    }
}

void NetworkAccessList::deleteOneItem(QListWidgetItem* listItem)
{
    if (listItem)
    {
        auto widget = qobject_cast<NetworkAccessItem*>(m_listWidget->itemWidget(listItem));
        if (widget)
        {
            // 删除前先断开信号连接
            disconnect(widget, &NetworkAccessItem::sigAdd, this, &NetworkAccessList::addItem);
            disconnect(widget, &NetworkAccessItem::sigDelete, this, &NetworkAccessList::deleteItem);
            delete widget;
            widget = nullptr;
        }

        delete listItem;
        listItem = nullptr;
    }
}
