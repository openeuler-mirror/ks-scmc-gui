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
    CONTAINER_SETTING_TYPE_TEMPLATE_EDIT
};

enum TabConfigGuideItemType
{
    TAB_CONFIG_GUIDE_ITEM_TYPE_CPU = 0,
    TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY,
    TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_CARD,
    TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_ENVS = 0,
    TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_GRAPHIC,
    TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_VOLUMES,
    TAB_CONFIG_GUIDE_ITEM_TYPE_HIGH_AVAILABILITY
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
    explicit ContainerSetting(ContainerSettingType type, QPair<int64_t, QString> ids = QPair<int64_t, QString>(), QWidget *parent = nullptr);
    ~ContainerSetting();
    void paintEvent(QPaintEvent *event);
    void setItems(int row, int col, QWidget *);
    void setTitle(QString title);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

private:
    void initUI();
    void initSummaryUI();
    GuideItem *createGuideItem(QListWidget *parent, QString text, int type = GUIDE_ITEM_TYPE_NORMAL, QString icon = "");
    void initBaseConfPages();
    void initAdvancedConfPages();
    void updateRemovableItem(QString itemText);
    void getContainerInspect();
    void getNodeInfo();
    void getImageInfo(int64_t node_id);

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
    void getNodeListResult(const QPair<grpc::Status, node::ListReply> &);
    void getListImageFinishedResult(const QPair<grpc::Status, image::ListReply> &);
    void getNetworkListResult(const QPair<grpc::Status, network::ListReply> &reply);

    void getCreateContainerResult(const QPair<grpc::Status, container::CreateReply> &);
    void getContainerInspectResult(const QPair<grpc::Status, container::InspectReply> &);
    void getUpdateContainerResult(const QPair<grpc::Status, container::UpdateReply> &);

    void getCreateTemplateFinishResult(const QPair<grpc::Status, container::CreateTemplateReply> &reply);
    void getUpdateTemplateFinishedResult(const QPair<grpc::Status, container::UpdateTemplateReply> &reply);

private:
    Ui::ContainerSetting *ui;
    QStackedWidget *m_baseConfStack;
    QStackedWidget *m_advancedConfStack;
    QList<GuideItem *> m_baseItems;
    QList<GuideItem *> m_advancedItems;
    QMenu *m_addMenu;
    QComboBox *m_cbImage;
    QLabel *m_labImage;
    QMap<int64_t, QString> m_nodeInfo;       //id,address
    QPair<int64_t, QString> m_containerIds;  //nodeId,containerId
    int m_netWorkCount;
    ContainerSettingType m_type;
    double m_totalCPU;
    QList<NetworkConfTab *> m_netWorkPages;
    QPair<grpc::Status, network::ListReply> m_networkReply;
};

#endif  // CONTAINERSETTING_H
