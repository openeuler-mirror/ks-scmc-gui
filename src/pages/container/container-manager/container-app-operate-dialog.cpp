/**
 * @file          src/pages/container/container-manager/container-app-operate-dialog.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "container-app-operate-dialog.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QRegularExpression>
#include <QToolTip>
#include "ui_container-app-operate-dialog.h"
ContainerAppOperateDialog::ContainerAppOperateDialog(AppOperateType type, QWidget *parent) : KiranTitlebarWindow(parent),
                                                                                             ui(new Ui::ContainerAppOperateDialog),
                                                                                             m_type(type)
{
    ui->setupUi(getWindowContentWidget());
    initUI();
}

ContainerAppOperateDialog::ContainerAppOperateDialog(int appId, QString name, QString path, bool isGUI, AppOperateType type, QWidget *parent) : KiranTitlebarWindow(parent),
                                                                                                                                                ui(new Ui::ContainerAppOperateDialog),
                                                                                                                                                m_type(type),
                                                                                                                                                m_appId(appId)
{
    ui->setupUi(getWindowContentWidget());
    initUI();
    ui->lineEdit_name->setText(name);
    ui->lineEdit_path->setText(path);
    ui->cb_type->setCurrentIndex(ui->cb_type->findData(isGUI));
}

ContainerAppOperateDialog::~ContainerAppOperateDialog()
{
    delete ui;
}

int ContainerAppOperateDialog::getAppId()
{
    return m_appId;
}

AppOperateType ContainerAppOperateDialog::getType()
{
    return m_type;
}

void ContainerAppOperateDialog::onSave()
{
    if (ui->lineEdit_name->text().isEmpty() || ui->lineEdit_path->text().isEmpty())
    {
        ui->label_tip->setText(tr("Please complate the information! "));
        return;
    }

    if (!checkFirstCharacter(ui->lineEdit_name->text()))
    {
        ui->label_tip->setText(tr("The characters can only be letter or digit!"));
        return;
    }

    ui->label_tip->clear();

    emit sigSave(ui->lineEdit_name->text(),
                 ui->lineEdit_path->text(),
                 ui->cb_type->currentData().toInt());
}

bool ContainerAppOperateDialog::checkFirstCharacter(const QString &text)
{
    // 检测是否为数字、字母
    QRegularExpression regex("^[a-zA-Z0-9]+$");

    if (regex.match(text).hasMatch())
    {
        return true;
    }

    return false;
}

void ContainerAppOperateDialog::initUI()
{
    setIcon(QIcon(":/images/logo.png"));
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint | KiranTitlebarWindow::TitlebarCloseButtonHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setResizeable(false);
    setFixedSize(500, 400);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_save, Kiran::BUTTON_Default);

    ui->cb_type->addItem(tr("GUI app"), true);
    ui->cb_type->addItem(tr("Non-GUI"), false);

    ui->lineEdit_name->setMaxLength(50);
    ui->lineEdit_name->setPlaceholderText(tr("Please input 1 to 50 characters"));
    ui->lineEdit_path->setMaxLength(4096);
    ui->lineEdit_path->setPlaceholderText(tr("Please input 1 to 4096 characters"));

    ui->btn_tip_name->setIcon(QIcon(":/images/tips.svg"));
    ui->btn_tip_name->setStyleSheet("QToolTip{"
                                    "background-color: rgb(255,255,255);"
                                    "color:#000000;"
                                    "border:0px solid rgb(0,0,0);"
                                    "border-radius: 6px;"
                                    "outline:none; "
                                    "}"
                                    "QToolButton{border:none;}");
    ui->btn_tip_name->setToolTip(tr("The characters can only be letter or digit."));

    connect(ui->btn_cancel, &QPushButton::clicked, this, &ContainerAppOperateDialog::close);
    connect(ui->btn_save, &QPushButton::clicked, this, &ContainerAppOperateDialog::onSave);
}
