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
    BACKUP_OPERATE_TYPE_EDIT,
    BACKUP_OPERATE_TYPE_EXPORT
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

private slots:
    void save();

signals:
    void sigSave(BackupOperateType type, QString desc, QString name);
    void sigExport(bool isDownload, QString name, QString version, QString desc, QString path);

private:
    Ui::ContainerBackupOperateDialog *ui;
    BackupOperateType m_type;
    int64_t m_backupId;
};

#endif  // CONTAINERBACKUPOPERATEDIALOG_H
