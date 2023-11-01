#include "node-config-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QDoubleValidator>
#include "notification-manager.h"
#include "ui_node-config-page.h"
NodeConfigPage::NodeConfigPage(QWidget *parent) : Page(parent),
                                                  ui(new Ui::NodeConfigPage),
                                                  m_nodeId(-1)
{
    ui->setupUi(this);
    m_objId = InfoWorker::generateId(this);

    m_editable = false;
    updateUI(m_editable);

    ui->btn_save->setStyleSheet("#btn_save{background-color:#2eb3ff;"
                                "border:none;"
                                "border-radius: 4px;"
                                "color:#ffffff;}"
                                "#btn_save:hover{ background-color:#77ceff;}"
                                "#btn_save:focus{outline:none;}");
    ui->btn_cancel->setStyleSheet("#btn_cancel{background-color:#393939;"
                                  "border:none;"
                                  "border-radius: 4px;"
                                  "color:#ffffff;}"
                                  "#btn_cancel:hover{ background-color:#454545;}"
                                  "#btn_cancel:focus{outline:none;}");

    connect(ui->btn_save, &QPushButton::clicked, this, &NodeConfigPage::onSave);
    connect(ui->btn_cancel, &QPushButton::clicked, this, &NodeConfigPage::onCancel);
    connect(&InfoWorker::getInstance(), &InfoWorker::listNodeFinished, this, &NodeConfigPage::getListResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::updateNodeFinished, this, &NodeConfigPage::getUpdateResult);
}

NodeConfigPage::~NodeConfigPage()
{
    delete ui;
}

void NodeConfigPage::setNodeId(int64_t nodeId)
{
    m_nodeId = nodeId;
}

void NodeConfigPage::updateInfo(QString keyword)
{
    //interface getinfo
    InfoWorker::getInstance().listNode(m_objId);
}

void NodeConfigPage::onSave()
{
    if (m_editable)
    {
        //interface
        node::UpdateRequest req;
        req.set_node_id(m_nodeId);
        req.set_name(m_nodeName);
        auto limit = req.mutable_rsc_limit();
        limit->set_cpu_limit(ui->lineEdit_cpu->text().toInt());
        limit->set_disk_limit(ui->lineEdit_disk->text().toInt() * 1024);  //to MB
        limit->set_memory_limit(ui->lineEdit_memory->text().toInt());

        InfoWorker::getInstance().updateNode(m_objId, req);
    }

    m_editable = !m_editable;
    updateUI(m_editable);
}

void NodeConfigPage::onCancel()
{
    if (m_editable)
    {
        ui->lineEdit_cpu->setText(QString::number(m_cpuLimit));
        ui->lineEdit_memory->setText(QString::number(m_memoryLimit));
        ui->lineEdit_disk->setText(QString::number(m_diskLimit));
        m_editable = !m_editable;
        updateUI(m_editable);
    }
}

void NodeConfigPage::getListResult(const QString objId, const QPair<grpc::Status, node::ListReply> &reply)
{
    KLOG_INFO() << "getListResult" << m_objId << objId;
    KLOG_INFO() << "node id:" << m_nodeId;
    if (m_objId == objId && m_nodeId > 0)
    {
        if (reply.first.ok())
        {
            int size = reply.second.nodes_size();
            if (size <= 0)
            {
                updateUI(false);
                ui->btn_save->setDisabled(true);
                return;
            }
            for (auto node : reply.second.nodes())
            {
                if (node.id() == m_nodeId)
                {
                    m_nodeName = node.name();
                    auto cpuTotal = node.status().cpu_stat().total();
                    auto memoryTotal = node.status().mem_stat().total();
                    auto diskTotal = node.status().disk_stat().total();
                    KLOG_INFO() << cpuTotal << memoryTotal << diskTotal;
                    if (ui->lineEdit_cpu->placeholderText().isEmpty())
                        ui->lineEdit_cpu->setPlaceholderText(tr("Maximum %1 cores").arg(cpuTotal));
                    if (!ui->lineEdit_cpu->validator())
                    {
                        QDoubleValidator *v = new QDoubleValidator(0.01, cpuTotal, 2, this);
                        v->setNotation(QDoubleValidator::StandardNotation);
                        ui->lineEdit_cpu->setValidator(v);
                    }

                    if (ui->lineEdit_disk->placeholderText().isEmpty())
                        ui->lineEdit_disk->setPlaceholderText(tr("Maximum %1 GB").arg(diskTotal / 1024));  //to GB
                    if (!ui->lineEdit_disk->validator())
                    {
                        QDoubleValidator *v = new QDoubleValidator(0.01, diskTotal / 1024, 2, this);
                        v->setNotation(QDoubleValidator::StandardNotation);
                        ui->lineEdit_disk->setValidator(v);
                    }

                    if (ui->lineEdit_memory->placeholderText().isEmpty())
                        ui->lineEdit_memory->setPlaceholderText(tr("Maximum %1 MB").arg(memoryTotal));
                    if (!ui->lineEdit_memory->validator())
                    {
                        QDoubleValidator *v = new QDoubleValidator(0.01, memoryTotal, 2, this);
                        v->setNotation(QDoubleValidator::StandardNotation);
                        ui->lineEdit_memory->setValidator(v);
                    }

                    auto limit = node.rsc_limit();
                    m_cpuLimit = limit.cpu_limit();
                    m_memoryLimit = limit.memory_limit();
                    m_diskLimit = limit.disk_limit() / 1024;
                    ui->lineEdit_cpu->setText(QString::number(m_cpuLimit));
                    ui->lineEdit_memory->setText(QString::number(m_memoryLimit));
                    ui->lineEdit_disk->setText(QString::number(m_diskLimit));
                    break;
                }
            }
        }
    }
}

void NodeConfigPage::getUpdateResult(const QString objId, const QPair<grpc::Status, node::UpdateReply> &reply)
{
    KLOG_INFO() << "getUpdateResult" << m_objId << objId << m_nodeId;
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            NotificationManager::sendNotify(tr("Update node config successful!"), "");
            updateInfo();
        }
        else
        {
            NotificationManager::sendNotify(tr("Update node config failed!"), reply.first.error_message().data());
            //还原为修改前的数据
            ui->lineEdit_cpu->setText(QString::number(m_cpuLimit));
            ui->lineEdit_memory->setText(QString::number(m_memoryLimit));
            ui->lineEdit_disk->setText(QString::number(m_diskLimit));
        }
    }
}

void NodeConfigPage::updateUI(bool editable)
{
    ui->lineEdit_cpu->setDisabled(!editable);
    ui->lineEdit_disk->setDisabled(!editable);
    ui->lineEdit_memory->setDisabled(!editable);

    if (editable)
        ui->lineEdit_cpu->setFocus();

    QString text = editable ? tr("Save") : tr("Edit");
    ui->btn_save->setText(text);
}
