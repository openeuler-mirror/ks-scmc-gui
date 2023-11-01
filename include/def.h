/**
 * @file          /ks-scmc-gui/include/def.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef DEF_H
#define DEF_H
#include <QString>

#define CONTAINER_ID "container id"
#define NODE_ID "node id"
#define NODE_ADDRESS "node address"
#define NODE_NAME "node name"
#define NODE_COMMENT "node comment"
#define CONTAINER_NAME "container name"
#define CONTAINER_STATUS "container status"
#define CONTAIENR_APP_IS_RUNNING "container app is_running"
#define TEMPLATE_ID "template id"
#define TEMPLATE_NAME "template name"
#define BACKUP_ID "backup id"
#define BACKUP_NAME "backup name"
#define SCMC_GUI_VERSION_FILE_PATH "/usr/share/ks-scmc-gui/ks-scmc-gui.version"
#define SCMC_VERSION_FILE_PATH "/usr/share/ks-scmc/ks-scmc.version"

enum GUIDE_ITEM_TYPE
{
    GUIDE_ITEM_TYPE_NORMAL,
    GUIDE_ITEM_TYPE_GROUP,
    GUIDE_ITEM_TYPE_SUB
};

enum GUIDE_ITEM
{
    GUIDE_ITEM_HONE = 0,
    GUIDE_ITEM_WARNING_LIST,
    GUIDE_ITEM_LOG_LIST,
    GUIDE_ITEM_CONTAINER_List_PAGE_MANAGER,
    GUIDE_ITEM_CONTAINER_TEMPLATE_LIST,
    GUIDE_ITEM_IMAGE_LIST,
    GUIDE_ITEM_IMAGE_APPROVAL_LIST,
    GUIDE_ITEM_IMAGE_APPROVABLE_CONTROLLER,
    GUIDE_ITEM_NODE_MANAGER,
    GUIDE_ITEM_OUTLINE
};

enum ACTION_BUTTON_TYPE
{
    ACTION_BUTTON_TYPE_APP,
    ACTION_BUTTON_TYPE_APP_STOP,
    ACTION_BUTTON_TYPE_APP_RUN,
    ACTION_BUTTON_TYPE_MONITOR,
    ACTION_BUTTON_TYPE_EDIT,
    ACTION_BUTTON_TYPE_TERINAL,
    ACTION_BUTTON_TYPE_DELETE,
    ACTION_BUTTON_TYPE_IMAGE_PASS,
    ACTION_BUTTON_TYPE_IMAGE_REFUSE,
    ACTION_BUTTON_TYPE_BACKUP_RESUME,
    ACTION_BUTTON_TYPE_BACKUP_UPDATE,
    ACTION_BUTTON_TYPE_BACKUP_EXPORT,
    ACTION_BUTTON_TYPE_BACKUP_REMOVE,
    ACTION_BUTTON_TYPE_WARN_READ,
    ACTION_BUTTON_TYPE_WARN_IGNORE,
    ACTION_BUTTON_TYPE_MENU
};

enum ErrorCode
{
    NO_ERROR = 0,
    INPUT_NULL_ERROR,
    CONFIG_ARG_ERROR,
    INPUT_ARG_ERROR
};

enum OPERATION_BUTTOM
{
    OPERATION_BUTTOM_CONTAINER_LIST_RUN,
    OPERATION_BUTTOM_CONTAINER_LIST_STOP,
    OPERATION_BUTTOM_CONTAINER_LIST_RESTART,
    OPERATION_BUTTOM_CONTAINER_LIST_DELETE,
    OPERATION_BUTTOM_CONTAINER_LIST_MORE,
    OPERATION_BUTTOM_IMAGE_MANAGER_UPLOAD,
    OPERATION_BUTTOM_IMAGE_MANAGER_DOWNLOAD,
    OPERATION_BUTTOM_IMAGE_MANAGER_UPDATE,
    OPERATION_BUTTOM_IMAGE_MANAGER_REMOVE,
    OPERATION_BUTTOM_IMAGE_MANAGER_CHECK,
    OPERATION_BUTTOM_IMAGE_MANAGER_PASS,
    OPERATION_BUTTOM_IMAGE_MANAGER_REFUSE,
    OPERATION_BUTTOM_WARN_READ,
    OPERATION_BUTTOM_WARN_IGNORE
};

enum ImageTransmissionStatus
{
    IMAGE_TRANSMISSION_STATUS_UPLOADING = 0,
    IMAGE_TRANSMISSION_STATUS_UPLOADING_SUCCESSFUL,
    IMAGE_TRANSMISSION_STATUS_UPLOADING_FAILED,
    IMAGE_TRANSMISSION_STATUS_DOWNLOADING,
    IMAGE_TRANSMISSION_STATUS_DOWNLOADING_SUCCESSFUL,
    IMAGE_TRANSMISSION_STATUS_DOWNLOADING_FAILED
};

enum TabPageType
{
    TAB_PAGE_TYPE_AUDIT_CENTER_APPLY_PENDING,
    TAB_PAGE_TYPE_AUDIT_CENTER_APPLY_PASS,
    TAB_PAGE_TYPE_AUDIT_CENTER_APPLY_REJECT
};
#endif  // DEF_H
