#include "container-backup-operate-dialog.h"
#include <widget-property-helper.h>
#include <QTextEdit>
#include "ui_container-backup-operate-dialog.h"
ContainerBackupOperateDialog::ContainerBackupOperateDialog(BackupOperateType type, QWidget *parent) : KiranTitlebarWindow(parent),
                                                                                                      ui(new Ui::ContainerBackupOperateDialog),
                                                                                                      m_type(type)
{
    ui->setupUi(getWindowContentWidget());
    setAttribute(Qt::WA_DeleteOnClose);
    setIcon(QIcon(":/images/logo.png"));
    setResizeable(false);
    setButtonHints(TitlebarMinimizeButtonHint | TitlebarCloseButtonHint);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_ok, Kiran::BUTTON_Default);

    connect(ui->btn_ok, &QPushButton::clicked,
            [this] {
                emit sigSave(m_type, ui->textEdit_desc->toPlainText());
                close();
            });
    connect(ui->btn_cancel, &QPushButton::clicked,
            [this] {
                close();
            });
}

ContainerBackupOperateDialog::~ContainerBackupOperateDialog()
{
    delete ui;
}
