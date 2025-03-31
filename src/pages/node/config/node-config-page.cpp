/**
 * @file          src/pages/node/config/node-config-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "node-config-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QIntValidator>
#include <QKeyEvent>
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

    ui->lineEdit_cpu->installEventFilter(this);
    ui->lineEdit_memory->installEventFilter(this);
    ui->lineEdit_disk->installEventFilter(this);

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
    // interface getinfo
    InfoWorker::getInstance().listNode(m_objId);
}

bool NodeConfigPage::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->lineEdit_cpu || watched == ui->lineEdit_disk || watched == ui->lineEdit_memory)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            //在输入框中过滤逗号“，”，防止用户输入，解决string转数字时不能识别逗号导致转失败的问题
            if (keyEvent->key() == Qt::Key_Comma)
                return true;
            else
                return false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return QWidget::eventFilter(watched, event);
    }
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
        limit->set_disk_limit(ui->lineEdit_disk->text().toInt());
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
    if (m_objId != objId)
        return;

    if (m_nodeId <= 0)
        return;

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Get node list result failed:" << reply.first.error_message().data();
        return;
    }

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
            KLOG_DEBUG() << "Total cpu: " << cpuTotal
                         << "Total memory: " << memoryTotal
                         << "Total disk: " << diskTotal;

            ui->lineEdit_cpu->setPlaceholderText(tr("Maximum %1 cores").arg(cpuTotal));
            if (!ui->lineEdit_cpu->validator())
            {
                QIntValidator *v = new QIntValidator(0, cpuTotal, this);
                ui->lineEdit_cpu->setValidator(v);
            }

            //由于使用QDoubleValidator会导致输入字符时判断不正确，因此改用QIntValidator。
            //所有将原来的GB单位修改为MB，精度更小，方便设置阈值
            ui->lineEdit_disk->setPlaceholderText(tr("Maximum %1 MB").arg(diskTotal));  //
            if (!ui->lineEdit_disk->validator())
            {
                QIntValidator *v = new QIntValidator(0, diskTotal, this);
                ui->lineEdit_disk->setValidator(v);
            }

            ui->lineEdit_memory->setPlaceholderText(tr("Maximum %1 MB").arg(memoryTotal));
            if (!ui->lineEdit_memory->validator())
            {
                QIntValidator *v = new QIntValidator(0, memoryTotal, this);
                ui->lineEdit_memory->setValidator(v);
            }

            auto limit = node.rsc_limit();
            m_cpuLimit = limit.cpu_limit();
            m_memoryLimit = limit.memory_limit();
            m_diskLimit = limit.disk_limit();
            ui->lineEdit_cpu->setText(QString::number(m_cpuLimit));
            ui->lineEdit_memory->setText(QString::number(m_memoryLimit));
            ui->lineEdit_disk->setText(QString::number(m_diskLimit));
            break;
        }
    }
}

void NodeConfigPage::getUpdateResult(const QString objId, const QPair<grpc::Status, node::UpdateReply> &reply)
{
    if (m_objId != objId)
        return;

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
