/**
 * @file          src/pages/container/advanced-configuration/IPC.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma once

#include <QComboBox>
#include <QWidget>
#include "base/interface/ipc.h"

class IPCTab : public QWidget
{
    Q_OBJECT
public:
    explicit IPCTab(QWidget *parent = nullptr);

    void update(qint64 nodeID);

    QString getIPC();
    void setIPC(qint64 nodeID, QString ipc);

private:
    void initUI();
    void handleIPCList(const QPair<grpc::Status, ipcbase::IpcListReply> &);

private:
    QComboBox *m_combobox;
    qint64 m_nodeID;
    QString m_currentIPC;
};