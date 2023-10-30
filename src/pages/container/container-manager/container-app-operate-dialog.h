#ifndef CONTAINERAPPOPERATEDIALOG_H
#define CONTAINERAPPOPERATEDIALOG_H

#include <kiran-titlebar-window.h>
#include <QWidget>
namespace Ui
{
class ContainerAppOperateDialog;
}

enum AppOperateType
{
    APP_OPERATE_TYPE_CREATE,
    APP_OPERATE_TYPE_EDIT
};

class ContainerAppOperateDialog : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit ContainerAppOperateDialog(AppOperateType type, QWidget *parent = nullptr);
    explicit ContainerAppOperateDialog(int appId, QString name, QString path, bool isGUI, AppOperateType type, QWidget *parent = nullptr);
    ~ContainerAppOperateDialog();
    int getAppId();
    AppOperateType getType();

private:
    void onSave();

signals:
    void sigSave(const QString name, const QString path, const bool isGui);

private:
    void initUI();

private:
    Ui::ContainerAppOperateDialog *ui;
    AppOperateType m_type;
    int m_appId;
};

#endif  // CONTAINERAPPOPERATEDIALOG_H
