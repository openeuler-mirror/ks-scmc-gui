/**
 * @file          /ks-scmc-gui/include/rpc.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef RPC_H_
#define RPC_H_

#include <grpcpp/grpcpp.h>

#include "proto/container.grpc.pb.h"
#include "proto/image.grpc.pb.h"
#include "proto/network.grpc.pb.h"
#include "proto/node.grpc.pb.h"
#include "proto/user.grpc.pb.h"
#include "proto/logging.grpc.pb.h"

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
