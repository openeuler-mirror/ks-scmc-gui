/**
 * @file          src/pages/node/IPC/ipc-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "ipc-page.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-titlebar-window.h>
#include <widget-property-helper.h>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "ui_ipc-page.h"

IPCPage::IPCPage(QWidget *parent) : Page(parent),
                                    ui(new Ui::IPCPage),
                                    m_nodeID(-1),
                                    m_createDialog(nullptr)
{
    ui->setupUi(this);
    m_objId = IPC::generateId(this);
    initUI();
    initConnect();
}

void IPCPage::updateInfo(QString keyword)
{
    // TODO:待后台代码完善即可调用
    // IPC::getInstance().listIPC(m_objId);
}

void IPCPage::setNodeID(qint64 nodeID)
{
    m_nodeID = nodeID;
}

void IPCPage::createIPC()
{
}
void IPCPage::deleteIPC()
{
}

void IPCPage::listIPCFinished(QString objId, const QPair<grpc::Status, ipcbase::IpcListReply> &reply)
{
    if (m_objId != objId)
        return;

    ui->tableView->clearTable();

    if (!reply.first.ok())
    {
        if (reply.first.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED)
        {
            setTips(tr("Response timeout!"));
        }
        return;
    }

    // 返回成功后使能创建按钮
    ui->btn_create->setEnabled(true);

    int size = reply.second.ipcname_ids_size();
    if (size <= 0)
    {
        return;
    }

    QList<IpcInfo> ipcInfos;
    for (auto info : reply.second.ipcname_ids())
    {
        KLOG_DEBUG() << "Get ipc list. "
                     << "ipc name:" << info.name().data() << "start time:" << info.start_time();

        QString name = info.name().data();
        QString createTime = "-";
        if (info.start_time() != 0)
        {
            auto dt = QDateTime::fromSecsSinceEpoch(info.start_time());
            createTime = dt.toString("yyyy/MM/dd hh:mm:ss");
        }

        QString ipcContainers = "";
        for (auto container : info.containers())
        {
            ipcContainers = ipcContainers + "," + QString(container.data());
        }

        auto ipcInfo = IpcInfo{.selected = false,
                               .ipcName = name,
                               .containers = ipcContainers,
                               .createTime = createTime};
        ipcInfos.push_back(ipcInfo);
    }
    ui->tableView->setIpcInfos(ipcInfos);
}

void IPCPage::createIPCFinished(const QString objId, const QPair<grpc::Status, ipcbase::IpcCreateReply> &reply)
{
    if (m_objId != objId)
        return;

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Failed to create node:" << m_nodeID << "ipc:" << reply.first.error_message().data();
        return;
    }

    updateInfo();
}

void IPCPage::removeIPCFinished(const QString objId, const QPair<grpc::Status, ipcbase::IpcRemoveReply> &reply)
{
    if (m_objId != objId)
        return;

    if (!reply.first.ok())
    {
        KLOG_WARNING() << "Failed to remove node:" << m_nodeID << "ipc list:" << reply.first.error_message().data();
        return;
    }
    updateInfo();
}

void IPCPage::initUI()
{
    // 在搜索框右侧添加搜索按钮
    QHBoxLayout *layout = new QHBoxLayout(ui->lineEdit_search);
    layout->setMargin(0);
    layout->setContentsMargins(10, 0, 10, 0);

    QPushButton *btn_search = new QPushButton(ui->lineEdit_search);
    btn_search->setObjectName("btn_search");
    btn_search->setFixedSize(QSize(16, 16));
    btn_search->setIcon(QIcon(":/images/search.svg"));
    btn_search->setStyleSheet("#btn_search{background:transparent;border:none;}"
                              "#btn_search:focus{outline:none;}");
    btn_search->setCursor(Qt::PointingHandCursor);
    layout->addStretch();
    layout->addWidget(btn_search);
    ui->lineEdit_search->setTextMargins(0, 0, btn_search->width() + 10, 0);

    // 设置“创建”、“删除”按钮样式
    ui->btn_create->setDisabled(true);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_create, Kiran::BUTTON_Default);

    ui->btn_delete->setDisabled(true);
    ui->btn_delete->setObjectName("btnDelete");
    ui->btn_delete->setStyleSheet("#btnDelete{background-color:#ff4b4b;"
                                  "border:none;"
                                  "border-radius: 4px;"
                                  "color:#ffffff;}"
                                  "#btnDelete:hover{ background-color:#ff6c6c;}"
                                  "#btnDelete:focus{outline:none;}"
                                  "#btnDelete:disabled{color:#919191;background:#393939;}");

    connect(ui->btn_delete, &QPushButton::clicked, this, &IPCPage::deleteIPC);
    connect(ui->btn_create, &QPushButton::clicked, this, &IPCPage::popupCreateDialog);
    connect(btn_search, &QPushButton::clicked, this, &IPCPage::search);
    connect(ui->lineEdit_search, &QLineEdit::returnPressed, this, &IPCPage::search);
    connect(ui->btn_refresh, &QToolButton::clicked, this, &IPCPage::refresh);
}

void IPCPage::initCreateDialog()
{
    auto window = new KiranTitlebarWindow(this);
    window->setAttribute(Qt::WA_DeleteOnClose, true);
    window->setWindowModality(Qt::ApplicationModal);
    window->setTitle(tr("Create IPC"));
    window->setIcon(QIcon(":/images/logo.png"));
    window->setButtonHints(KiranTitlebarWindow::TitlebarCloseButtonHint);
    window->setResizeable(false);

    // 创建内容窗口
    auto content = new QWidget(window);
    auto mainLayout = new QVBoxLayout(content);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    auto layout = new QHBoxLayout();
    layout->setSpacing(20);
    layout->setContentsMargins(0, 0, 0, 0);
    auto label = new QLabel(tr("IPC Name"), content);
    m_edit = new QLineEdit(content);
    m_edit->setPlaceholderText(tr("Please input 2 to 40 characters"));
    m_edit->setMaxLength(40);
    m_edit->setFixedHeight(36);

    auto tipBtn = new QToolButton(content);
    tipBtn->setIcon(QIcon(":/images/tips.svg"));
    tipBtn->setToolTip(tr("Only letter, digit or ._- three special characters;\nThe first characters must be letter or digit"));
    tipBtn->setFixedSize(16, 16);

    layout->addWidget(label);
    layout->addWidget(m_edit);
    layout->addWidget(tipBtn);

    auto btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(20);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    auto btnCancel = new QPushButton(tr("Cancel"), content);
    btnCancel->setFixedSize(100, 36);
    auto btnConfirm = new QPushButton(tr("Confirm"), content);
    btnConfirm->setFixedSize(100, 36);
    Kiran::WidgetPropertyHelper::setButtonType(btnConfirm, Kiran::BUTTON_Default);
    btnLayout->addStretch();
    btnLayout->addWidget(btnConfirm);
    btnLayout->addWidget(btnCancel);

    mainLayout->addLayout(layout);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    window->setWindowContentWidget(content);
    window->resize(500, 250);

    connect(btnCancel, &QPushButton::clicked, window, &IPCPage::hide);
    connect(btnConfirm, &QPushButton::clicked, this, &IPCPage::createIPC);

    m_createDialog = window;
}

void IPCPage::search()
{
    auto text = ui->lineEdit_search->text();
    ui->tableView->searchTextChanged(text);
}

void IPCPage::refresh()
{
    updateInfo();
}
void IPCPage::clearText()
{
    ui->lineEdit_search->clear();
}

void IPCPage::setTips(const QString &tips)
{
    ui->label_tips->setText(tips);
    m_timerID = this->startTimer(10000);  // 10秒后提示消失
}

void IPCPage::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerID)
    {
        ui->label_tips->clear();
        killTimer(m_timerID);
    }
}
void IPCPage::popupCreateDialog()
{
    if (!m_createDialog)
    {
        initCreateDialog();
    }
    m_edit->clear();
    m_edit->setFocus();

    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    m_createDialog->move(screenGeometry.x() + (screenGeometry.width() - m_createDialog->width()) / 2,
                         screenGeometry.y() + (screenGeometry.height() - m_createDialog->height()) / 2);
    m_createDialog->raise();
    m_createDialog->show();
}

void IPCPage::initConnect()
{
    connect(&IPC::getInstance(), &IPC::listIPCFinished, this, &IPCPage::listIPCFinished);
    connect(&IPC::getInstance(), &IPC::createIPCFinished, this, &IPCPage::createIPCFinished);
    connect(&IPC::getInstance(), &IPC::removeIPCFinished, this, &IPCPage::removeIPCFinished);
}
