/**
 * @file          /ks-scmc-gui/src/pages/container/advanced-configuration/volumes-conf-tab.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "volumes-conf-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QVBoxLayout>
#include "ui_volumes-conf-tab.h"
VolumesConfTab::VolumesConfTab(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::VolumesConfTab),
                                                  m_configTable(nullptr)
{
    ui->setupUi(this);
    initUI();
}

VolumesConfTab::~VolumesConfTab()
{
    delete ui;
}

bool VolumesConfTab::getVolumeInfo(container::ContainerConfigs *cfg, QString &errMSg)
{
    if (cfg)
    {
        auto itemList = m_configTable->getAllData();
        for (auto item : itemList)
        {
            if (item->m_firstColVal.isEmpty() && item->m_secondColVal.isEmpty())
            {
                continue;
            }
            else if (!item->m_firstColVal.isEmpty() && !item->m_secondColVal.isEmpty())
            {
                KLOG_DEBUG() << "Volumes info:"
                             << "container path:" << item->m_firstColVal
                             << "host path: " << item->m_secondColVal
                             << "permission: " << item->m_thirdColVal;

                auto mount = cfg->add_mounts();
                mount->set_type("bind");
                mount->set_target(item->m_firstColVal.toStdString());
                mount->set_source(item->m_secondColVal.toStdString());
                mount->set_read_only(item->m_thirdColVal);
            }
            else
            {
                errMSg = tr("Please improve the contents in volumes table!");
                return false;
            }
        }
        return true;
    }
    else
    {
        errMSg = tr("The container config arg is error.");
        return false;
    }
}

bool VolumesConfTab::getVolumeInfo(container::UpdateRequest *req, QString &errMSg)
{
    if (req)
    {
        auto itemList = m_configTable->getAllData();
        for (auto item : itemList)
        {
            if (item->m_firstColVal.isEmpty() && item->m_secondColVal.isEmpty())
            {
                continue;
            }
            else if (!item->m_firstColVal.isEmpty() && !item->m_secondColVal.isEmpty())
            {
                KLOG_DEBUG() << "Volumes info:"
                             << "container path:" << item->m_firstColVal
                             << "host path: " << item->m_secondColVal
                             << "permission: " << item->m_thirdColVal;

                auto mount = req->add_mounts();
                mount->set_type("bind");
                mount->set_target(item->m_firstColVal.toStdString());
                mount->set_source(item->m_secondColVal.toStdString());
                mount->set_read_only(item->m_thirdColVal);
            }
            else
            {
                errMSg = tr("Please improve the contents in volumes table!");
                return false;
            }
        }
        return true;
    }
    else
    {
        errMSg = tr("The container config arg is error.");
        return false;
    }
}

void VolumesConfTab::setVolumeInfo(const container::ContainerConfigs *cfg)
{
    QList<QSharedPointer<ModelItem>> itemList;
    for (auto mounts : cfg->mounts())
    {
        KLOG_DEBUG() << "Volumes info:"
                     << "host path: " << mounts.source().data()
                     << "container path: " << mounts.target().data()
                     << "permission: " << mounts.read_only();
        QSharedPointer<ModelItem> pItem(new ModelItem());
        pItem->m_firstColVal = mounts.target().data();
        pItem->m_secondColVal = mounts.source().data();
        pItem->m_thirdColVal = mounts.read_only();
        itemList.push_back(pItem);
    }
    if (!itemList.isEmpty())
        m_configTable->setData(itemList);
}

void VolumesConfTab::initUI()
{
    m_configTable = new ConfigTable(CONFIG_TABLE_TYPE_VOLUMES, this);
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setMargin(20);
    vLayout->addWidget(m_configTable);
}
