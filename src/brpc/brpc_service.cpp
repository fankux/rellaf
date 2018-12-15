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

#include "common.h"
#include "brpc_service.h"
#include "function_mapper.hpp"
#include "brpc/http_status_code.h"

namespace rellaf {

void BrpcService::entry(RpcController* controller, Message* req, Message* resp,
        Closure* done) {
    RELLAF_UNUSED(req);
    RELLAF_UNUSED(resp);

    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl = dynamic_cast<brpc::Controller*>(controller);

    std::string name = FunctionMapper::instance().fetch_name(cntl->http_request());
    if (name.empty()) {
        cntl->http_response().set_status_code(brpc::HTTP_STATUS_NOT_FOUND);
        return_response(cntl, "");
        return;
    }

    std::string ret_body;
    int status = FunctionMapper::instance().invoke(name, cntl, ret_body);
    if (status == -1) {
        cntl->http_request().set_status_code(brpc::HTTP_STATUS_INTERNAL_SERVER_ERROR);
    }
    return_response(cntl, ret_body);
}

void BrpcService::bind_api_sign(const std::string& sign, const std::string& api) {
    if (api.empty()) {
        RELLAF_DEBUG("ignore empty api of %s", sign.c_str());
        return;
    }
    _api_sign_mapper.emplace(api, sign);
    RELLAF_DEBUG("api sign map : %s <==> %s", api.c_str(), sign.c_str());
}

void BrpcService::return_response(brpc::Controller* cntl, const std::string& raw) {
    butil::IOBufBuilder os;
    os << raw;
    os.move_to(cntl->response_attachment());
}

}