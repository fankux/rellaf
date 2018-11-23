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

namespace rellaf {

struct HttpResponse {
    butil::IOBuf body;
    int status;
    std::string content_type;
    std::string reason_phrase;
    brpc::CompressType compress_type;
};

int http_request(const std::string& api, brpc::HttpMethod method,
        const std::map<std::string, std::string>& params,
        const std::map<std::string, std::string>& headers,
        const std::string& content_type, const std::string& request_body,
        HttpResponse& response, int timeout = -1);

int http_get(const std::string& api, const std::map<std::string, std::string>& params,
        HttpResponse& response, int timeout = -1);

int http_post(const std::string& api, const std::map<std::string, std::string>& params,
        const std::string& request_body, HttpResponse& response, int timeout = -1);

}