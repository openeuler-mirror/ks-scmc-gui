/**
 * @file          src/global-application.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */

#include "global-application.h"
#include <kiran-log/qt5-log-i.h>

#define TIMEOUT 1800000  // 30min

GlobalApplication::GlobalApplication(int &argc, char **argv) : KiranSingleApplication(argc, argv),
                                                               m_timer(nullptr),
                                                               m_pthread(nullptr)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(TIMEOUT);
    m_pthread = new QThread();
    m_pthread->moveToThread(m_pthread);

    connect(m_pthread, SIGNAL(started()), m_timer, SLOT(start()));
    connect(m_timer, &QTimer::timeout, this, &GlobalApplication::expired);
    m_pthread->start();
}

GlobalApplication::~GlobalApplication()
{
    if (m_pthread)
    {
        m_pthread->quit();
        m_pthread->wait();
    }
    if (m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }
}

bool GlobalApplication::notify(QObject *receiver, QEvent *e)
{
    if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::KeyPress)
    {
        m_pthread->quit();
    }
    else if (e->type() == QEvent::MouseButtonRelease || e->type() == QEvent::KeyRelease)
    {
        m_pthread->start();
    }
    return KiranSingleApplication::notify(receiver, e);
}

void GlobalApplication::expired()
{
    emit sessionExpired();
}
