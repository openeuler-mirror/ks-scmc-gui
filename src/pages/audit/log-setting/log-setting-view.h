/**
 * @file          src/pages/audit/log-setting/log-setting-view.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma once

#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include "page.h"

class LogSettingView : public Page
{
    Q_OBJECT

public:
    explicit LogSettingView(QWidget *parent = nullptr);
    ~LogSettingView();
    void updateInfo(QString keyword = "");

    void setPeriod(int mouth);
    int getPeriod();

private:
    void initUI();
    void updateUI();

private:
    QLineEdit *m_edit;
    QPushButton *m_btn_confirm;
    QPushButton *m_btn_cancel;
    int m_period;
};
