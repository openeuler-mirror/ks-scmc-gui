#include "passwd-update-dialog.h"
#include "ui_passwd-update-dialog.h"

PasswdUpdateDialog::PasswdUpdateDialog(QString userName, QWidget *parent) : KiranTitlebarWindow(parent),
                                                                            ui(new Ui::PasswdUpdateDialog)
{
    ui->setupUi(getWindowContentWidget());
}

PasswdUpdateDialog::~PasswdUpdateDialog()
{
    delete ui;
}
