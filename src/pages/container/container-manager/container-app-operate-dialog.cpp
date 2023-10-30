#include "container-app-operate-dialog.h"
#include <widget-property-helper.h>
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
    ui->label_tip->clear();

    emit sigSave(ui->lineEdit_name->text(),
                 ui->lineEdit_path->text(),
                 ui->cb_type->currentData().toInt());
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

    connect(ui->btn_cancel, &QPushButton::clicked, this, &ContainerAppOperateDialog::close);
    connect(ui->btn_save, &QPushButton::clicked, this, &ContainerAppOperateDialog::onSave);
}
