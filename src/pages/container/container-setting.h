/**
 * @file          /ks-scmc-gui/src/pages/container/container-setting.h
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef CONTAINERSETTING_H
#define CONTAINERSETTING_H
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QWidget>
#include "base/interface/container.h"
#include "def.h"
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
    CONTAINER_SETTING_TYPE_CONTAINER_CREATE_FROM_TEMPLATE,
    CONTAINER_SETTING_TYPE_CONTAINER_GENERATE_TEMPLATE
};

enum TabConfigGuideItemType
{
    TAB_CONFIG_GUIDE_ITEM_TYPE_CPU = 0,
    TAB_CONFIG_GUIDE_ITEM_TYPE_MEMORY,
    TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_CARD,
    TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_CMD = 0,
    TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_ENVS,
    TAB_CONFIG_GUIDE_ITEM_TYPE_ITEM_VOLUMES,
    TAB_CONFIG_GUIDE_ITEM_TYPE_HIGH_AVAILABILITY,
    TAB_CONFIG_GUIDE_ITEM_TYPE_FILE_PROTECT = 0,
    TAB_CONFIG_GUIDE_ITEM_TYPE_PROCESS_SECURITY,
    TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_ACCESS_CONTROL,
    TAB_CONFIG_GUIDE_ITEM_TYPE_NETWORK_PROCESS_WHITE_LIST,
    TAB_CONFIG_GUIDE_ITEM_TYPE_START_STOP_CONTROL
};

struct NodeInfo
{
    NodeInfo()
    {
        nodeID = 0;
        totalCPU = 0;
        totalMemory = 0;
    }
    int nodeID;
    QString nodeAddr;
    double totalCPU;
    double totalMemory;
};

class GuideItem;
class NetworkConfTab;
class ContainerSetting : public QWidget
{
    Q_OBJECT

public:
    explicit ContainerSetting(ContainerSettingType type, QWidget *parent = nullptr);
    ~ContainerSetting();
    void paintEvent(QPaintEvent *event);
    void setItems(int row, int col, QWidget *);
    void setTitle(QString title);

    void setTemplateList(QMultiMap<int, QPair<int, QString>> templateMap);
    void setNodeInfos(QMap<int, NodeInfo *> nodeInfoMap);
    void setImageList(QStringList imageList);
    void setNetworkInfos(QMultiMap<int, QString> networksMap);

    void getContainerInspect(int nodeID, const QString containerID);
    void getTemplateInspect();
    void getTemplateInspect(int templateID);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

private:
    void initUI();
    void initSummaryUI();
    void initConnection();
    GuideItem *createGuideItem(QListWidget *parent, QString text, int type = GUIDE_ITEM_TYPE_NORMAL, QString icon = "");
    void initBaseConfPages();
    void initAdvancedConfPages();
    void initSecurityConfPages();
    void showLongText(QLineEdit *lineEdit, QString orgText);
    QString tooptipWordWrap(const QString &org);
    void deleteItem(QString itemText, int row);
    void updateRemovableItem(QString itemText);
    void setNodeNetworkList(int nodeId);

    bool writeContainerConfig(container::ContainerConfigs *cntrCfg);
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
    void onNodeSelectedChanged(const QString &newStr);
    void onTempSelectedChanged(QString newStr);

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

    int m_templateId;
    int m_nodeID;
    QString m_containerID;
    int m_netWorkCount;

    ContainerSettingType m_type;
    QMap<int, double> m_nodeTotalCPU;     //node total cpu
    QMap<int, double> m_nodeTotalMemory;  //node total memory
    QList<NetworkConfTab *> m_netWorkPages;
    QMultiMap<int, QString> m_networksMap;
    QMultiMap<int, QPair<int, QString>> m_templateMap;
    QMap<int, NodeInfo *> m_nodeInfoMap;
    QStringList m_imageList;
};

#endif  // CONTAINERSETTING_H
