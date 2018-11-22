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

#include "brpc/server.h"
#include "common.h"
#include "brpc_service.h"

namespace rellaf {

class BrpcDispatcher {
RELLAF_SINGLETON(BrpcDispatcher);
RELLAF_AVOID_COPY(BrpcDispatcher);

public:
    virtual ~BrpcDispatcher() {
        for (auto& entry : _services) {
            delete entry.second;
        }
    }

    void reset(brpc::Server& server);

    int reg_http_serivces(brpc::Server& server);

    template<class T>
    class Reg {
    public:
        explicit Reg(const std::string& service_name) {
            auto& mp = BrpcDispatcher::instance()._services;
            if (mp.find(service_name) == mp.end()) {
                T* service = new(std::nothrow) T();
                if (service == nullptr) {
                    RELLAF_DEBUG("critical!!!allocate service failed");
                    exit(-1);
                }
                service->bind_pb_serivce(service);
                mp.emplace(service_name, service);
                RELLAF_DEBUG("service %s registered", service_name.c_str());
            }
        }
    };

private:
    // <service_name, service_ptr>
    std::unordered_map<std::string, BrpcService*> _services;
};

#define RELLAF_DEF_BRPC_HTTP_SERVICE(_class_)    \
static BrpcDispatcher::Reg<_class_> obj_##_class_(#_class_)

}
