#include "volumes-conf-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QVBoxLayout>
#include "ui_volumes-conf-tab.h"
VolumesConfTab::VolumesConfTab(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::VolumesConfTab)
{
    ui->setupUi(this);
    initUI();
}

VolumesConfTab::~VolumesConfTab()
{
    delete ui;
}

ErrorCode VolumesConfTab::getVolumeInfo(container::ContainerConfigs *cfg)
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
                KLOG_INFO() << "container path:" << item->m_firstColVal
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
                return INPUT_NULL_ERROR;
            }
        }
        return NO_ERROR;
    }
    return CONFIG_ARG_ERROR;
}

void VolumesConfTab::setVolumeInfo(const container::Mount *mounts)
{
    KLOG_INFO() << "setVolumeInfo";
    KLOG_INFO() << mounts->source().data() << mounts->target().data() << mounts->read_only();
}

void VolumesConfTab::initUI()
{
    m_configTable = new ConfigTable(CONFIG_TABLE_TYPE_VOLUMES, this);
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setMargin(20);
    vLayout->addWidget(m_configTable);
}
