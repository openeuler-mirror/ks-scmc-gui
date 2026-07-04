/**
 * @file          /ks-scmc-gui/src/pages/container/advanced-configuration/high-availability-tab.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef HIGHAVAILABILITYTAB_H
#define HIGHAVAILABILITYTAB_H

#include <QWidget>
#include "info-worker.h"
namespace Ui
{
class HighAvailabilityTab;
}

class HighAvailabilityTab : public QWidget
{
    Q_OBJECT

public:
    explicit HighAvailabilityTab(QWidget *parent = nullptr);
    ~HighAvailabilityTab();
    void getRestartPolicy(container::RestartPolicy *cfg);
    void setRestartPolicy(container::RestartPolicy *cfg);

private slots:
    void onCbActivated(QString text);
    void setLineEditVisible(bool visible);

private:
    void initUI();

private:
    Ui::HighAvailabilityTab *ui;
};

#endif  // HIGHAVAILABILITYTAB_H
