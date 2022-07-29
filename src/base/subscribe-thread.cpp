#include "subscribe-thread.h"
#include <kiran-log/qt5-log-i.h>
#include "info-worker.h"
#include "proto/system.grpc.pb.h"
#include "rpc.h"
#include "user-configuration.h"
//extern std::string s_authKey;

SubscribeThread::SubscribeThread(QObject* parent) : m_isCanceled(false)
{
}

SubscribeThread::~SubscribeThread()
{
    KLOG_INFO() << "~SubscribThread";
}

void SubscribeThread::subscribe()
{
    sys::SubscribeRequest req;
    KLOG_INFO() << "_subscribe";
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
    //auto stream = sys::System::NewStub(chan)->AsyncSubscribe(&context, req, &cq, (void*)1);
    std::unique_ptr<::grpc::ClientAsyncReader<::sys::SubscribeReply>> stream(sys::System::NewStub(chan)->AsyncSubscribe(&context, req, &cq, (void*)1));
    //stream->StartCall((void*)1);
    //
    //    stream->Finish(&status, (void*)1);

    void* got_tag;
    bool ok = false;
    bool ret = cq.Next(&got_tag, &ok);
    if (ret && ok && got_tag == (void*)1)
    {
        // check reply and status
        while (1)
        {
            KLOG_INFO() << "read ............";
            if (m_isCanceled)
            {
                KLOG_INFO() << "cancel";
                context.TryCancel();
                break;
            }
            stream->Read(&reply, (void*)1);
            KLOG_INFO() << "reply.msg_type :" << reply.msg_type();
            if (reply.msg_type() == sys::UserSessionExpire)
            {
                KLOG_INFO() << "UserSessionExpire";
                emit sessinoExpire();
                break;
            }

            ok = false;
            KLOG_INFO() << "get Next ............";
            ret = cq.Next(&got_tag, &ok);
            KLOG_INFO() << "Next ............";
            if (!ret || !ok || got_tag != (void*)1)
                break;
        }
    }
}

void SubscribeThread::cancel()
{
    m_isCanceled = true;
}
