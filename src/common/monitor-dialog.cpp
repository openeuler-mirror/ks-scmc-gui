/**
 * @file          /ks-scmc-gui/src/common/monitor-dialog.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "monitor-dialog.h"
#include <QVBoxLayout>
#include "monitor-content.h"
MonitorDialog::MonitorDialog(int nodeId, std::string containerId = "", QWidget *parent) : KiranTitlebarWindow(parent)
{
    setIcon(QIcon(":/images/logo.png"));
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::ApplicationModal);
    setMonitorContent(nodeId, containerId);
}

QSize MonitorDialog::sizeHint() const
{
    return QSize(1630, 820);
}

void MonitorDialog::setMonitorContent(int nodeId, std::string containerId)
{
    QVBoxLayout *mainVLayout = new QVBoxLayout(getWindowContentWidget());
    mainVLayout->setMargin(0);

    MonitorContent *monitorContent = new MonitorContent(this, nodeId, containerId);
    monitorContent->setParent(getWindowContentWidget());
    mainVLayout->addWidget(monitorContent);
}
