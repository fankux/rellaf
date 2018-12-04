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

#include <functional>
#include <unordered_map>
#include <memory>
#include "common.h"
#include "trim.hpp"

#include "json_to_model.h"

namespace rellaf {

using brpc::HttpHeader;
using brpc::HttpMethod;

class HttpContext {
    const HttpHeader& request_header;
    const butil::IOBuf& request_body;
    HttpHeader& response_header;
    butil::IOBuf& response_body;

public:
    HttpContext(const HttpHeader& req_header, const butil::IOBuf& req_body,
            HttpHeader& resp_header, butil::IOBuf& resp_body) :
            request_header(req_header),
            request_body(req_body),
            response_header(resp_header),
            response_body(resp_body) {}
};

class FunctionMapper {
rellaf_singleton(FunctionMapper)

public:
    std::string fetch_name(const HttpHeader& header) {
        const std::string& api = header.uri().path();
        auto entry = _api_hdrs.find(api);
        if (entry == _api_hdrs.end()) {
            RELLAF_DEBUG("api not exist : %s", api.c_str());
            return "";
        }
        return entry->second;
    }

    int invoke(const std::string& name, brpc::Controller* cntl, std::string& ret_body) {
        int ret = -1;
        auto func_entry = _funcs.find(name);
        if (func_entry != _funcs.end()) {
            ret = (func_entry->second)(cntl->request_attachment().to_string(), ret_body);
            cntl->http_response().set_content_type("application/json");
            return ret;
        }

        HttpContext ctx(cntl->http_request(), cntl->request_attachment(), cntl->http_response(),
                cntl->response_attachment());
        auto ctx_func_entry = _ctx_funcs.find(name);
        if (ctx_func_entry != _ctx_funcs.end()) {
            ret = (ctx_func_entry->second)(cntl->request_attachment().to_string(), ret_body, ctx);
            cntl->http_response().set_content_type("application/json");
            return ret;
        }

        return -1;
    }

    void reg(const std::string& api, const std::string& name, HttpMethod method,
            std::function<int(const std::string&, std::string&)> func) {
        reg_api(api, name);
        FunctionMapper::instance()._funcs.emplace(name, func);
        RELLAF_DEBUG("default handler %s registered", name.c_str());
    }

    void reg(const std::string& api, const std::string& name, HttpMethod method,
            std::function<int(const std::string&, std::string&, HttpContext&)> ctx_func) {
        reg_api(api, name);
        FunctionMapper::instance()._ctx_funcs.emplace(name, ctx_func);
        RELLAF_DEBUG("default handler %s registered", name.c_str());
    }

private:
    void reg_api(const std::string& api, const std::string& name) {
        std::string api_filter = api;
        trim(api_filter);
        FunctionMapper::instance()._api_hdrs.emplace(api_filter, name);
    }

private:

    // api ==> name
    std::unordered_map<std::string, std::string> _api_hdrs;

    // request body as json string parsing to model,
    // return value as model convert json string as well
    std::unordered_map<std::string, std::function<int(const std::string&, std::string&)>> _funcs;

    // function with http context
    std::unordered_map<std::string,
            std::function<int(const std::string&, std::string&, HttpContext&)>> _ctx_funcs;
};

}