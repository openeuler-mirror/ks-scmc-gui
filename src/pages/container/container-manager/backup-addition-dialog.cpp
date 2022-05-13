#include "backup-addition-dialog.h"
#include <widget-property-helper.h>
#include <QTextEdit>
#include "ui_backup-addition-dialog.h"
BackupAdditionDialog::BackupAdditionDialog(QWidget *parent) : KiranTitlebarWindow(parent),
                                                              ui(new Ui::BackupAdditionDialog)
{
    ui->setupUi(getWindowContentWidget());
    setAttribute(Qt::WA_DeleteOnClose);
    setTitle(tr("Backup Addition"));
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_ok, Kiran::BUTTON_Default);
    setIcon(QIcon(":/images/logo.png"));

    connect(ui->btn_ok, &QPushButton::clicked,
            [this] {
                emit sigSave(ui->textEdit_desc->toPlainText());
            });
    connect(ui->btn_cancel, &QPushButton::clicked,
            [this] {
                close();
            });
}

BackupAdditionDialog::~BackupAdditionDialog()
{
    delete ui;
}
