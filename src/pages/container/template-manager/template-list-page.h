#ifndef TEMPLATELISTPAGE_H
#define TEMPLATELISTPAGE_H
#include <table-page.h>
#include "info-worker.h"

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

    void getNetworkListResult(const QPair<grpc::Status, network::ListReply> &reply);
    void getListTemplateFinishResult(const QPair<grpc::Status, container::ListTemplateReply> &reply);
    void getRemoveTemplateFinishResult(const QPair<grpc::Status, container::RemoveTemplateReply> &reply);

private:
    void initTable();
    void initButtons();
    void initTemplateConnect();
    void getCheckedItemsId(QList<int64_t> &ids);
    void getItemId(int row, int64_t &id);
    void getNetworkInfo(int64_t node_id);
    void getTemplateInfo();

private:
    ContainerSetting *m_createTPSetting;
    ContainerSetting *m_editTPSetting;
    QMultiMap<int, QString> m_networksMap;
};

#endif  // TEMPLATELISTPAGE_H
