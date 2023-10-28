/**
 * @file          /ks-scmc-gui/src/pages/container/container-manager/container-backup-operate-dialog.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "container-backup-operate-dialog.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QDateTime>
#include <QFileDialog>
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
    if (m_type != BACKUP_OPERATE_TYPE_EXPORT)
    {
        ui->stackedWidget->setCurrentWidget(ui->page_operate);

        ui->lineEdit_version->setText(currTime.toString("yyyyMMddhhmmss"));
        ui->lineEdit_version->setMaxLength(20);
        ui->lineEdit_version->setPlaceholderText(tr("Please input 1 to 20 characters"));
        if (m_type == BACKUP_OPERATE_TYPE_EDIT)
            ui->lineEdit_version->setDisabled(true);

        ui->btn_tip->setIcon(QIcon(":/images/tips.svg"));
        ui->btn_tip->setToolTip(tr("Only letter, digit or ._- three special characters;\nThe first characters must be letter or digit"));
        ui->label_tip_version->hide();

        ui->textEdit_desc->setPlaceholderText(tr("Please input 0 to 200 characters"));
    }
    else
    {
        ui->stackedWidget->setCurrentWidget(ui->page_export);
        ui->btn_tip_name->setIcon(QIcon(":/images/tips.svg"));
        ui->btn_tip_name->setToolTip(tr("Only lowercase, digit or ._- three special characters;\nThe first and last characters cannot be special characters"));
        ui->btn_tip_version->setIcon(QIcon(":/images/tips.svg"));
        ui->btn_tip_version->setToolTip(tr("Only letter, digit or ._- three special characters;\nThe first characters must be letter or digit"));

        ui->lineEdit_image_name->setMaxLength(50);
        ui->lineEdit_image_name->setPlaceholderText(tr("Please input 1 to 50 characters"));
        ui->lineEdit_image_version->setMaxLength(20);
        ui->lineEdit_image_version->setPlaceholderText(tr("Please input 1 to 20 characters"));
        ui->lineEdit_image_version->setText(currTime.toString("yyyyMMddhhmmss"));
        ui->textEdit_image_desc->setPlaceholderText(tr("Please input 0 to 200 characters"));

        ui->rd_btn_local->setChecked(true);
        ui->label_image_tip->hide();
    }
    connect(ui->textEdit_image_desc, &QTextEdit::textChanged, this, &ContainerBackupOperateDialog::limitLength);
    connect(ui->textEdit_desc, &QTextEdit::textChanged, this, &ContainerBackupOperateDialog::limitLength);
    connect(ui->btn_ok, &QPushButton::clicked, this, &ContainerBackupOperateDialog::save);
    connect(ui->btn_cancel, &QPushButton::clicked, this, &ContainerBackupOperateDialog::close);
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

void ContainerBackupOperateDialog::save()
{
    if (m_type != BACKUP_OPERATE_TYPE_EXPORT)
    {
        if (ui->lineEdit_version->text().isEmpty())
        {
            ui->label_tip_version->setText(tr("Please input backup version!"));
            ui->label_tip_version->show();
            return;
        }
        ui->label_tip_version->hide();
        emit sigSave(m_type, ui->textEdit_desc->toPlainText(), ui->lineEdit_version->text());
    }
    else
    {
        if (ui->lineEdit_image_name->text().isEmpty() || ui->lineEdit_image_version->text().isEmpty())
        {
            ui->label_image_tip->setText(tr("Please complate the information!"));
            ui->label_image_tip->show();
            return;
        }

        ui->label_tip_version->hide();
        bool isDownLoad = ui->rd_btn_local->isChecked();
        QString name = ui->lineEdit_image_name->text();
        QString version = ui->lineEdit_image_version->text();
        QString desc = ui->textEdit_image_desc->toPlainText();
        QString path = "";

        if (isDownLoad)
        {
            path = QFileDialog::getExistingDirectory(this, tr("Please select the path to save"), "./");
            if (path.isEmpty())
            {
                return;
            }
            if (path.at(path.size() - 1) != "/")
                path += "/";
            KLOG_INFO() << "imagePath:" << path;
        }

        emit sigExport(isDownLoad, name, version, desc, path);

        //popup file dialog
    }
}
