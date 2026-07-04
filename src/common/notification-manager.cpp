// clang-format off
#include <libnotify/notify.h>
#include "notification-manager.h"
#include <kiran-log/qt5-log-i.h>

// clang-format on
NotificationManager::NotificationManager(QObject *parent) : QObject(parent)
{
}

void NotificationManager::sendNotify(const QString &summary, const QString &body)
{
    if (!notify_init("NotificationManager"))
    {
        KLOG_DEBUG() << "init notify failed!";
        return;
    }

    QString icon = ":/images/logo.png";
    GError *error = nullptr;

    NotifyNotification *notify = notify_notification_new(summary.toStdString().c_str(),
                                                         body.toStdString().c_str(),
                                                         icon.toStdString().c_str());
    if (!notify_notification_show(notify, &error))
    {
        KLOG_DEBUG() << error->message;
        g_error_free(error);
    }

    g_object_unref(G_OBJECT(notify));
    notify_uninit();
}
