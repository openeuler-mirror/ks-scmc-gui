/**
 * @file          src/common/notification-manager.h
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */

#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
class NotificationManager : public QObject
{
    Q_OBJECT
public:
    NotificationManager(QObject *parent = nullptr);

    static void sendNotify(const QString &summary, const QString &body);
};

#endif  // NOTIFICATIONMANAGER_H
