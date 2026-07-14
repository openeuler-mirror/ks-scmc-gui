/**
 * @file          src/global-application.h
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */

#ifndef GLOBALAPPLICATION_H
#define GLOBALAPPLICATION_H
#include <kiran-single-application.h>
#include <QThread>
#include <QTimer>

class GlobalApplication : public KiranSingleApplication
{
    Q_OBJECT
public:
    GlobalApplication(int &argc, char **argv);
    ~GlobalApplication();
    bool notify(QObject *receiver, QEvent *e);

private:
    void expired();

signals:
    void sessionExpired();

private:
    QTimer *m_timer;
    QThread *m_pthread;
};

#endif  // GLOBALAPPLICATION_H
