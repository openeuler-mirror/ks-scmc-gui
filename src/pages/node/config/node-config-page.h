#ifndef NODECONFIGPAGE_H
#define NODECONFIGPAGE_H

#include <QWidget>
#include "info-worker.h"
#include "page.h"
namespace Ui
{
class NodeConfigPage;
}

class NodeConfigPage : public Page
{
    Q_OBJECT

public:
    explicit NodeConfigPage(QWidget *parent = nullptr);
    ~NodeConfigPage();
    void setNodeId(int64_t nodeId);
    void updateInfo(QString keyword = "");

private slots:
    void onSave();
    void onCancel();
    void getListResult(const QString objId, const QPair<grpc::Status, node::ListReply> &);
    void getUpdateResult(const QString objId, const QPair<grpc::Status, node::UpdateReply> &);

private:
    void updateUI(bool editable);

private:
    Ui::NodeConfigPage *ui;
    QString m_objId;
    bool m_editable;
    int64_t m_nodeId;
    std::string m_nodeName;
    int64_t m_cpuLimit;
    int64_t m_memoryLimit;
    int64_t m_diskLimit;
};

#endif  // NODECONFIGPAGE_H
