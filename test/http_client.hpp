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

#include <string>
#include <map>
#include "brpc/controller.h"
#include "brpc/channel.h"
#include "common.h"

namespace rellaf {

struct HttpResponse {
    butil::IOBuf message;
    int status;
};

int http_request(const std::string& api, brpc::HttpMethod method,
        const std::map<std::string, std::string>& params,
        const std::map<std::string, std::string>& headers,
        const std::string& request_body, HttpResponse& response,
        const std::string& content_type, int timeout = -1) {
    brpc::Controller controller;
    brpc::ChannelOptions options;
    brpc::Channel channel;
    options.protocol = brpc::PROTOCOL_HTTP;
    options.timeout_ms = timeout;
    options.connect_timeout_ms = 600000;

    brpc::URI uri;
    uri.SetHttpURL(api);

    if (channel.Init(api.c_str(), &options) != 0) {
        RELLAF_DEBUG("fail to initialize baidu_rpc channel, api : %s", api.c_str());
        return -1;
    }

    controller.http_request().set_method(method);
    controller.http_request().uri() = api;
    if (!content_type.empty()) {
        controller.http_request().set_content_type(content_type);
    }

    for (auto& entry : headers) {
        controller.http_request().SetHeader(entry.first, entry.second);
    }

    for (auto& entry : params) {
        controller.http_request().uri().SetQuery(entry.first, entry.second);
    }

    if (!request_body.empty()) {
        butil::IOBufBuilder os;
        os << request_body;
        os.move_to(controller.request_attachment());
    }

    std::string port_sign = (uri.port() <= 0) ? "" : ":" + std::to_string(uri.port());
    const std::string& request_url = uri.host();
    const std::string& request_query = uri.path() + "?" + uri.query();
    RELLAF_DEBUG("http %s request : http://%s%s%s, payload : %s", brpc::HttpMethod2Str(method),
            request_url.c_str(), port_sign.c_str(), request_query.c_str(),
            request_body.c_str());

    channel.CallMethod(nullptr, &controller, nullptr, nullptr, nullptr);
    response.status = controller.http_response().status_code();
    response.message.swap(controller.response_attachment());

    if (controller.Failed() || controller.IsCanceled()) {
        RELLAF_DEBUG("invoke %s failed, api : http://%s%s%s, message : %s",
                brpc::HttpMethod2Str(method), request_url.c_str(), port_sign.c_str(),
                request_query.c_str(), controller.ErrorText().c_str());
        return -1;
    }

    return 0;
}

int http_get(const std::string& api, const std::map<std::string, std::string>& params,
        HttpResponse& response, int timeout = -1) {
    return http_request(api, brpc::HTTP_METHOD_GET, params, {}, "", response, "", timeout);
}

int http_post(const std::string& api, const std::map<std::string, std::string>& params,
        const std::string& request_body, HttpResponse& response, int timeout = -1) {
    return http_request(api, brpc::HTTP_METHOD_POST, params, {},
            request_body, response, "", timeout);
}

}