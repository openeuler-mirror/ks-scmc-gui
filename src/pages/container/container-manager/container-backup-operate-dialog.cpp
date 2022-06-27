/**
 * @file          /ks-scmc-gui/src/pages/container/container-manager/container-backup-operate-dialog.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
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

    ui->textEdit_desc->setPlaceholderText(tr("Please input 0 to 200 characters"));

    connect(ui->textEdit_desc, &QTextEdit::textChanged, this, &ContainerBackupOperateDialog::limitLength);
    connect(ui->btn_ok, &QPushButton::clicked,
            [this] {
                emit sigSave(m_type, ui->textEdit_desc->toPlainText());
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

void ContainerBackupOperateDialog::limitLength()
{
    QString textContent = ui->textEdit_desc->toPlainText();
    int length = textContent.count();
    int maxLength = 200;  // 最大字符数
    if (length > maxLength)
    {
        int position = ui->textEdit_desc->textCursor().position();
        QTextCursor textCursor = ui->textEdit_desc->textCursor();
        textContent.remove(position - (length - maxLength), length - maxLength);
        ui->textEdit_desc->setText(textContent);
        textCursor.setPosition(position - (length - maxLength));
        ui->textEdit_desc->setTextCursor(textCursor);
    }
}
