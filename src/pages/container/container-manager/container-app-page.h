#ifndef CONTAINERAPPDIALOG_H
#define CONTAINERAPPDIALOG_H

#include <kiran-titlebar-window.h>
#include <QProcess>
#include "container-app-operate-dialog.h"
#include "info-worker.h"
#include "table-page.h"

class ContainerAppDialog : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit ContainerAppDialog(int64_t nodeId, QString nodeAddr, std::string containerId, QString containerName, QWidget *parent = nullptr);
    ~ContainerAppDialog() = default;
};

class ContainerAppPage : public TablePage
{
    Q_OBJECT
public:
    explicit ContainerAppPage(int64_t nodeId, QString nodeAddr, std::string containerId, QString containerName, QWidget *parent = nullptr);
    ~ContainerAppPage();
    void updateInfo(QString keyword = "");

private slots:
    void onCreate();
    void onEdit(int row);
    void onRun(int row);
    void onStop(int row);
    void onDelete();
    void onDelete(int row);
    void onSaveApp(const QString name, const QString path, const bool isGui);
    void guiAppStatus(QProcess::ProcessState state);
    void getListAppEntryFinished(const QString objId, const QPair<grpc::Status, container::ListAppEntryReply> &);
    void getAddAppEntryFinished(const QString objId, const QPair<grpc::Status, container::AddAppEntryReply> &);
    void getUpdateAppEntryFinished(const QString objId, const QPair<grpc::Status, container::UpdateAppEntryReply> &);
    void getRemoveAppEntryFinished(const QString objId, const QPair<grpc::Status, container::RemoveAppEntryReply> &);
    void getRunAppEntryFinished(const QString objId, const QPair<grpc::Status, container::RunAppEntryReply> &);
    void getKillAppEntryFinished(const QString objId, const QPair<grpc::Status, container::KillAppEntryReply> &);

private:
    void initButtons();
    void initTable();
    void initConnect();
    void showOperateDlg();

private:
    QString m_objId;
    int m_nodeId;
    std::string m_containerId;
    QString m_nodeAddr;
    QString m_containerName;
    QMap<int, QPushButton *> m_batchOpBtnMap;
    ContainerAppOperateDialog *m_appOp;
    QProcess *m_proc;
    int m_runningAppRow;
};

#endif  // CONTAINERAPPDIALOG_H
