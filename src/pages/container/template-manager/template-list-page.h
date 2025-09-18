/**
 * @file          /ks-scmc-gui/src/pages/container/template-manager/template-list-page.h
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef TEMPLATELISTPAGE_H
#define TEMPLATELISTPAGE_H
#include <kiran-titlebar-window.h>
#include <table-page.h>
#include <QDialog>
#include "base/interface/container.h"
#include "base/interface/image.h"
#include "base/interface/network.h"
#include "base/interface/node.h"
#include "container/container-setting.h"

class TemplateImportDialog : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit TemplateImportDialog(QWidget *parent = nullptr);
    QString getTemplateName() const;
    QString getTemplateFile() const;
    void clear();

private slots:
    void onSelectFile();

signals:
    void fileSelected();

private:
    QLineEdit *m_templateNameEdit;
    QLineEdit *m_templateFileEdit;
    QPushButton *m_selectFileBtn;
};

class ContainerSetting;
class TemplateListPage : public TablePage
{
    Q_OBJECT
public:
    explicit TemplateListPage(QWidget *parent = nullptr);
    ~TemplateListPage();
    void updateInfo(QString keyword = "");

private slots:
    void onEdit(int row);
    void onDelete(int row);
    void onCreateTemplate();
    void onRemoveTemplate();
    void onImportTemplate();
    void onExportTemplate();

    void getNetworkListResult(const QString objId, const QPair<grpc::Status, network::ListReply> &reply);
    void getNodeListResult(QString objId, const QPair<grpc::Status, node::ListReply> &);
    void getListImageFinishedResult(QString objId, const QPair<grpc::Status, image::ListReply> &);

    void getCreateTemplateFinishResult(const QString objId, const QPair<grpc::Status, container::CreateTemplateReply> &reply);
    void getListTemplateFinishResult(const QString objId, const QPair<grpc::Status, container::ListTemplateReply> &reply);
    void getRemoveTemplateFinishResult(const QString objId, const QPair<grpc::Status, container::RemoveTemplateReply> &reply);

private:
    void initTable();
    void initButtons();
    void initTemplateConnect();
    void getCheckedItemsId(QList<int64_t> &ids);
    void getItemId(int row, int64_t &id);
    void exportTemplate(const QList<int64_t> &ids, const QString &exportPath);

    QString convertTemplateToJson(const container::ContainerTemplate &data);
    void parseJsonToTemplate(const QByteArray &jsonData, const QString &templateName);

    void getNetworkInfo(int64_t node_id);
    void getTemplateInfo();
    void getNodeInfo();
    void getImageInfo();

private:
    QString m_objId;
    ContainerSetting *m_createTPSetting;
    ContainerSetting *m_editTPSetting;
    QMultiMap<int, QString> m_networksMap;
    QMap<int, NodeInfo *> m_nodeInfoMap;
    QStringList m_imageInfos;
    QMap<int64_t, QString> m_templateJsonMap;  // 存储模板ID和对应的JSON数据
    TemplateImportDialog *m_importDialog;
};

#endif  // TEMPLATELISTPAGE_H
