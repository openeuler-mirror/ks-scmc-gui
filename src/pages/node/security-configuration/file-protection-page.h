/**
 * @file          src/pages/node/security-configuration/file-protection-page.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#ifndef FILEPROTECTIONPAGE_H
#define FILEPROTECTIONPAGE_H

#include <QListWidget>
#include <QObject>
#include <QRadioButton>
#include "base/interface/node.h"
#include "page.h"

class SecurityListItem;
class SecurityList;
class FileProtectionPage : public Page
{
    Q_OBJECT
public:
    explicit FileProtectionPage(QWidget *parent = nullptr);
    void updateInfo(QString keyword = "") override;

    void setNodeId(qint64 nodeID);

private slots:
    void save();
    void setProtectEnabled(bool checked);
    void getUpdateFileProtectFinished(const QString objId, const QPair<grpc::Status, node::UpdateFileProtectReply> &);
    void getFileProtectFinished(const QString objId, const QPair<grpc::Status, node::GetFileProtectReply> &);

private:
    void initUI();

private:
    QString m_objId;
    qint64 m_nodeID;
    QRadioButton *m_btnOpen;
    QRadioButton *m_btnClose;
    SecurityList *m_fileList;
    bool m_protectEnabled;
};

#endif  // FILEPROTECTIONPAGE_H
