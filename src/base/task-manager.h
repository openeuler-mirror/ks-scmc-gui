/**
 * @file          /ks-scmc-gui/src/base/task-manager.h
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2025 KylinSec. All rights reserved.
 */

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <grpcpp/client_context.h>
#include <kiran-log/qt5-log-i.h>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPair>
#include <QSharedPointer>

class TaskManager : public QObject
{
public:
    static TaskManager& instance()
    {
        static TaskManager instance;
        return instance;
    }

    void registerTask(const QString& id,
                      QSharedPointer<grpc::ClientContext> context = nullptr);

    void unregisterTask(const QString& id);
    bool hasRunningTasks();
    int runningTaskCount();
    void cancelAll();

private:
    explicit TaskManager(QObject* parent = nullptr);
    Q_DISABLE_COPY(TaskManager)

    QMap<QString, QSharedPointer<grpc::ClientContext>> m_tasks;
    mutable QMutex m_mutex;
};

#endif  // TASKMANAGER_H