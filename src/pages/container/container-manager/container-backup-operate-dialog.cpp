/**
 * @file          /ks-scmc-gui/src/pages/container/container-manager/container-backup-operate-dialog.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "container-backup-operate-dialog.h"
#include <widget-property-helper.h>
#include <QDateTime>
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

    QDateTime currTime = QDateTime::currentDateTime();
    ui->lineEdit_version->setText(currTime.toString("yyyyMMddhhmmss"));

    ui->textEdit_desc->setPlaceholderText(tr("Please input 0 to 200 characters"));
    ui->lineEdit_version->setMaxLength(20);
    ui->lineEdit_version->setPlaceholderText(tr("Please input 1 to 20 characters"));
    ui->btn_tip->setIcon(QIcon(":/images/tips.svg"));
    ui->btn_tip->setToolTip(tr("Only letter, digit or ._- three special characters;\nThe first characters must be letter or digit"));
    ui->label_tip_version->hide();

    connect(ui->textEdit_desc, &QTextEdit::textChanged, this, &ContainerBackupOperateDialog::limitLength);
    connect(ui->btn_ok, &QPushButton::clicked,
            [this] {
                if (ui->lineEdit_version->text().isEmpty())
                {
                    ui->label_tip_version->setText(tr("Please input backup version!"));
                    ui->label_tip_version->show();
                    return;
                }
                ui->label_tip_version->hide();
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
