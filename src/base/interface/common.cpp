/**
 * @file          src/base/interface/common.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "common.h"

#include <QMutexLocker>

Common::Common(QObject *parent) : QObject(parent)
{
    QMutexLocker locker(&mutex);
    qRegisterMetaType<ImageTransmissionStatus>("ImageTransmissionStatus");
}

Common::~Common()
{
}

std::string Common::getAuthKey()
{
    return s_authKey;
}

void Common::setAuthKey(std::string str)
{
    s_authKey = str;
}

void Common::stopTransfer(const QString &name, const QString &version, bool isStop)
{
    QMutexLocker locker(&mutex);
    m_transferStatusMap.insert(name + "-" + version, isStop);
}

bool Common::isTransferStoped(const QString &name, const QString &version)
{
    if (m_transferStatusMap.contains(name + "-" + version))
        return m_transferStatusMap.value(name + "-" + version);
    else
        return false;
}
