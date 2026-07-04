#ifndef SUBSCRIBETHREAD_H
#define SUBSCRIBETHREAD_H

#include <QObject>

class SubscribeThread : public QObject
{
    Q_OBJECT
public:
    SubscribeThread(QObject *parent = nullptr);
    ~SubscribeThread() = default;

    void subscribe();
    void cancel();

signals:
    void sessinoExpire();

private:
    bool m_isCanceled;
};

#endif  // SUBSCRIBETHREAD_H
