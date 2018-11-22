// Copyright 2018 Fankux
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: Fankux (fankux@gmail.com)
//

#pragma once

#include <unordered_map>
#include <unordered_set>
#include "google/protobuf/service.h"
#include "brpc/server.h"
#include "common.h"
#include "handler_mapper.hpp"

namespace rellaf {

using ::google::protobuf::RpcController;
using ::google::protobuf::Service;
using ::google::protobuf::Message;
using ::google::protobuf::Closure;

class BrpcService {
RELLAF_AVOID_COPY(BrpcService);

public:
    BrpcService() = default;

    virtual ~BrpcService() = default;


    /**
     * http request entry,
     * baidu rpc service implementation,
     * "controller" contain http protocol context
     */
    void entry(RpcController* controller, Message* request, Message* response, Closure* done);

    const std::unordered_map<std::string, std::string>& get_api_sign_mapper() const {
        return _api_sign_mapper;
    }

protected:
    virtual void bind_api_sign(const std::string& api, const std::string& sign);

    virtual void bind_pb_serivce(Service* service) = 0;

private:
    void return_response(brpc::Controller* cntl, const std::string& raw);

protected:
    std::unordered_set<std::string> _sign_entrys;
    std::unordered_map<std::string, std::string> _api_sign_mapper;

};

#define RELLAF_DEF_BRPC_HTTP_API(_api_, _sign_, _handler_) do {                                 \
    bind_api_sign(_api_, #_sign_);                                                              \
    static HandlerMapper::Reg<_handler_> hdr_##_handler_(#_handler_, _api_, (HttpMethod)0);     \
} while (0)

// definition brpc request entry method signature fowarding call BrpcService::entry
#define RELLAG_DEF_BRPC_HTTP_SIGN(_pb_api_, _pb_reg_t_, _pb_resp_t_)                            \
public:                                                                                         \
void _pb_api_(RpcController* controller, const _pb_reg_t_* request,                             \
        _pb_resp_t_* response, Closure* done) override {                                        \
    BrpcService::entry(controller, (Message*)request, (Message*)response, done);                \
}

#define RELLAF_DCL_BRPC_HTTP_SERVICE(_name_)                                                    \
public:                                                                                         \
void bind_pb_serivce(::google::protobuf::Service* base_service) override {                      \
    for (int i = 0; i < _name_::descriptor()->method_count(); ++i) {                            \
        const ::google::protobuf::MethodDescriptor* method = _name_::descriptor()->method(i);   \
        RELLAF_DEBUG("protobuf service :%s, methd: %s",                                         \
                _name_::descriptor()->name().c_str(), method->name().c_str());                  \
        _sign_entrys.insert(method->name());                                                    \
    }                                                                                           \
}

}
