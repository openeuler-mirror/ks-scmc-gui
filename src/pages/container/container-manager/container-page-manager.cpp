#include "container-page-manager.h"
#include "container-info-page.h"
#include "container-list-page.h"
ContainerPageManager::ContainerPageManager(QWidget *parent) : StackedPage(parent)
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
        ContainerInfoPage *containerInfoPage = new ContainerInfoPage(this);
        page = containerInfoPage;
        ;
    }
    default:
        break;
    }

    addStackedPage(page, key);
}

void ContainerPageManager::updateInfo(QString keyword)
{
    setCurrentPage(CONTAINER_PAGE_TYPE_LIST);
    updateCurrentPage();
}

void ContainerPageManager::onNameClicked(QMap<QString, QVariant> infoMap)
{
    setCurrentPage(CONTAINER_PAGE_TYPE_INFO);
    //m_nodeInfoPage->setNodeId(nodeId);
    updateCurrentPage();
}
