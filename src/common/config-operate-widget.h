/**
 * @file          /ks-scmc-gui/src/common/config-operate-widget.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef CONFIGOPERATEWIDGET_H
#define CONFIGOPERATEWIDGET_H

#include <QWidget>

namespace Ui
{
class ConfigOperateWidget;
}

class ConfigOperateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigOperateWidget(int curRow, int page, QWidget *parent = nullptr);
    ~ConfigOperateWidget();
    int getCurRow();
    int getCurPage();
    void setPage(int page);

private:
    void initControl();
    void initConnections();

signals:
    void sendSaveSig(ConfigOperateWidget *);
    void sendEditSig(ConfigOperateWidget *);
    void sendAddSig(ConfigOperateWidget *);
    void sendDeleteSig(ConfigOperateWidget *);

private slots:
    void sendSave();
    void sendEdit();
    void sendAdd();
    void sendDelete();

private:
    Ui::ConfigOperateWidget *ui;
    int m_CurRow;
};

#endif  // CONFIGOPERATEWIDGET_H
