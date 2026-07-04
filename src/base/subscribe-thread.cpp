#include "subscribe-thread.h"
#include <kiran-log/qt5-log-i.h>
#include "info-worker.h"
#include "proto/system.grpc.pb.h"
#include "rpc.h"
#include "user-configuration.h"

SubscribeThread::SubscribeThread(QObject* parent) : m_isCanceled(false)
{
}

void SubscribeThread::subscribe()
{
    sys::SubscribeRequest req;
    auto chan = get_rpc_channel(UserConfiguration::getServerAddr());
    if (!chan)
    {
        KLOG_INFO() << "subscribe failed to get connection";
    }

    grpc::ClientContext context;

    auto authkey = InfoWorker::getInstance().getAuthKey();
    KLOG_INFO() << "s_authKey" << authkey.data();
    if (authkey.size() > 0)
        context.AddMetadata("authorization", authkey);

    grpc::CompletionQueue cq;
    sys::SubscribeReply reply;
    grpc::Status status;
    std::unique_ptr<::grpc::ClientAsyncReader<::sys::SubscribeReply>> stream(sys::System::NewStub(chan)->AsyncSubscribe(&context, req, &cq, (void*)1));
    stream->Finish(&status, (void*)1);

    void* got_tag;
    bool ok = false;
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(1000);

    while (!m_isCanceled)
    {
        bool ret = cq.AsyncNext(&got_tag, &ok, deadline);
        if (ret && got_tag == (void*)1)
        {
            if (ok)
            {
                stream->Read(&reply, (void*)1);
                if (reply.msg_type() == sys::UserSessionExpire)
                {
                    KLOG_INFO() << "UserSessionExpire";
                    emit sessinoExpire();
                    break;
                }
            }

            ok = false;
            continue;
        }
    }
    if (m_isCanceled)
        context.TryCancel();
}

void SubscribeThread::cancel()
{
    m_isCanceled = true;
}
