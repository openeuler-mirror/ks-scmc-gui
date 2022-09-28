/**
 * @file          /ks-scmc-gui/src/pages/container/container-manager/container-backup-page.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "container-backup-page.h"
#include <kiran-log/qt5-log-i.h>
#include <widget-property-helper.h>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QRadioButton>
#include <QTimer>
#include "message-dialog.h"
#include "notification-manager.h"
#define TIMEOUT 60000
using namespace grpc;

ContainerBackupPage::ContainerBackupPage(QWidget *parent) : TablePage(nullptr),
                                                            m_backupAddDlg(nullptr),
                                                            m_backupEditDlg(nullptr),
                                                            m_exportWindow(nullptr),
                                                            m_nodeId(-1),
                                                            m_containerId("")
{
    m_objId = InfoWorker::generateId(this);
    initTable();
    initButtons();
    initConnect();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [this] {
        updateInfo();
    });
}

ContainerBackupPage::~ContainerBackupPage()
{
    if (m_backupAddDlg)
    {
        delete m_backupAddDlg;
        m_backupAddDlg = nullptr;
    }
    if (m_backupEditDlg)
    {
        delete m_backupEditDlg;
        m_backupEditDlg = nullptr;
    }
    if (m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }
}

void ContainerBackupPage::updateInfo(QString keyword)
{
    KLOG_INFO() << "ContainerBackupPage updateInfo";
    clearCheckState();
    clearText();
    if (keyword.isEmpty())
    {
        if (m_nodeId >= 0 && !QString::fromStdString(m_containerId).isEmpty())
        {
            setBusy(true);
            InfoWorker::getInstance().listBackup(m_objId, m_nodeId, m_containerId);
            m_timer->start(TIMEOUT);
        }
    }
}

void ContainerBackupPage::updateBackupList(int nodeId, std::string containerId, QString containerStatus)
{
    if (nodeId >= 0 && !QString::fromStdString(containerId).isEmpty())
    {
        m_nodeId = nodeId;
        m_containerId = containerId;
        m_containerStatus = containerStatus;
        updateInfo();
    }
}

void ContainerBackupPage::onCreateBackupBtn()
{
    if (!m_backupAddDlg)
    {
        m_backupAddDlg = new ContainerBackupOperateDialog(BACKUP_OPERATE_TYPE_CREATE);
        m_backupAddDlg->setTitle(tr("Backup Addition"));
        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_backupAddDlg->move(screenGeometry.x() + (screenGeometry.width() - m_backupAddDlg->width()) / 2,
                             screenGeometry.y() + (screenGeometry.height() - m_backupAddDlg->height()) / 2);
        m_backupAddDlg->show();

        connect(m_backupAddDlg, &ContainerBackupOperateDialog::sigSave, this, &ContainerBackupPage::onBackupOperate);
        connect(m_backupAddDlg, &ContainerBackupOperateDialog::destroyed,
                [=] {
                    KLOG_INFO() << " m_backupAddDlg destroy";
                    m_backupAddDlg->deleteLater();
                    m_backupAddDlg = nullptr;
                });
    }
}

void ContainerBackupPage::onRemoveBackupBtn()
{
    QList<QMap<QString, QVariant>> infoMap = getCheckedItemInfo(1);
    if (!infoMap.empty())
    {
        auto ret = MessageDialog::message(tr("Delete Container Backup"),
                                          tr("Are you sure you want to delete the container backup?"),
                                          tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                          ":/images/warning.svg",
                                          MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
        if (ret == MessageDialog::StandardButton::Yes)
        {
            auto backupId = infoMap.at(0).value(BACKUP_ID).toInt();
            InfoWorker::getInstance().removeBackup(m_objId, m_nodeId, backupId);
        }
        else
            KLOG_INFO() << "cancel";
    }
}

void ContainerBackupPage::onBackupOperate(BackupOperateType type, QString desc)
{
    if (type == BACKUP_OPERATE_TYPE_CREATE)
        InfoWorker::getInstance().createBackup(m_objId, m_nodeId, m_containerId, desc.toStdString());
    else if (type == BACKUP_OPERATE_TYPE_EDIT)
        InfoWorker::getInstance().updateBackup(m_objId, m_nodeId, m_updateBackupId, desc.toStdString());
}

void ContainerBackupPage::onRemoveBackup(int row)
{
    auto item = getItem(row, 1);
    QMap<QString, QVariant> infoMap = item->data().toMap();
    auto ret = MessageDialog::message(tr("Delete Container Backup"),
                                      tr("Are you sure you want to delete the container backup?"),
                                      tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                      ":/images/warning.svg",
                                      MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
    if (ret == MessageDialog::StandardButton::Yes)
    {
        auto backupId = infoMap.value(BACKUP_ID).toInt();
        InfoWorker::getInstance().removeBackup(m_objId, m_nodeId, backupId);
    }
    else
        KLOG_INFO() << "cancel";
}

void ContainerBackupPage::onResumeBackup(int row)
{
    auto item = getItem(row, 1);
    QMap<QString, QVariant> infoMap = item->data().toMap();
    KLOG_INFO() << m_containerStatus;

    int ret = MessageDialog::message(tr("Resume Backup"),
                                     tr("Backup recovery confirmation"),
                                     tr("%1 backup is selected for recovery").arg(infoMap.value(BACKUP_NAME).toString()),
                                     ":/images/warning.svg",
                                     MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);

    if (ret == MessageDialog::StandardButton::Yes)
    {
        auto backupId = infoMap.value(BACKUP_ID).toInt();
        InfoWorker::getInstance().resumeBackup(m_objId, m_nodeId, m_containerId, backupId);
    }
    else
        KLOG_INFO() << "cancel";
}

void ContainerBackupPage::onUpdateBackup(int row)
{
    auto item = getItem(row, 1);
    QMap<QString, QVariant> infoMap = item->data().toMap();
    int64_t backupId = infoMap.value(BACKUP_ID).toInt();
    m_updateBackupId = backupId;

    if (!m_backupEditDlg)
    {
        m_backupEditDlg = new ContainerBackupOperateDialog(BACKUP_OPERATE_TYPE_EDIT);
        m_backupEditDlg->setTitle(tr("Backup Update"));
        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_backupEditDlg->move(screenGeometry.x() + (screenGeometry.width() - m_backupEditDlg->width()) / 2,
                              screenGeometry.y() + (screenGeometry.height() - m_backupEditDlg->height()) / 2);
        m_backupEditDlg->show();

        connect(m_backupEditDlg, &ContainerBackupOperateDialog::sigSave, this, &ContainerBackupPage::onBackupOperate);
        connect(m_backupEditDlg, &ContainerBackupOperateDialog::destroyed,
                [=] {
                    KLOG_INFO() << " m_backupEditDlg destroy";
                    m_backupEditDlg->deleteLater();
                    m_backupEditDlg = nullptr;
                });
    }
}

void ContainerBackupPage::onExportBackup(int row)
{
    KLOG_INFO() << "export backup " << row;
    if (m_exportWindow == nullptr)
    {
        m_exportWindow = createExportWindow();
        connect(m_exportWindow, &KiranTitlebarWindow::destroyed,
                [=] {
                    KLOG_INFO() << " export backup window destroy";
                    m_exportWindow->deleteLater();
                    m_exportWindow = nullptr;
                });
    }
    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    m_exportWindow->move(screenGeometry.x() + (screenGeometry.width() - m_exportWindow->width()) / 2,
                         screenGeometry.y() + (screenGeometry.height() - m_exportWindow->height()) / 2);
    m_exportWindow->raise();
    m_exportWindow->show();
}

void ContainerBackupPage::getListBackupFinished(const QString objId, const QPair<grpc::Status, container::ListBackupReply> &reply)
{
    KLOG_INFO() << "getListBackupFinished" << m_objId << objId;
    if (m_objId == objId)
    {
        setBusy(false);
        if (reply.first.ok())
        {
            setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
            clearTable();
            int size = reply.second.data_size();
            if (size <= 0)
            {
                setTableDefaultContent("-");
                return;
            }
            int row = 0;
            QMap<QString, QVariant> idMap;
            for (auto data : reply.second.data())
            {
                qint64 backupId = data.id();
                idMap.insert(BACKUP_ID, backupId);
                idMap.insert(BACKUP_NAME, data.backup_name().data());

                QStandardItem *itemCheck = new QStandardItem();
                itemCheck->setCheckable(true);

                QStandardItem *itemName = new QStandardItem(data.backup_name().data());
                itemName->setTextAlignment(Qt::AlignCenter);
                itemName->setData(QVariant::fromValue(idMap));

                int status = data.status();
                QStandardItem *itemStatus = new QStandardItem();
                itemStatus->setTextAlignment(Qt::AlignCenter);
                switch (status)
                {
                case 0:
                    itemStatus->setText(tr("On going"));
                    itemStatus->setForeground(QBrush(QColor("#00921b")));
                    break;
                case 1:
                    itemStatus->setText(tr("Successful"));
                    itemStatus->setForeground(QBrush(QColor("#00921b")));
                    break;
                case 2:
                    itemStatus->setText(tr("Failed"));
                    itemStatus->setForeground(QBrush(QColor("#d30000")));
                    break;
                }

                auto dt = QDateTime::fromSecsSinceEpoch(data.created_at());
                QStandardItem *startTime = new QStandardItem(dt.toString("yyyy/MM/dd hh:mm:ss"));
                startTime->setTextAlignment(Qt::AlignCenter);

                QString size = QString("%1M").arg(QString::number(data.image_size() / 1024 / 1024));  //字节转化成M
                QStandardItem *itemSize = new QStandardItem(size);
                itemSize->setTextAlignment(Qt::AlignCenter);

                QStandardItem *itemDesc = new QStandardItem(data.backup_desc().data());
                itemDesc->setTextAlignment(Qt::AlignCenter);

                setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemStatus << startTime << itemSize << itemDesc);
                row++;
            }
        }
        else
        {
            if (reply.first.error_code() == PERMISSION_DENIED)
                setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
            else
            {
                setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
                if (DEADLINE_EXCEEDED == reply.first.error_code())
                {
                    setTips(tr("Response timeout!"));
                }
            }
            setTableDefaultContent("-");
        }
    }
}

void ContainerBackupPage::getUpdateBackupFinished(const QString objId, const QPair<grpc::Status, container::UpdateBackupReply> &reply)
{
    KLOG_INFO() << "getUpdateBackupFinished" << m_objId << objId;
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            updateInfo();
            if (m_backupEditDlg)
                m_backupEditDlg->close();
        }
        else
        {
            MessageDialog::message(tr("Update Container Backup"),
                                   tr("Update container backup failed!"),
                                   tr("Error: %1").arg(reply.first.error_message().data()),
                                   ":/images/error.svg",
                                   MessageDialog::StandardButton::Ok);
        }
    }
}

void ContainerBackupPage::getCreateBackupFinished(const QString objId, const QPair<grpc::Status, container::CreateBackupReply> &reply)
{
    KLOG_INFO() << "getCreateBackupFinished" << m_objId << objId;
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            updateInfo();
            if (m_backupAddDlg)
                m_backupAddDlg->close();
        }
        else
        {
            MessageDialog::message(tr("Create Container Backup"),
                                   tr("Create container backup failed!"),
                                   tr("Error: %1").arg(reply.first.error_message().data()),
                                   ":/images/error.svg",
                                   MessageDialog::StandardButton::Ok);
        }
    }
}

void ContainerBackupPage::getResumeBackupFinished(const QString objId, const QPair<grpc::Status, container::ResumeBackupReply> &reply)
{
    KLOG_INFO() << "getResumeBackupFinished" << m_objId << objId;
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            m_containerId = reply.second.container_id().data();
            updateInfo();
            NotificationManager::sendNotify(tr("Resume container backup seccessful!"), "");
            //            MessageDialog::message(tr("Resume Container Backup"),
            //                                   tr("Resume container backup seccessful!"),
            //                                   "",
            //                                   ":/images/success.svg",
            //                                   MessageDialog::StandardButton::Ok);
        }
        else
        {
            NotificationManager::sendNotify(tr("Resume container backup failed!"),
                                            tr("Error: %1").arg(reply.first.error_message().data()));
            //            MessageDialog::message(tr("Resume Container Backup"),
            //                                   tr("Resume container backup failed!"),
            //                                   tr("Error: %1").arg(reply.first.error_message().data()),
            //                                   ":/images/error.svg",
            //                                   MessageDialog::StandardButton::Ok);
        }
    }
}

void ContainerBackupPage::getRemoveBackupFinished(const QString objId, const QPair<grpc::Status, container::RemoveBackupReply> &reply)
{
    KLOG_INFO() << "getRemoveBackupFinished" << m_objId << objId;
    if (m_objId == objId)
    {
        if (reply.first.ok())
        {
            updateInfo();
        }
        else
        {
            MessageDialog::message(tr("Remove Container Backup"),
                                   tr("Remove container backup failed!"),
                                   tr("Error: %1").arg(reply.first.error_message().data()),
                                   ":/images/error.svg",
                                   MessageDialog::StandardButton::Ok);
        }
    }
}

void ContainerBackupPage::initTable()
{
    QStringList tableHHeaderDate = {
        "",
        QString(tr("Backup version")),
        QString(tr("Backup status")),
        QString(tr("Backup start time")),
        QString(tr("Backup size")),
        QString(tr("Description")),
        QString(tr("Quick Actions"))};
    setHeaderSections(tableHHeaderDate);

    setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>{{ACTION_BUTTON_TYPE_BACKUP_RESUME, QPair<QString, QString>{tr("Resume"), tr("Resume")}},
                                                                                                   {ACTION_BUTTON_TYPE_BACKUP_UPDATE, QPair<QString, QString>{tr("Update"), tr("Update")}},
                                                                                                   {ACTION_BUTTON_TYPE_BACKUP_EXPORT, QPair<QString, QString>{tr("Export"), tr("Export")}},
                                                                                                   {ACTION_BUTTON_TYPE_BACKUP_REMOVE, QPair<QString, QString>{tr("Remove"), tr("Remove")}}});

    setTableDefaultContent("-");
    setHeaderCheckable(false);
    setTableSingleChoose(true);
    connect(this, &ContainerBackupPage::sigBackupResume, this, &ContainerBackupPage::onResumeBackup);
    connect(this, &ContainerBackupPage::sigBackupUpdate, this, &ContainerBackupPage::onUpdateBackup);
    connect(this, &ContainerBackupPage::sigBackupRemove, this, &ContainerBackupPage::onRemoveBackup);
    connect(this, &ContainerBackupPage::sigBackupExport, this, &ContainerBackupPage::onExportBackup);
}

void ContainerBackupPage::initButtons()
{
    QPushButton *btnCreate = new QPushButton(this);
    btnCreate->setText(tr("Create"));
    btnCreate->setObjectName("btnCreate");
    btnCreate->setFixedSize(QSize(78, 32));
    connect(btnCreate, &QPushButton::clicked, this, &ContainerBackupPage::onCreateBackupBtn);

    QPushButton *btnRemove = new QPushButton(this);
    btnRemove->setText(tr("Remove"));
    btnRemove->setObjectName("btnRemove");
    btnRemove->setObjectName("btnRemove");
    btnRemove->setStyleSheet("#btnRemove{background-color:#ff4b4b;"
                             "border:none;"
                             "border-radius: 4px;"
                             "color:#ffffff;}"
                             "#btnRemove:hover{ background-color:#ff6c6c;}"
                             "#btnRemove:focus{outline:none;}"
                             "#btnRemove:disabled{color:#919191;background:#393939;}");
    btnRemove->setFixedSize(QSize(78, 32));
    connect(btnRemove, &QPushButton::clicked, this, &ContainerBackupPage::onRemoveBackupBtn);

    addSingleOperationButton(btnCreate);
    addBatchOperationButtons(QList<QPushButton *>() << btnRemove);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
    setOpBtnEnabled(OPERATOR_BUTTON_TYPE_BATCH, false);
}

void ContainerBackupPage::initConnect()
{
    connect(&InfoWorker::getInstance(), &InfoWorker::createBackupFinished, this, &ContainerBackupPage::getCreateBackupFinished);
    connect(&InfoWorker::getInstance(), &InfoWorker::removeBackupFinished, this, &ContainerBackupPage::getRemoveBackupFinished);
    connect(&InfoWorker::getInstance(), &InfoWorker::listBackupFinished, this, &ContainerBackupPage::getListBackupFinished);
    connect(&InfoWorker::getInstance(), &InfoWorker::updateBackupFinished, this, &ContainerBackupPage::getUpdateBackupFinished);
    connect(&InfoWorker::getInstance(), &InfoWorker::resumeBackupFinished, this, &ContainerBackupPage::getResumeBackupFinished);
}

KiranTitlebarWindow *ContainerBackupPage::createExportWindow()
{
    KiranTitlebarWindow *window = new KiranTitlebarWindow(this);
    window->setAttribute(Qt::WA_DeleteOnClose, true);
    window->setTitle("Export");
    window->setIcon(QIcon(":/images/logo.png"));
    window->setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint | KiranTitlebarWindow::TitlebarCloseButtonHint);
    window->setResizeable(false);

    //创建内容窗口
    QWidget *content = new QWidget(window);
    QVBoxLayout *mainLayout = new QVBoxLayout(content);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QHBoxLayout *versionLayout = new QHBoxLayout;
    versionLayout->setMargin(0);
    versionLayout->setSpacing(10);
    QLabel *label = new QLabel(tr("Version"), content);
    QLineEdit *line = new QLineEdit(content);
    versionLayout->addWidget(label);
    versionLayout->addWidget(line);

    QHBoxLayout *radioBtnLayout = new QHBoxLayout;
    radioBtnLayout->setMargin(0);
    radioBtnLayout->setSpacing(10);
    QRadioButton *btnToLocal = new QRadioButton(tr("Export to local"), content);
    btnToLocal->setChecked(true);
    QRadioButton *btnToImage = new QRadioButton(tr("Export to image manager"), content);
    radioBtnLayout->addStretch();
    radioBtnLayout->addWidget(btnToLocal);
    radioBtnLayout->addWidget(btnToImage);
    radioBtnLayout->addStretch();

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);
    btnLayout->setSpacing(20);

    QPushButton *btnConfirm = new QPushButton(tr("Confirm"), content);
    btnConfirm->setFixedSize(100, 40);
    Kiran::WidgetPropertyHelper::setButtonType(btnConfirm, Kiran::BUTTON_Default);
    connect(btnConfirm, &QPushButton::clicked,
            [=] {

            });

    QPushButton *btnCancel = new QPushButton(tr("Cancel"), content);
    btnCancel->setFixedSize(100, 40);
    connect(btnCancel, &QPushButton::clicked, window, &KiranTitlebarWindow::close);

    btnLayout->addStretch();
    btnLayout->addWidget(btnConfirm);
    btnLayout->addWidget(btnCancel);

    mainLayout->addLayout(versionLayout);
    mainLayout->addLayout(radioBtnLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    window->setWindowContentWidget(content);
    window->resize(500, 400);

    return window;
}
