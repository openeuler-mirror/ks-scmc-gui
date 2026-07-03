#ifndef RPC_H_
#define RPC_H_

#include <grpcpp/grpcpp.h>

#include "proto/container_service.grpc.pb.h"
#include "proto/image_service.grpc.pb.h"
#include "proto/network_service.grpc.pb.h"
#include "proto/node_service.grpc.pb.h"
#include "proto/user_service.grpc.pb.h"

/*
 * Example:
    auto chan = get_rpc_channel(address);
    if (m_Channel != nullptr) {
        NodeClient cli(m_Channel);
        cli.List(request);
    }
 */
std::shared_ptr<grpc::Channel> get_rpc_channel(const std::string& addr);

#endif
