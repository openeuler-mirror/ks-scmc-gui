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
#include "base/interface/ipc.h"
#include "page.h"

namespace Ui
{
class IPCPage;
}

class KiranTitlebarWindow;
class IPCPage : public Page
{
    Q_OBJECT
public:
    explicit IPCPage(QWidget *parent = nullptr);
    ~IPCPage();
    void updateInfo(QString keyword = "") override;

    void setNodeID(qint64 nodeID);

protected:
    virtual void timerEvent(QTimerEvent *event);

private slots:
    void search();
    void refresh();
    void popupCreateDialog();
    void createIPC();
    void deleteIPC();
    void updateDeleteBtnState(Qt::CheckState checkState);

    void listIPCFinished(QString objId, const QPair<grpc::Status, ipcbase::IpcListReply> &);
    void createIPCFinished(const QString objId, const QPair<grpc::Status, ipcbase::IpcCreateReply> &);
    void removeIPCFinished(const QString objId, const QPair<grpc::Status, ipcbase::IpcRemoveReply> &);

private:
    void initUI();
    void initCreateDialog();
    void clearText();
    void setTips(const QString &tips);
    void initConnect();

private:
    Ui::IPCPage *ui;
    QString m_objId;
    int m_timerID;
    qint64 m_nodeID;
    KiranTitlebarWindow *m_createDialog;
    QLineEdit *m_edit;
};

#endif  // IPCPAGE_H
