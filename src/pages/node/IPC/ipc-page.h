/**
 * @file          src/pages/node/IPC/ipc-page.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#ifndef IPCPAGE_H
#define IPCPAGE_H

#include <QLineEdit>
#include <QObject>
#include "table-page.h"

class KiranTitlebarWindow;
class IPCPage : public TablePage
{
    Q_OBJECT
public:
    explicit IPCPage(QWidget *parent = nullptr);
    void updateInfo(QString keyword = "") override;

private slots:
    void popupCreateDialog();
    void createIPC();
    void deleteIPC();

private:
    void initCreateDialog();
    void initButtons();
    void initTable();
    void initConnect();

private:
    KiranTitlebarWindow *m_createDialog;
    QLineEdit *m_edit;
};

#endif  // IPCPAGE_H
