#ifndef CONTAINERBACKUPOPERATEDIALOG_H
#define CONTAINERBACKUPOPERATEDIALOG_H

#include <kiran-titlebar-window.h>
#include <QWidget>

enum BackupOperateType
{
    BACKUP_OPERATE_TYPE_CREATE,
    BACKUP_OPERATE_TYPE_EDIT
};

namespace Ui
{
class ContainerBackupOperateDialog;
}

class ContainerBackupOperateDialog : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit ContainerBackupOperateDialog(BackupOperateType type, QWidget *parent = nullptr);
    ~ContainerBackupOperateDialog();

signals:
    void sigSave(BackupOperateType type, QString desc);

private:
    Ui::ContainerBackupOperateDialog *ui;
    BackupOperateType m_type;
    int64_t m_backupId;
};

#endif  // CONTAINERBACKUPOPERATEDIALOG_H
