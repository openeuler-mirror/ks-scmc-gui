#ifndef CONTAINERSETTING_H
#define CONTAINERSETTING_H
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QWidget>
#include "def.h"
#include "info-worker.h"
namespace Ui
{
class ContainerSetting;
}
enum ContainerSettingType
{
    CONTAINER_SETTING_TYPE_CONTAINER_CREATE,
    CONTAINER_SETTING_TYPE_CONTAINER_EDIT,
    CONTAINER_SETTING_TYPE_TEMPLATE_CREATE,
    CONTAINER_SETTING_TYPE_TEMPLATE_EDIT,
    CONTAINER_SETTING_TYPE_CONTAINER_CREATE_FROM_TEMPLATE
};

enum TabConfigGuideItemType
{
    TAB_CONFIG_GUIDE_ITEM_TYPE_CPU = 0,
    TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY,
    TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_CARD,
    TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_ENVS = 0,
    TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_GRAPHIC,
    TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_VOLUMES,
    TAB_CONFIG_GUIDE_ITEM_TYPE_HIGH_AVAILABILITY,
    TAB_CONFIG_GUIDE_ITEM_TYPE_FILE_PROTECT = 0,
    TAB_CONFIG_GUIDE_ITEM_TYPE_PROCESS_SECURITY,
    TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_ACCESS_CONTROL,
    TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_PROCESS_WHITE_LIST,
    TAB_CONFIG_GUIDE_ITEM_TYPE_START_STOP_CONTROL
};

enum MultiConfigType
{
    MULTI_CONFIG_TYPE_NETWOR
};

class GuideItem;
class NetworkConfTab;
class ContainerSetting : public QWidget
{
    Q_OBJECT

public:
    explicit ContainerSetting(ContainerSettingType type, QMultiMap<int, QString> networksMap, QMap<QString, QVariant> ids = QMap<QString, QVariant>(), QWidget *parent = nullptr);
    ~ContainerSetting();
    void paintEvent(QPaintEvent *event);
    void setItems(int row, int col, QWidget *);
    void setTitle(QString title);
    void setTemplateList(QMultiMap<int, QPair<int, QString>> templateMap);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

private:
    void initUI();
    void initSummaryUI();
    GuideItem *createGuideItem(QListWidget *parent, QString text, int type = GUIDE_ITEM_TYPE_NORMAL, QString icon = "");
    void initBaseConfPages();
    void initAdvancedConfPages();
    void initSecurityConfPages();
    void deleteItem(QString itemText, int row);
    void updateRemovableItem(QString itemText);
    void getContainerInspect();
    void getTemplateInspect(int templateId);
    void getNodeInfo();
    void getImageInfo(int64_t node_id);
    void setNodeNetworkList(int nodeId);

    void createContainer();
    void updateContainer();

    void createTemplate();
    void updateTemplate();

signals:
    void sigUpdateContainer();
    void sigUpdateTemplate();

private slots:
    void onItemClicked(QListWidgetItem *item);
    void onAddItem(QAction *action);
    void onDelItem();
    void onConfirm();
    void onNodeSelectedChanged(QString newStr);
    void onTempSelectedChanged(QString newStr);

    void getNodeListResult(QString objId, const QPair<grpc::Status, node::ListReply> &);
    void getListImageFinishedResult(QString objId, const QPair<grpc::Status, image::ListReply> &);

    void getCreateContainerResult(QString objId, const QPair<grpc::Status, container::CreateReply> &);
    void getContainerInspectResult(QString objId, const QPair<grpc::Status, container::InspectReply> &);
    void getUpdateContainerResult(QString objId, const QPair<grpc::Status, container::UpdateReply> &);

    void getInspectTemplateFinishResult(QString objId, const QPair<grpc::Status, container::InspectTemplateReply> &reply);
    void getCreateTemplateFinishResult(QString objId, const QPair<grpc::Status, container::CreateTemplateReply> &reply);
    void getUpdateTemplateFinishedResult(QString objId, const QPair<grpc::Status, container::UpdateTemplateReply> &reply);

private:
    Ui::ContainerSetting *ui;
    QString m_objId;
    QStackedWidget *m_baseConfStack;
    QStackedWidget *m_advancedConfStack;
    QStackedWidget *m_securityConfStack;
    QList<GuideItem *> m_baseItems;
    QList<GuideItem *> m_advancedItems;
    QList<GuideItem *> m_securityItems;
    QMenu *m_addMenu;
    QComboBox *m_cbImage;
    QLabel *m_labImage;
    QMap<int64_t, QString> m_nodeInfo;       //id,address
    QPair<int64_t, QString> m_containerIds;  //nodeId,containerId
    int m_templateId;
    int m_netWorkCount;
    ContainerSettingType m_type;
    QMap<int, double> m_nodeTotalCPU;  //node total cpu
    QList<NetworkConfTab *> m_netWorkPages;
    QMultiMap<int, QString> m_networksMap;
    QMultiMap<int, QPair<int, QString>> m_templateMap;
};

#endif  // CONTAINERSETTING_H
