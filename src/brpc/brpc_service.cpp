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
#include "handler_mapper.hpp"
#include "brpc/http_status_code.h"

namespace rellaf {

void BrpcService::entry(RpcController* controller, Message* req, Message* resp,
        Closure* done) {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl = dynamic_cast<brpc::Controller*>(controller);

    RELLAF_DEBUG("instance http income, method : %d, content_type : %s",
            cntl->http_request().method(), cntl->http_request().content_type().c_str());

    // check if current api need to forward to master
    std::shared_ptr<Handler> handler = HandlerMapper::instance().create(cntl->http_request());
    if (handler == nullptr) {
        RELLAF_DEBUG("create handler failed");
        cntl->http_request().set_status_code(brpc::HTTP_STATUS_INTERNAL_SERVER_ERROR);
        return_response(cntl, "");
        return;
    }

    std::string ret_body;
    int status = handler->process(cntl->request_attachment(), ret_body);
    cntl->http_response().set_status_code(status);
    return_response(cntl, ret_body);
}

void BrpcService::bind_api_sign(const std::string& api, const std::string& sign) {
    _api_sign_mapper.emplace(api, sign);
    RELLAF_DEBUG("api sign map : %s <==> %s", api.c_str(), sign.c_str());
}

void BrpcService::return_response(brpc::Controller* cntl, const std::string& raw) {
    cntl->http_response().set_content_type("application/json");
    RELLAF_DEBUG("instance operation finish, response : %s", raw.c_str());

    butil::IOBufBuilder os;
    os << raw << std::endl;
    os.move_to(cntl->response_attachment());
}

}