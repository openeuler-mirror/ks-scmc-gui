#ifndef PASSWDUPDATEDIALOG_H
#define PASSWDUPDATEDIALOG_H

#include <kiran-titlebar-window.h>
#include <QWidget>

namespace Ui
{
class PasswdUpdateDialog;
}

class PasswdUpdateDialog : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit PasswdUpdateDialog(QString userName, QWidget *parent = nullptr);
    ~PasswdUpdateDialog();

private:
    Ui::PasswdUpdateDialog *ui;
};

#endif  // PASSWDUPDATEDIALOG_H
