#include "container-backup-page.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include "backup-addition-dialog.h"
ContainerBackupPage::ContainerBackupPage(QWidget *parent) : TablePage(nullptr),
                                                            m_backupAddDlg(nullptr),
                                                            m_nodeId(-1),
                                                            m_containerId("")
{
    initTable();
    initButtons();
    initConnect();
}

ContainerBackupPage::~ContainerBackupPage()
{
    if (m_backupAddDlg)
    {
        delete m_backupAddDlg;
        m_backupAddDlg = nullptr;
    }
}

void ContainerBackupPage::updateInfo(QString keyword)
{
    KLOG_INFO() << "ContainerBackupPage updateInfo";
    clearText();
    if (keyword.isEmpty())
    {
        //initConnect();
        //gRPC->拿数据->填充内容
        //InfoWorker::getInstance().listBackup();
    }
}

void ContainerBackupPage::updateBackupList(int nodeId, std::string containerId)
{
    if (nodeId >= 0 && !QString::fromStdString(containerId).isEmpty())
    {
        m_nodeId = nodeId;
        m_containerId = containerId;
        InfoWorker::getInstance().listBackup(nodeId, containerId);
    }
}

void ContainerBackupPage::onCreateBackupBtn()
{
    if (!m_backupAddDlg)
    {
        m_backupAddDlg = new BackupAdditionDialog();
        int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
        QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
        m_backupAddDlg->move(screenGeometry.x() + (screenGeometry.width() - m_backupAddDlg->width()) / 2,
                             screenGeometry.y() + (screenGeometry.height() - m_backupAddDlg->height()) / 2);
        m_backupAddDlg->show();

        connect(m_backupAddDlg, &BackupAdditionDialog::sigSave, this, &ContainerBackupPage::onCreateBackup);
        connect(m_backupAddDlg, &BackupAdditionDialog::destroyed,
                [=] {
                    KLOG_INFO() << " m_backupAddDlg destroy";
                    m_backupAddDlg->deleteLater();
                    m_backupAddDlg = nullptr;
                });
    }
}

void ContainerBackupPage::onRemoveBackupBtn()
{
}

void ContainerBackupPage::onRemoveBackup(int row)
{
}

void ContainerBackupPage::onResumeBackup(int row)
{
}

void ContainerBackupPage::onUpdateBackup(int row)
{
}

void ContainerBackupPage::onCreateBackup(QString desc)
{
    InfoWorker::getInstance().createBackup(m_nodeId, m_containerId, desc.toStdString());
}

void ContainerBackupPage::getListBackupFinished(const QPair<grpc::Status, container::ListBackupReply> &reply)
{
    KLOG_INFO() << "getListBackupFinished";
    if (reply.first.ok())
    {
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, true);
        int size = reply.second.data_size();
        if (size <= 0)
        {
            setTableDefaultContent("-");
            return;
        }
        clearTable();
        setHeaderCheckable(true);
        int row = 0;
        QMap<QString, QVariant> idMap;
        for (auto data : reply.second.data())
        {
            qint64 backupId = data.id();
            idMap.insert(BACKUP_ID, backupId);

            QStandardItem *itemCheck = new QStandardItem();
            itemCheck->setCheckable(true);

            QStandardItem *itemName = new QStandardItem(data.backup_name().data());
            itemName->setData(QVariant::fromValue(idMap));
            itemName->setTextAlignment(Qt::AlignCenter);

            int status = data.status();
            QString strStatus;
            if (status == 0)
                strStatus = tr("Backing up");
            else if (status == 1)
                strStatus = tr("Successful");
            else if (status == 2)
                strStatus == tr("Failed");

            QStandardItem *itemStatus = new QStandardItem(strStatus);
            itemStatus->setTextAlignment(Qt::AlignCenter);

            auto dt = QDateTime::fromSecsSinceEpoch(data.created_at());
            QStandardItem *startTime = new QStandardItem(dt.toString("yyyy/MM/dd hh:mm:ss"));
            startTime->setTextAlignment(Qt::AlignCenter);

            QStandardItem *itemPath = new QStandardItem(data.file_path().data());
            itemPath->setTextAlignment(Qt::AlignCenter);

            QStandardItem *itemSize = new QStandardItem(data.file_size());
            itemSize->setTextAlignment(Qt::AlignCenter);

            QStandardItem *itemDesc = new QStandardItem(data.backup_desc().data());
            itemDesc->setTextAlignment(Qt::AlignCenter);

            setTableItems(row, 0, QList<QStandardItem *>() << itemCheck << itemName << itemStatus << startTime << itemPath << itemSize << itemDesc);
            row++;
        }
    }
    else
    {
        setTableDefaultContent("-");
        setOpBtnEnabled(OPERATOR_BUTTON_TYPE_SINGLE, false);
        setHeaderCheckable(false);
    }
}

void ContainerBackupPage::getUpdateBackupFinished(const QPair<grpc::Status, container::UpdateBackupReply> &)
{
}

void ContainerBackupPage::getCreateBackupFinished(const QPair<grpc::Status, container::CreateBackupReply> &reply)
{
    KLOG_INFO() << "getCreateBackupFinished";
    if (reply.first.ok())
    {
        InfoWorker::getInstance().listBackup(m_nodeId, m_containerId);
    }
}

void ContainerBackupPage::getResumeBackupFinished(const QPair<grpc::Status, container::ResumeBackupReply> &)
{
}

void ContainerBackupPage::getRemoveBackupFinished(const QPair<grpc::Status, container::RemoveBackupReply> &)
{
}

void ContainerBackupPage::initTable()
{
    QStringList tableHHeaderDate = {
        "",
        QString(tr("Backup file Name")),
        QString(tr("Backup status")),
        QString(tr("Backup start time")),
        QString(tr("Backup path")),
        QString(tr("Backup size")),
        QString(tr("Description")),
        QString(tr("Quick Actions"))};
    setHeaderSections(tableHHeaderDate);

    setTableActions(tableHHeaderDate.size() - 1, QMap<ACTION_BUTTON_TYPE, QString>{{ACTION_BUTTON_TYPE_BACKUP_RESUME, tr("Resume")},
                                                                                   {ACTION_BUTTON_TYPE_BACKUP_UPDATE, tr("Update")},
                                                                                   {ACTION_BUTTON_TYPE_BACKUP_REMOVE, tr("Remove")}});

    setTableDefaultContent("-");

    connect(this, &ContainerBackupPage::sigBackupResume, this, &ContainerBackupPage::onResumeBackup);
    connect(this, &ContainerBackupPage::sigBackupUpdate, this, &ContainerBackupPage::onUpdateBackup);
    connect(this, &ContainerBackupPage::sigBackupRemove, this, &ContainerBackupPage::onRemoveBackup);
    //    connect(this, &ContainerListPage::sigMonitor, this, &ContainerListPage::onMonitor);
    //    connect(this, &ContainerListPage::sigEdit, this, &ContainerListPage::onEdit);

    //    connect(this, &ContainerListPage::sigItemClicked, this, &ContainerListPage::onItemClicked);
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
