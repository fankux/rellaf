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
#include "model.h"
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

#define rellaf_brpc_http_dcl(_clazz_, _pb_req_t_, _pb_resp_t_)                                  \
public:                                                                                         \
using pb_req_t = _pb_req_t_;                                                                    \
using pb_resp_t = _pb_resp_t_;                                                                  \
void bind_pb_serivce(::google::protobuf::Service* base_service) override {                      \
    for (int i = 0; i < _clazz_::descriptor()->method_count(); ++i) {                           \
        const ::google::protobuf::MethodDescriptor* method = _clazz_::descriptor()->method(i);  \
        RELLAF_DEBUG("protobuf service :%s, methd: %s",                                         \
                _clazz_::descriptor()->name().c_str(), method->name().c_str());                 \
        _sign_entrys.insert(method->name());                                                    \
    }                                                                                           \
}                                                                                               \
private:                                                                                        \
template<class Handler>                                                                         \
class Reg {                                                                                     \
public:                                                                                         \
    Reg(_clazz_* inst, const std::string& sign, const std::string& api,                         \
            const std::string& handler, bool singleton) {                                       \
        HandlerMapper::Reg<Handler> _handler(handler, api, (HttpMethod)0, singleton);           \
        inst->bind_api_sign(api, sign);                                                         \
    }                                                                                           \
}

// definition brpc request entry method signature fowarding call BrpcService::entry
#define RELLAF_BRPC_HTTP_DEF_SIGN(_sign_)                                                       \
public:                                                                                         \
void _sign_(RpcController* controller, const pb_req_t* request,                                 \
        pb_resp_t* response, Closure* done) override {                                          \
    BrpcService::entry(controller, (Message*)request, (Message*)response, done);                \
}

#define rellaf_brpc_http_def_api(_sign_, _api_, _handler_, _singleton_)                         \
RELLAF_BRPC_HTTP_DEF_SIGN(_sign_)                                                               \
private:                                                                                        \
    Reg<_handler_> _reg_##_sign_##_##_handler_{this, #_sign_, _api_, #_handler_, _singleton_}

}
