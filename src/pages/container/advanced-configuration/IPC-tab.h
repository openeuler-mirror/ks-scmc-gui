/**
 * @file          src/pages/container/advanced-configuration/IPC.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma once

#include <QComboBox>
#include <QWidget>

class IPCTab : public QWidget
{
    Q_OBJECT
public:
    explicit IPCTab(QWidget *parent = nullptr);

    void setIPC();
    int getIPC();

private:
    void initUI();

private:
    QComboBox *m_combobox;
};