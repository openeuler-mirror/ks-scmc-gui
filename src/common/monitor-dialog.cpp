#include "monitor-dialog.h"
#include <QVBoxLayout>
#include "monitor-content.h"
MonitorDialog::MonitorDialog(int nodeId, std::string containerId = "", QWidget *parent) : KiranTitlebarWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowModality(Qt::WindowModal);
    setMonitorContent(nodeId, containerId);
}

void MonitorDialog::setMonitorContent(int nodeId, std::string containerId)
{
    QVBoxLayout *mainVLayout = new QVBoxLayout(getWindowContentWidget());
    mainVLayout->setMargin(0);

    MonitorContent *monitorContent = new MonitorContent(nodeId, containerId);
    monitorContent->setParent(getWindowContentWidget());
    mainVLayout->addWidget(monitorContent);
}
