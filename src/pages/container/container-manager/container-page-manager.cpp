/**
 * @file          /ks-scmc-gui/src/pages/container/container-manager/container-page-manager.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "container-page-manager.h"
#include <kiran-log/qt5-log-i.h>
#include "container-info-page.h"
#include "container-list-page.h"
ContainerPageManager::ContainerPageManager(QWidget *parent) : StackedPage(parent), m_containerInfoPage(nullptr)
{
    createStackedPage(CONTAINER_PAGE_TYPE_LIST, "Container List");
    createStackedPage(CONTAINER_PAGE_TYPE_INFO, "Container Info");
}

void ContainerPageManager::createStackedPage(ContainerPageType type, QString key)
{
    Page *page = nullptr;
    switch (type)
    {
    case CONTAINER_PAGE_TYPE_LIST:
    {
        ContainerListPage *containerListPage = new ContainerListPage(this);
        connect(containerListPage, &ContainerListPage::sigContainerNameClicked, this, &ContainerPageManager::onNameClicked);
        page = containerListPage;
        break;
    }
    case CONTAINER_PAGE_TYPE_INFO:
    {
        m_containerInfoPage = new ContainerInfoPage(this);
        page = m_containerInfoPage;
    }
    default:
        break;
    }

    addStackedPage(page, key);
}

void ContainerPageManager::updateInfo(QString keyword)
{
    setCurrentPage(CONTAINER_PAGE_TYPE_LIST);
    updateCurrentPage(keyword);
}

void ContainerPageManager::onNameClicked(QMap<QString, QVariant> infoMap)
{
    setCurrentPage(CONTAINER_PAGE_TYPE_INFO);

    m_containerInfoPage->setContainerInfo(infoMap);
    updateCurrentPage();
}
