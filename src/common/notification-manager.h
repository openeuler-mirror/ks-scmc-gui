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
