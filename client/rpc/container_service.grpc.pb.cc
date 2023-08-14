// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: container_service.proto

#include "container_service.pb.h"
#include "container_service.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace container {

static const char* Container_method_names[] = {
  "/container.Container/List",
  "/container.Container/Create",
  "/container.Container/Inspect",
  "/container.Container/Start",
  "/container.Container/Stop",
  "/container.Container/Remove",
  "/container.Container/Restart",
  "/container.Container/Update",
  "/container.Container/Kill",
  "/container.Container/Status",
  "/container.Container/MonitorHistory",
};

std::unique_ptr< Container::Stub> Container::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Container::Stub> stub(new Container::Stub(channel));
  return stub;
}

Container::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_List_(Container_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Create_(Container_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Inspect_(Container_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Start_(Container_method_names[3], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Stop_(Container_method_names[4], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Remove_(Container_method_names[5], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Restart_(Container_method_names[6], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Update_(Container_method_names[7], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Kill_(Container_method_names[8], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Status_(Container_method_names[9], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_MonitorHistory_(Container_method_names[10], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Container::Stub::List(::grpc::ClientContext* context, const ::container::ListRequest& request, ::container::ListReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_List_, context, request, response);
}

void Container::Stub::experimental_async::List(::grpc::ClientContext* context, const ::container::ListRequest* request, ::container::ListReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_List_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::List(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::ListReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_List_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::List(::grpc::ClientContext* context, const ::container::ListRequest* request, ::container::ListReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_List_, context, request, response, reactor);
}

void Container::Stub::experimental_async::List(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::ListReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_List_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::container::ListReply>* Container::Stub::AsyncListRaw(::grpc::ClientContext* context, const ::container::ListRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::ListReply>::Create(channel_.get(), cq, rpcmethod_List_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::container::ListReply>* Container::Stub::PrepareAsyncListRaw(::grpc::ClientContext* context, const ::container::ListRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::ListReply>::Create(channel_.get(), cq, rpcmethod_List_, context, request, false);
}

::grpc::Status Container::Stub::Create(::grpc::ClientContext* context, const ::container::CreateRequest& request, ::container::CreateReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Create_, context, request, response);
}

void Container::Stub::experimental_async::Create(::grpc::ClientContext* context, const ::container::CreateRequest* request, ::container::CreateReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Create_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Create(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::CreateReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Create_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Create(::grpc::ClientContext* context, const ::container::CreateRequest* request, ::container::CreateReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Create_, context, request, response, reactor);
}

void Container::Stub::experimental_async::Create(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::CreateReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Create_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::container::CreateReply>* Container::Stub::AsyncCreateRaw(::grpc::ClientContext* context, const ::container::CreateRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::CreateReply>::Create(channel_.get(), cq, rpcmethod_Create_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::container::CreateReply>* Container::Stub::PrepareAsyncCreateRaw(::grpc::ClientContext* context, const ::container::CreateRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::CreateReply>::Create(channel_.get(), cq, rpcmethod_Create_, context, request, false);
}

::grpc::Status Container::Stub::Inspect(::grpc::ClientContext* context, const ::container::InspectRequest& request, ::container::InspectReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Inspect_, context, request, response);
}

void Container::Stub::experimental_async::Inspect(::grpc::ClientContext* context, const ::container::InspectRequest* request, ::container::InspectReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Inspect_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Inspect(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::InspectReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Inspect_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Inspect(::grpc::ClientContext* context, const ::container::InspectRequest* request, ::container::InspectReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Inspect_, context, request, response, reactor);
}

void Container::Stub::experimental_async::Inspect(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::InspectReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Inspect_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::container::InspectReply>* Container::Stub::AsyncInspectRaw(::grpc::ClientContext* context, const ::container::InspectRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::InspectReply>::Create(channel_.get(), cq, rpcmethod_Inspect_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::container::InspectReply>* Container::Stub::PrepareAsyncInspectRaw(::grpc::ClientContext* context, const ::container::InspectRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::InspectReply>::Create(channel_.get(), cq, rpcmethod_Inspect_, context, request, false);
}

::grpc::Status Container::Stub::Start(::grpc::ClientContext* context, const ::container::StartRequest& request, ::container::StartReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Start_, context, request, response);
}

void Container::Stub::experimental_async::Start(::grpc::ClientContext* context, const ::container::StartRequest* request, ::container::StartReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Start_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Start(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::StartReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Start_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Start(::grpc::ClientContext* context, const ::container::StartRequest* request, ::container::StartReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Start_, context, request, response, reactor);
}

void Container::Stub::experimental_async::Start(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::StartReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Start_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::container::StartReply>* Container::Stub::AsyncStartRaw(::grpc::ClientContext* context, const ::container::StartRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::StartReply>::Create(channel_.get(), cq, rpcmethod_Start_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::container::StartReply>* Container::Stub::PrepareAsyncStartRaw(::grpc::ClientContext* context, const ::container::StartRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::StartReply>::Create(channel_.get(), cq, rpcmethod_Start_, context, request, false);
}

::grpc::Status Container::Stub::Stop(::grpc::ClientContext* context, const ::container::StopRequest& request, ::container::StopReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Stop_, context, request, response);
}

void Container::Stub::experimental_async::Stop(::grpc::ClientContext* context, const ::container::StopRequest* request, ::container::StopReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Stop_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Stop(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::StopReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Stop_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Stop(::grpc::ClientContext* context, const ::container::StopRequest* request, ::container::StopReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Stop_, context, request, response, reactor);
}

void Container::Stub::experimental_async::Stop(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::StopReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Stop_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::container::StopReply>* Container::Stub::AsyncStopRaw(::grpc::ClientContext* context, const ::container::StopRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::StopReply>::Create(channel_.get(), cq, rpcmethod_Stop_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::container::StopReply>* Container::Stub::PrepareAsyncStopRaw(::grpc::ClientContext* context, const ::container::StopRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::StopReply>::Create(channel_.get(), cq, rpcmethod_Stop_, context, request, false);
}

::grpc::Status Container::Stub::Remove(::grpc::ClientContext* context, const ::container::RemoveRequest& request, ::container::RemoveReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Remove_, context, request, response);
}

void Container::Stub::experimental_async::Remove(::grpc::ClientContext* context, const ::container::RemoveRequest* request, ::container::RemoveReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Remove_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Remove(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::RemoveReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Remove_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Remove(::grpc::ClientContext* context, const ::container::RemoveRequest* request, ::container::RemoveReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Remove_, context, request, response, reactor);
}

void Container::Stub::experimental_async::Remove(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::RemoveReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Remove_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::container::RemoveReply>* Container::Stub::AsyncRemoveRaw(::grpc::ClientContext* context, const ::container::RemoveRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::RemoveReply>::Create(channel_.get(), cq, rpcmethod_Remove_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::container::RemoveReply>* Container::Stub::PrepareAsyncRemoveRaw(::grpc::ClientContext* context, const ::container::RemoveRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::RemoveReply>::Create(channel_.get(), cq, rpcmethod_Remove_, context, request, false);
}

::grpc::Status Container::Stub::Restart(::grpc::ClientContext* context, const ::container::RestartRequest& request, ::container::RestartReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Restart_, context, request, response);
}

void Container::Stub::experimental_async::Restart(::grpc::ClientContext* context, const ::container::RestartRequest* request, ::container::RestartReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Restart_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Restart(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::RestartReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Restart_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Restart(::grpc::ClientContext* context, const ::container::RestartRequest* request, ::container::RestartReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Restart_, context, request, response, reactor);
}

void Container::Stub::experimental_async::Restart(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::RestartReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Restart_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::container::RestartReply>* Container::Stub::AsyncRestartRaw(::grpc::ClientContext* context, const ::container::RestartRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::RestartReply>::Create(channel_.get(), cq, rpcmethod_Restart_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::container::RestartReply>* Container::Stub::PrepareAsyncRestartRaw(::grpc::ClientContext* context, const ::container::RestartRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::RestartReply>::Create(channel_.get(), cq, rpcmethod_Restart_, context, request, false);
}

::grpc::Status Container::Stub::Update(::grpc::ClientContext* context, const ::container::UpdateRequest& request, ::container::UpdateReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Update_, context, request, response);
}

void Container::Stub::experimental_async::Update(::grpc::ClientContext* context, const ::container::UpdateRequest* request, ::container::UpdateReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Update_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Update(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::UpdateReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Update_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Update(::grpc::ClientContext* context, const ::container::UpdateRequest* request, ::container::UpdateReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Update_, context, request, response, reactor);
}

void Container::Stub::experimental_async::Update(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::UpdateReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Update_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::container::UpdateReply>* Container::Stub::AsyncUpdateRaw(::grpc::ClientContext* context, const ::container::UpdateRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::UpdateReply>::Create(channel_.get(), cq, rpcmethod_Update_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::container::UpdateReply>* Container::Stub::PrepareAsyncUpdateRaw(::grpc::ClientContext* context, const ::container::UpdateRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::UpdateReply>::Create(channel_.get(), cq, rpcmethod_Update_, context, request, false);
}

::grpc::Status Container::Stub::Kill(::grpc::ClientContext* context, const ::container::KillRequest& request, ::container::KillReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Kill_, context, request, response);
}

void Container::Stub::experimental_async::Kill(::grpc::ClientContext* context, const ::container::KillRequest* request, ::container::KillReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Kill_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Kill(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::KillReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Kill_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Kill(::grpc::ClientContext* context, const ::container::KillRequest* request, ::container::KillReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Kill_, context, request, response, reactor);
}

void Container::Stub::experimental_async::Kill(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::KillReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Kill_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::container::KillReply>* Container::Stub::AsyncKillRaw(::grpc::ClientContext* context, const ::container::KillRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::KillReply>::Create(channel_.get(), cq, rpcmethod_Kill_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::container::KillReply>* Container::Stub::PrepareAsyncKillRaw(::grpc::ClientContext* context, const ::container::KillRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::KillReply>::Create(channel_.get(), cq, rpcmethod_Kill_, context, request, false);
}

::grpc::Status Container::Stub::Status(::grpc::ClientContext* context, const ::container::StatusRequest& request, ::container::StatusReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Status_, context, request, response);
}

void Container::Stub::experimental_async::Status(::grpc::ClientContext* context, const ::container::StatusRequest* request, ::container::StatusReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Status_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Status(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::StatusReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Status_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::Status(::grpc::ClientContext* context, const ::container::StatusRequest* request, ::container::StatusReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Status_, context, request, response, reactor);
}

void Container::Stub::experimental_async::Status(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::StatusReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Status_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::container::StatusReply>* Container::Stub::AsyncStatusRaw(::grpc::ClientContext* context, const ::container::StatusRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::StatusReply>::Create(channel_.get(), cq, rpcmethod_Status_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::container::StatusReply>* Container::Stub::PrepareAsyncStatusRaw(::grpc::ClientContext* context, const ::container::StatusRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::StatusReply>::Create(channel_.get(), cq, rpcmethod_Status_, context, request, false);
}

::grpc::Status Container::Stub::MonitorHistory(::grpc::ClientContext* context, const ::container::MonitorHistoryRequest& request, ::container::MonitorHistoryReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_MonitorHistory_, context, request, response);
}

void Container::Stub::experimental_async::MonitorHistory(::grpc::ClientContext* context, const ::container::MonitorHistoryRequest* request, ::container::MonitorHistoryReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_MonitorHistory_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::MonitorHistory(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::MonitorHistoryReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_MonitorHistory_, context, request, response, std::move(f));
}

void Container::Stub::experimental_async::MonitorHistory(::grpc::ClientContext* context, const ::container::MonitorHistoryRequest* request, ::container::MonitorHistoryReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_MonitorHistory_, context, request, response, reactor);
}

void Container::Stub::experimental_async::MonitorHistory(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::container::MonitorHistoryReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_MonitorHistory_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::container::MonitorHistoryReply>* Container::Stub::AsyncMonitorHistoryRaw(::grpc::ClientContext* context, const ::container::MonitorHistoryRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::MonitorHistoryReply>::Create(channel_.get(), cq, rpcmethod_MonitorHistory_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::container::MonitorHistoryReply>* Container::Stub::PrepareAsyncMonitorHistoryRaw(::grpc::ClientContext* context, const ::container::MonitorHistoryRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::container::MonitorHistoryReply>::Create(channel_.get(), cq, rpcmethod_MonitorHistory_, context, request, false);
}

Container::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Container_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Container::Service, ::container::ListRequest, ::container::ListReply>(
          [](Container::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::container::ListRequest* req,
             ::container::ListReply* resp) {
               return service->List(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Container_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Container::Service, ::container::CreateRequest, ::container::CreateReply>(
          [](Container::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::container::CreateRequest* req,
             ::container::CreateReply* resp) {
               return service->Create(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Container_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Container::Service, ::container::InspectRequest, ::container::InspectReply>(
          [](Container::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::container::InspectRequest* req,
             ::container::InspectReply* resp) {
               return service->Inspect(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Container_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Container::Service, ::container::StartRequest, ::container::StartReply>(
          [](Container::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::container::StartRequest* req,
             ::container::StartReply* resp) {
               return service->Start(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Container_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Container::Service, ::container::StopRequest, ::container::StopReply>(
          [](Container::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::container::StopRequest* req,
             ::container::StopReply* resp) {
               return service->Stop(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Container_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Container::Service, ::container::RemoveRequest, ::container::RemoveReply>(
          [](Container::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::container::RemoveRequest* req,
             ::container::RemoveReply* resp) {
               return service->Remove(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Container_method_names[6],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Container::Service, ::container::RestartRequest, ::container::RestartReply>(
          [](Container::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::container::RestartRequest* req,
             ::container::RestartReply* resp) {
               return service->Restart(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Container_method_names[7],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Container::Service, ::container::UpdateRequest, ::container::UpdateReply>(
          [](Container::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::container::UpdateRequest* req,
             ::container::UpdateReply* resp) {
               return service->Update(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Container_method_names[8],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Container::Service, ::container::KillRequest, ::container::KillReply>(
          [](Container::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::container::KillRequest* req,
             ::container::KillReply* resp) {
               return service->Kill(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Container_method_names[9],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Container::Service, ::container::StatusRequest, ::container::StatusReply>(
          [](Container::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::container::StatusRequest* req,
             ::container::StatusReply* resp) {
               return service->Status(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Container_method_names[10],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Container::Service, ::container::MonitorHistoryRequest, ::container::MonitorHistoryReply>(
          [](Container::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::container::MonitorHistoryRequest* req,
             ::container::MonitorHistoryReply* resp) {
               return service->MonitorHistory(ctx, req, resp);
             }, this)));
}

Container::Service::~Service() {
}

::grpc::Status Container::Service::List(::grpc::ServerContext* context, const ::container::ListRequest* request, ::container::ListReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Container::Service::Create(::grpc::ServerContext* context, const ::container::CreateRequest* request, ::container::CreateReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Container::Service::Inspect(::grpc::ServerContext* context, const ::container::InspectRequest* request, ::container::InspectReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Container::Service::Start(::grpc::ServerContext* context, const ::container::StartRequest* request, ::container::StartReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Container::Service::Stop(::grpc::ServerContext* context, const ::container::StopRequest* request, ::container::StopReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Container::Service::Remove(::grpc::ServerContext* context, const ::container::RemoveRequest* request, ::container::RemoveReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Container::Service::Restart(::grpc::ServerContext* context, const ::container::RestartRequest* request, ::container::RestartReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Container::Service::Update(::grpc::ServerContext* context, const ::container::UpdateRequest* request, ::container::UpdateReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Container::Service::Kill(::grpc::ServerContext* context, const ::container::KillRequest* request, ::container::KillReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Container::Service::Status(::grpc::ServerContext* context, const ::container::StatusRequest* request, ::container::StatusReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Container::Service::MonitorHistory(::grpc::ServerContext* context, const ::container::MonitorHistoryRequest* request, ::container::MonitorHistoryReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace container

