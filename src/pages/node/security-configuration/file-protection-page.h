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
#include "base/interface/node.h"
#include "page.h"

class FileProtectionPage : public Page
{
    Q_OBJECT
public:
    explicit FileProtectionPage(QWidget *parent = nullptr);
    void updateInfo(QString keyword = "") override;

private slots:
    void save();
    void setProtectEnabled(bool checked);
    void getUpdateFileProtectFinished(const QString objId, const QPair<grpc::Status, node::UpdateFileProtectReply> &);

private:
    void initUI();

private:
    QListWidget *m_fileList;
    bool m_protectEnabled;
};

#endif  // FILEPROTECTIONPAGE_H
