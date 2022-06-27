/**
 * @file          /ks-scmc-gui/src/pages/node/node-operate-dialog.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef NODEOPERATEDIALOG_H
#define NODEOPERATEDIALOG_H

#include <kiran-titlebar-window.h>
#include <QWidget>

namespace Ui
{
class NodeOperateDialog;
}

enum NodeOperateType
{
    NODE_OPERATE_TYPE_CREATE,
    NODE_OPERATE_TYPE_EDIT
};

class NodeOperateDialog : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit NodeOperateDialog(NodeOperateType type, QWidget *parent = nullptr);
    ~NodeOperateDialog();
    void setNodeInfo(int64_t nodeId, QString name, QString addr, QString comment);
    NodeOperateType getType();
    int64_t getNodeId();

signals:
    void sigSave(QMap<QString, QString> newNodeInfo);

private slots:
    void onSave();
    void onCancel();
    void limitLength();

private:
    Ui::NodeOperateDialog *ui;
    NodeOperateType m_type;
    int64_t m_nodeId;
};

#endif  // NODEOPERATEDIALOG_H
