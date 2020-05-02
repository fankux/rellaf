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

#include "brpc/brpc_dispatcher.h"

namespace rellaf {

void BrpcDispatcher::reset(brpc::Server& server) {
    server.ClearServices();
}

int BrpcDispatcher::reg_http_serivces(brpc::Server& server) {
    if (_services.empty()) {
        return -1;
    }

    for (auto& entry : _services) {
        BrpcService* service = entry.second;
        if (service == nullptr) {
            RELLAF_DEBUG("service %s connect failed", entry.first.c_str());
            return -1;
        }

        if (service->get_api_sign_mapper().empty()) {
            RELLAF_DEBUG("service %s no handler", entry.first.c_str());
            return -1;
        }

        std::string api_str;
        for (auto& api_entry : service->get_api_sign_mapper()) {
            api_str += api_entry.first + " => " + api_entry.second + ",\n";
        }
        for (auto& api_entry : service->get_api_path_var_sign_mapper()) {
            api_str += api_entry.first + "/* => " + api_entry.second + ",\n";
        }
        if (!api_str.empty()) {
            api_str.pop_back();
            api_str.pop_back();
        }

        RELLAF_DEBUG("%s api sign :  %s", entry.first.c_str(), api_str.c_str());
        if (server.AddService(dynamic_cast<google::protobuf::Service*> (service),
                brpc::SERVER_OWNS_SERVICE, api_str) != 0) {
            RELLAF_DEBUG("%s api add service failed :  %s", entry.first.c_str(), api_str.c_str());
            return -1;
        }
        RELLAF_DEBUG("%s api sign added", entry.first.c_str());
    }

    return 0;
}

}