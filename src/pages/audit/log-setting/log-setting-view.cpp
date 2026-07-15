/**
 * @file          src/pages/audit/log-setting/log-setting-view.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "log-setting-view.h"
#include <kiran-log/qt5-log-i.h>
#include <QIntValidator>
#include <QLabel>
#include <QVBoxLayout>
#include "notification-manager.h"
LogSettingView::LogSettingView(QWidget *parent) : Page(parent),
                                                  m_edit(nullptr),
                                                  m_btn_cancel(nullptr),
                                                  m_btn_confirm(nullptr)
{
    m_ObjId = InfoWorker::generateId(this);
    initUI();
    initConnect();
}

LogSettingView::~LogSettingView()
{
}

void LogSettingView::updateInfo(QString keyword)
{
    InfoWorker::getInstance().getLog(m_ObjId);
}

void LogSettingView::setPeriod(int days)
{
    m_edit->setText(QString::number(days));
}

int LogSettingView::getPeriod()
{
    return m_edit->text().toInt();
}

void LogSettingView::initConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::loggingGetLogFinished, this, &LogSettingView::getLogFinishedResult);
    connect(&InfoWorker::getInstance(), &InfoWorker::loggingSetLogFinished, this, &LogSettingView::setLogFinishedResult);
}

void LogSettingView::updateUI()
{
    updateInfo();
}

void LogSettingView::setLogFinishedResult(const QString objId, const QPair<grpc::Status, logging::SetLogReply> &reply)
{
    if (m_ObjId != objId)
        return;

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "set log setting result failed: " << reply.first.error_message().data();
        NotificationManager::sendNotify(tr("Failed to set log save days!"), "");
        return;
    }

    NotificationManager::sendNotify(tr("Set log save days successfully!"), "");
}
void LogSettingView::getLogFinishedResult(const QString objId, const QPair<grpc::Status, logging::GetLogReply> &reply)
{
    if (m_ObjId != objId)
        return;

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Get log setting result failed: " << reply.first.error_message().data();
        return;
    }

    setPeriod(reply.second.save_log_days());
}

void LogSettingView::initUI()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(24, 16, 0, 24);
    mainLayout->setSpacing(16);

    // 添加日期设置输入框
    auto layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(10);
    auto label = new QLabel(this);
    label->setText(tr("Log Retention Period (Month)"));

    m_edit = new QLineEdit(this);
    m_edit->setPlaceholderText(tr("Please enter a number greater than or equal to 6"));
    m_edit->setMaxLength(2);
    m_edit->setValidator(new QIntValidator(6, 99, this));
    m_edit->setFixedSize(598, 40);

    layout->addWidget(label);
    layout->addWidget(m_edit);

    // 添加按钮
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(24);
    m_btn_confirm = new QPushButton(this);
    m_btn_confirm->setObjectName("btn_confirm");
    m_btn_confirm->setText(tr("Confirm"));
    m_btn_confirm->setFixedSize(78, 32);

    m_btn_cancel = new QPushButton(this);
    m_btn_cancel->setText(tr("Cancel"));
    m_btn_cancel->setObjectName("btn_cancel");
    m_btn_cancel->setFixedSize(78, 32);

    m_btn_confirm->setStyleSheet("#btn_confirm{background-color:#2eb3ff;"
                                 "border:none;"
                                 "border-radius: 4px;"
                                 "color:#ffffff;}"
                                 "#btn_confirm:hover{ background-color:#77ceff;}"
                                 "#btn_confirm:focus{outline:none;}");
    m_btn_cancel->setStyleSheet("#btn_cancel{background-color:#393939;"
                                "border:none;"
                                "border-radius: 4px;"
                                "color:#ffffff;}"
                                "#btn_cancel:hover{ background-color:#454545;}"
                                "#btn_cancel:focus{outline:none;}");

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_btn_confirm);
    buttonLayout->addWidget(m_btn_cancel);
    buttonLayout->addStretch();

    mainLayout->addLayout(layout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    connect(m_btn_cancel, &QPushButton::clicked, this, &LogSettingView::updateUI);
    connect(m_btn_confirm, &QPushButton::clicked, [this]()
            { 
                auto month = m_edit->text().toInt();
                if (month < 6)
                {
                    m_edit->clear();
                }
                else
                {
                    InfoWorker::getInstance().setLog(m_ObjId, month * 30);
                } });
}