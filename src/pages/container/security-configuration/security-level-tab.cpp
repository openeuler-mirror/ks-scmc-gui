/**
 * @file          src/pages/container/security-configuration/security-level-tab.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "security-level-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QObject>
#include "ui_security-level-tab.h"

#define LEVEL_1_DESC QObject::tr(                                                                                                                                   \
    "<h3><b>Level 1 Template:</b></h3>"                                                                                                                             \
    "<ol style='font-size: 12px; line-height: 1.5;'>"                                                                                                               \
    "<li>Containers can access the external network of the platform, but cannot access other containers on the platform or the same node.</li>"                     \
    "<li>Support network whitelist configuration. The network whitelist can be configured to access platforms and other containers on the same node.</li>"          \
    "<li>Support whitelist configuration for container network processes."                                                                                          \
    "If the container network process whitelist is closed, "                                                                                                        \
    "the network access permissions of all processes in the container will be executed according to the network whitelist and default rules. "                      \
    "If the network process whitelist is enabled, but the whitelist is empty, then processes in the container cannot access the external network of the container." \
    "Add network processes to the whitelist,"                                                                                                                       \
    "and the network access permissions of processes in the whitelist will be executed according to the network whitelist and default rules.</li>"                  \
    "<li>When a container applies a first level template, by default, the container can access the external network of the platform. "                              \
    "At this point, external access control can be enabled to prohibit the current container from accessing the external network. "                                 \
    "This control switch is only available when the container successfully applies the first level template.</li>"                                                  \
    "</ol>")

#define LEVEL_2_DESC QObject::tr(                                                                                                                                   \
    "<h3><b>Level 2 Template:</b></h3>"                                                                                                                             \
    "<ol style='font-size: 12px; line-height: 1.5;'>"                                                                                                               \
    "<li>Containers cannot access the external network of the platform, and other containers on the same node can access the platform.</li>"                        \
    "<li>Support network whitelist configuration. "                                                                                                                 \
    "The network whitelist can be configured to access external networks of the platform and other containers on the same node.</li>"                               \
    "<li>Support whitelist configuration for container network processes. If the container network process whitelist is closed, "                                   \
    "the network access permissions of all processes in the container will be executed according to the network whitelist and default rules."                       \
    "If the network process whitelist is enabled, but the whitelist is empty, then processes in the container cannot access the external network of the container." \
    "Add network processes to the whitelist, "                                                                                                                      \
    "and the network access permissions of processes in the whitelist will be executed according to the network whitelist and default rules.</li>"                  \
    "</ol>")

#define LEVEL_3_DESC QObject::tr(                                                                                                                               \
    "<h3><b>Level 3 Template:</b></h3>"                                                                                                                         \
    "<ol style='font-size: 12px; line-height: 1.5;'>"                                                                                                           \
    "<li>Containers cannot be started or stopped manually by executing commands at the node, and can only be started or stopped through platform control.</li>" \
    "<li>Containers cannot access the external network of the platform, and other containers on the same node can access the platform.</li>"                    \
    "<li>Support network whitelist configuration. "                                                                                                             \
    "The network whitelist can be configured to access external networks of the platform and other containers on the same node.</li>"                           \
    "<li>Support whitelist configuration for container network processes. "                                                                                     \
    "If the container network process whitelist is closed, "                                                                                                    \
    "the network access permissions of all processes in the container will be executed according to the network whitelist and default rules."                   \
    "If the network process whitelist is enabled, but the whitelist is empty, "                                                                                 \
    "then processes in the container cannot access the external network of the container. "                                                                     \
    "Add network processes to the whitelist,"                                                                                                                   \
    "and the network access permissions of processes in the whitelist will be executed according to the network whitelist and default rules.</li>"              \
    "</ol>")

#define LEVEL_4_DESC QObject::tr(                                                                                                                               \
    "<h3><b>Level 4 Template:</b></h3>"                                                                                                                         \
    "<ol style='font-size: 12px; line-height: 1.5;'>"                                                                                                           \
    "<li>Containers cannot be started or stopped manually by executing commands at the node, and can only be started or stopped through platform control.</li>" \
    "<li>Containers cannot access the platform's external network, platform, or other containers on the same node.</li>"                                        \
    "<li>Support network whitelist configuration."                                                                                                              \
    "The network whitelist can be configured to access platforms, external networks of platforms, and other containers on the same node.</li>"                  \
    "<li>Support whitelist configuration for container network processes. "                                                                                     \
    "If the container network process whitelist is closed, "                                                                                                    \
    "the network access permissions of all processes in the container will be executed according to the network whitelist and default rules. "                  \
    "If the network process whitelist is enabled, but the whitelist is empty, "                                                                                 \
    "then processes in the container cannot access the external network of the container."                                                                      \
    "Add network processes to the whitelist,"                                                                                                                   \
    "and the network access permissions of processes in the whitelist will be executed according to the network whitelist and default rules.</li>"              \
    "</ol>")

#define LEVEL_5_DESC QObject::tr(                                                                                                                  \
    "<h3><b>Level 5 Template:</b></h3>"                                                                                                            \
    "<ol style='font-size: 12px; line-height: 1.5;'>"                                                                                              \
    "<li>Containers can access the platform's external network, platform, and other containers on the same node.</li>"                             \
    "<li>Support whitelist configuration for container network processes."                                                                         \
    "If the container network process whitelist is closed, "                                                                                       \
    "the network access permissions of all processes in the container will be executed according to the network whitelist and default rules. "     \
    "If the network process whitelist is enabled, but the whitelist is empty, "                                                                    \
    "then processes in the container cannot access the external network of the container. "                                                        \
    "Add network processes to the whitelist, "                                                                                                     \
    "and the network access permissions of processes in the whitelist will be executed according to the network whitelist and default rules.</li>" \
    "</ol>")

SecurityLevelTab::SecurityLevelTab(QWidget *parent) : QWidget(parent),
                                                      ui(new Ui::SecurityLevelTab)
{
    ui->setupUi(this);
    initUI();
}

SecurityLevelTab::~SecurityLevelTab()
{
    delete ui;
}

void SecurityLevelTab::setSecurityInfo(int level, bool networkAccessCtl)
{
    ui->cb_level->setCurrentText(m_securityLevelMap[level].first);
    ui->checkbox_access_ctl->setChecked(networkAccessCtl);

    KLOG_DEBUG() << "Set container security level info to UI:"
                 << "level:" << m_securityLevelMap[level].first
                 << "networkAccessCtl:" << networkAccessCtl;
}

void SecurityLevelTab::getSecurityInfo(int &level, bool &networkAccessCtl)
{
    level = ui->cb_level->currentIndex() + 1;
    networkAccessCtl = ui->checkbox_access_ctl->isChecked();

    KLOG_DEBUG() << "Get container security level info from UI:"
                 << "level:" << m_securityLevelMap[level].first
                 << "networkAccessCtl:" << networkAccessCtl;
}

void SecurityLevelTab::initUI()
{
    m_securityLevelMap = {{LEVEL_NUM_1, qMakePair(LEVEL_1, LEVEL_1_DESC)},
                          {LEVEL_NUM_2, qMakePair(LEVEL_2, LEVEL_2_DESC)},
                          {LEVEL_NUM_3, qMakePair(LEVEL_3, LEVEL_3_DESC)},
                          {LEVEL_NUM_4, qMakePair(LEVEL_4, LEVEL_4_DESC)},
                          {LEVEL_NUM_5, qMakePair(LEVEL_5, LEVEL_5_DESC)}};

    ui->label_access_ctl->hide();
    ui->checkbox_access_ctl->hide();

    QStringList keys;
    for (const auto &pair : m_securityLevelMap)
    {
        keys << pair.first;
    }
    ui->cb_level->addItems(keys);

    connect(ui->cb_level, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index)
            {
                auto level = index + 1;
                auto isLevel1 = level == LEVEL_NUM_1;
                ui->label_access_ctl->setVisible(isLevel1);
                ui->checkbox_access_ctl->setVisible(isLevel1);

                ui->textEdit_level->setHtml(m_securityLevelMap[level].second);

                emit levelChanged(level);
            });

    ui->cb_level->setCurrentText(m_securityLevelMap.value(LEVEL_NUM_4).first);  // 默认四级模板
}
