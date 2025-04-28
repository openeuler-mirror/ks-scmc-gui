/**
 * @file          src/pages/node/security-configuration/network-access-ctl-page.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma once

#include <QAbstractButton>
#include <QListWidget>
#include "page.h"

namespace Ui
{
class NetworkConfigPage;
}

class NetworkConfigPage : public Page
{
    Q_OBJECT
public:
    explicit NetworkConfigPage(QWidget *parent = nullptr);
    void updateInfo(QString keyword = "") override;

    void setAccessCtrlInfo();
    void getAccessCtrlInfo();

    void setWhiteListInfo();
    void getWhiteListInfo();

private slots:
    void save();
    void setConfigEnabled(QAbstractButton *btn);

private:
    void initUI();
    void initConnect();

private:
    Ui::NetworkConfigPage *ui;
    QListWidget *m_accessList;
    QListWidget *m_processList;
    bool m_accessCtrlEnabled;
    bool m_whiteListEnabled;
};