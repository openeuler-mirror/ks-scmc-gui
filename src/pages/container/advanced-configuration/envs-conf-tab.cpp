/**
 * @file          /ks-scmc-gui/src/pages/container/advanced-configuration/envs-conf-tab.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "envs-conf-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QVBoxLayout>
#include "common/message-dialog.h"
#include "ui_envs-conf-tab.h"
EnvsConfTab::EnvsConfTab(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::EnvsConfTab),
                                            m_configTable(nullptr)
{
    ui->setupUi(this);
    initUI();
}

EnvsConfTab::~EnvsConfTab()
{
    delete ui;
}

ErrorCode EnvsConfTab::getEnvInfo(container::ContainerConfigs *cntrCfg)
{
    if (cntrCfg)
    {
        auto env = cntrCfg->mutable_envs();
        auto itemList = m_configTable->getAllData();
        for (auto item : itemList)
        {
            auto key = item->m_firstColVal;
            auto value = item->m_secondColVal;
            printf("%s,%d,%d,%d\n", __func__, __LINE__, key.isEmpty(), value.isEmpty());
            if (key.isEmpty())
            {
                if (!value.isEmpty())
                    return INPUT_NULL_ERROR;
                else
                    continue;
            }
            env->insert({key.toStdString(), value.toStdString()});
        }
        return NO_ERROR;
    }
    return CONFIG_ARG_ERROR;
}

void EnvsConfTab::setEnvInfo(const container::ContainerConfigs *cfg)
{
    KLOG_INFO() << "setEnvInfo";
    auto envMap = cfg->envs();

    QList<QSharedPointer<ModelItem>> itemList;
    for (auto it = envMap.begin(); it != envMap.end(); ++it)
    {
        QSharedPointer<ModelItem> pItem(new ModelItem());
        pItem->m_firstColVal = QString::fromStdString(it->first);
        pItem->m_secondColVal = QString::fromStdString(it->second.data());
        itemList.push_back(pItem);
    }

    m_configTable->setData(itemList);
}

void EnvsConfTab::initUI()
{
    m_configTable = new ConfigTable(CONFIG_TABLE_TYPE_ENV, this);
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setMargin(20);
    vLayout->addWidget(m_configTable);
}
