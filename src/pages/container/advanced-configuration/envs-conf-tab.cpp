#include <QVBoxLayout>
#include "common/message-dialog.h"
#include "envs-conf-tab.h"
#include "ui_envs-conf-tab.h"
EnvsConfTab::EnvsConfTab(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::EnvsConfTab)
{
    ui->setupUi(this);
    initUI();
}

EnvsConfTab::~EnvsConfTab()
{
    delete ui;
}

ErrorCode EnvsConfTab::getEnvInfo(container::ContainerConfig *cfg)
{
    if (cfg)
    {
        auto env = cfg->mutable_env();
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

void EnvsConfTab::initUI()
{
    m_configTable = new ConfigTable(CONFIG_TABLE_TYPE_ENV, this);
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setMargin(20);
    vLayout->addWidget(m_configTable);
}
