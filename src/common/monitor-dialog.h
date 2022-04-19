#ifndef MONITORDIALOG_H
#define MONITORDIALOG_H

#include <kiran-titlebar-window.h>
#include <QWidget>
class MonitorDialog : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit MonitorDialog(int nodeId, std::string containerId, QWidget *parent = nullptr);
    virtual QSize sizeHint() const override;

private:
    void setMonitorContent(int nodeId, std::string containerId = "");
};

#endif  // MONITORDIALOG_H
