/**
 * @file          src/pages/node/IPC/ipc-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "ipc-page.h"
#include <kiran-titlebar-window.h>
#include <widget-property-helper.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

IPCPage::IPCPage(QWidget *parent) : TablePage(parent),
                                    m_createDialog(nullptr)
{
    initButtons();
    initTable();
    initConnect();
}

void IPCPage::updateInfo(QString keyword)
{
}

void IPCPage::createIPC()
{
}
void IPCPage::deleteIPC()
{
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
    m_edit->setPlaceholderText(tr("Please input 1 to 50 characters"));
    m_edit->setMaxLength(50);
    m_edit->setFixedHeight(36);
    layout->addWidget(label);
    layout->addWidget(m_edit);

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
void IPCPage::initButtons()
{
    auto btnCreate = new QPushButton(this);
    btnCreate->setText(tr("Create"));
    btnCreate->setObjectName("btnCreate");
    btnCreate->setFixedSize(QSize(78, 32));
    Kiran::WidgetPropertyHelper::setButtonType(btnCreate, Kiran::BUTTON_Default);
    addSingleOperationButton(btnCreate);
    connect(btnCreate, &QPushButton::clicked, this, &IPCPage::popupCreateDialog);

    auto btnDelete = new QPushButton(this);
    btnDelete->setText(tr("Delete"));
    btnDelete->setObjectName("btnDelete");
    btnDelete->setFixedSize(QSize(78, 32));
    connect(btnDelete, &QPushButton::clicked, this, &IPCPage::deleteIPC);

    addBatchOperationButtons(QList<QPushButton *>() << btnDelete);
    // TODO: 为了测试
    // setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void IPCPage::initTable()
{
    QStringList tableHHeaderDate = {
        "",
        QString(tr("IPC")),
        QString(tr("Container")),
        QString(tr("Create Time"))};
    setHeaderSections(tableHHeaderDate);
    setHeaderCheckable(false);
    setTableDefaultContent("-");
    setTableSingleChoose(true);
}

void IPCPage::initConnect()
{
}
