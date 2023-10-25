/**
 * @file          /ks-scmc-gui/src/common/config-group.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "config-group.h"

ModelItem::ModelItem(QObject *parent) : QObject(parent)
{
    Q_UNUSED(parent);
    m_thirdColVal = 1;
}
