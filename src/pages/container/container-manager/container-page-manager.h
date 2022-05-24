#ifndef CONTAINERPAGEMANAGER_H
#define CONTAINERPAGEMANAGER_H

#include "stacked-page.h"

enum ContainerPageType
{
    CONTAINER_PAGE_TYPE_LIST,
    CONTAINER_PAGE_TYPE_INFO
};

class ContainerInfoPage;
class ContainerPageManager : public StackedPage
{
    Q_OBJECT
public:
    explicit ContainerPageManager(QWidget *parent = nullptr);
    void createStackedPage(ContainerPageType type, QString key);
    virtual void updateInfo(QString keyword = "");

private slots:
    void onNameClicked(QMap<QString, QVariant> infoMap);

private:
    ContainerInfoPage *m_containerInfoPage;
};

#endif  // CONTAINERPAGEMANAGER_H
