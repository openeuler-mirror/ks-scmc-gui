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
    void onBackupOperate(BackupOperateType type, QString desc);

    void onRemoveBackup(int row);
    void onResumeBackup(int row);
    void onUpdateBackup(int row);

    void getListBackupFinished(const QPair<grpc::Status, container::ListBackupReply> &);
    void getUpdateBackupFinished(const QPair<grpc::Status, container::UpdateBackupReply> &);
    void getCreateBackupFinished(const QPair<grpc::Status, container::CreateBackupReply> &);
    void getResumeBackupFinished(const QPair<grpc::Status, container::ResumeBackupReply> &);
    void getRemoveBackupFinished(const QPair<grpc::Status, container::RemoveBackupReply> &);

private:
    void initTable();
    void initButtons();
    void initConnect();

private:
    ContainerBackupOperateDialog *m_backupAddDlg;
    ContainerBackupOperateDialog *m_backupEditDlg;
    int m_nodeId;
    std::string m_containerId;
    QString m_containerStatus;
    int64_t m_updateBackupId;
};

#endif  // CONTAINERBACKUPPAGE_H
