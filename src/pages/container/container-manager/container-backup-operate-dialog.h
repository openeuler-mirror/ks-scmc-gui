/**
 * @file          /ks-scmc-gui/src/pages/container/container-manager/container-backup-operate-dialog.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
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

private:
    void limitLength();

signals:
    void sigSave(BackupOperateType type, QString desc);

private:
    Ui::ContainerBackupOperateDialog *ui;
    BackupOperateType m_type;
    int64_t m_backupId;
};

#endif  // CONTAINERBACKUPOPERATEDIALOG_H
