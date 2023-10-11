#ifndef CONTAINERBACKUPPAGE_H
#define CONTAINERBACKUPPAGE_H
#include "info-worker.h"
#include "table-page.h"

class BackupAdditionDialog;
class ContainerBackupPage : public TablePage
{
    Q_OBJECT
public:
    explicit ContainerBackupPage(QWidget *parent = nullptr);
    ~ContainerBackupPage();
    void updateInfo(QString keyword = "");
    void updateBackupList(int nodeId, std::string containerId);

private slots:
    void onCreateBackupBtn();
    void onRemoveBackupBtn();
    void onRemoveBackup(int row);
    void onResumeBackup(int row);
    void onUpdateBackup(int row);
    void onCreateBackup(QString desc);

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
    BackupAdditionDialog *m_backupAddDlg;
    int m_nodeId;
    std::string m_containerId;
};

#endif  // CONTAINERBACKUPPAGE_H
