#ifndef DEF_H
#define DEF_H
#include <QString>
enum GUIDE_ITEM_TYPE
{
    GUIDE_ITEM_TYPE_NORMAL,
    GUIDE_ITEM_TYPE_GROUP,
    GUIDE_ITEM_TYPE_SUB
};

enum GUIDE_ITEM
{
    GUIDE_ITEM_HONE = 0,
    GUIDE_ITEM_AUDIT_APPLY_LIST = 2,
    GUIDE_ITEM_AUDIT_WARNING_LIST = 3,
    GUIDE_ITEM_AUDIT_LOG_LIST,
    GUIDE_ITEM_CONTAINER_LIST = 6,
    GUIDE_ITEM_CONTAINER_TEMPLATE,
    GUIDE_ITEM_IMAGE_MANAGER,
    GUIDE_ITEM_NODE_MANAGER
};

enum ACTION_BUTTON_TYPE
{
    ACTION_BUTTON_TYPE_MONITOR,
    ACTION_BUTTON_TYPE_EDIT,
    ACTION_BUTTON_TYPE_TERINAL,
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
    OPERATION_BUTTOM_IMAGE_MANAGER_CHECK
};

#endif  // DEF_H
