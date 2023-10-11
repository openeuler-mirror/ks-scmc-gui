#ifndef BACKUPADDITIONDIALOG_H
#define BACKUPADDITIONDIALOG_H

#include <kiran-titlebar-window.h>
#include <QWidget>
namespace Ui
{
class BackupAdditionDialog;
}

class BackupAdditionDialog : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit BackupAdditionDialog(QWidget *parent = nullptr);
    ~BackupAdditionDialog();

signals:
    void sigSave(QString desc);

private:
    Ui::BackupAdditionDialog *ui;
};

#endif  // BACKUPADDITIONDIALOG_H
