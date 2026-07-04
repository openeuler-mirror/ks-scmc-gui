/**
 * @file          /ks-scmc-gui/src/pages/container/container-manager/container-backup-page.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef CONTAINERBACKUPPAGE_H
#define CONTAINERBACKUPPAGE_H
#include "container-backup-operate-dialog.h"
#include "info-worker.h"
#include "table-page.h"

class ContainerBackupOperateDialog;
class ContainerBackupPage : public TablePage
{
    Q_OBJECT
public:
    explicit ContainerBackupPage(QWidget *parent = nullptr);
    ~ContainerBackupPage();
    void updateInfo(QString keyword = "");
    void updateBackupList(int nodeId, std::string containerId, QString containerStatus);

private slots:
    void onCreateBackupBtn();
    void onRemoveBackupBtn();
    void onBackupOperate(BackupOperateType type, QString desc, QString name);

    void onRemoveBackup(int row);
    void onResumeBackup(int row);
    void onUpdateBackup(int row);
    void onExportBackup(int row);
    void exportBackup(bool isDownload, QString version, QString path);

    void getListBackupFinished(const QString objId, const QPair<grpc::Status, container::ListBackupReply> &);
    void getUpdateBackupFinished(const QString objId, const QPair<grpc::Status, container::UpdateBackupReply> &);
    void getCreateBackupFinished(const QString objId, const QPair<grpc::Status, container::CreateBackupReply> &);
    void getResumeBackupFinished(const QString objId, const QPair<grpc::Status, container::ResumeBackupReply> &);
    void getRemoveBackupFinished(const QString objId, const QPair<grpc::Status, container::RemoveBackupReply> &);
    void getExportBackupFinished(const QString objId, const QPair<grpc::Status, QString> &);

private:
    void initTable();
    void initButtons();
    void initConnect();

private:
    QString m_objId;
    ContainerBackupOperateDialog *m_backupAddDlg;
    ContainerBackupOperateDialog *m_backupEditDlg;
    ContainerBackupOperateDialog *m_backupExportDlg;
    //KiranTitlebarWindow *m_exportWindow;
    int m_nodeId;
    std::string m_containerId;
    QString m_containerStatus;
    int64_t m_updateBackupId;
    int64_t m_exportBackupId;
    QTimer *m_timer;
};

#endif  // CONTAINERBACKUPPAGE_H
