/**
 * @file          /ks-scmc-gui/src/base/task-manager.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2025 KylinSec. All rights reserved.
 */

#include "base/task-manager.h"

TaskManager::TaskManager(QObject* parent) : QObject(parent)
{
}

void TaskManager::registerTask(const QString& id,
                               QSharedPointer<grpc::ClientContext> context)
{
    QMutexLocker lock(&m_mutex);
    m_tasks[id] = context;
}

void TaskManager::unregisterTask(const QString& id)
{
    QMutexLocker lock(&m_mutex);
    m_tasks.remove(id);
}

bool TaskManager::hasRunningTasks()
{
    QMutexLocker lock(&m_mutex);
    return !m_tasks.isEmpty();
}

int TaskManager::runningTaskCount()
{
    QMutexLocker lock(&m_mutex);
    return m_tasks.size();
}

void TaskManager::cancelAll()
{
    QMutexLocker lock(&m_mutex);
    for (auto it = m_tasks.begin(); it != m_tasks.end(); ++it)
    {
        auto& context = it.value();
        if (context)
        {
            KLOG_DEBUG() << "Begin to cancel the" << it.key() << "task.";
            context->TryCancel();
        }
    }
}