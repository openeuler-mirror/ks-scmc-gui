#ifndef NODEADDITIONDIALOG_H
#define NODEADDITIONDIALOG_H

#include <QWidget>

namespace Ui
{
class NodeAdditionDialog;
}

class NodeAdditionDialog : public QWidget
{
    Q_OBJECT

public:
    explicit NodeAdditionDialog(QWidget *parent = nullptr);
    ~NodeAdditionDialog();

signals:
    void sigSave(QMap<QString, QString> newNodeInfo);

private slots:
    void onSave();
    void onCancel();

private:
    Ui::NodeAdditionDialog *ui;
};

#endif  // NODEADDITIONDIALOG_H
