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

#include "var_pattern.h"
#include "json/json_to_model.h"

namespace rellaf {

using brpc::HttpHeader;
using brpc::HttpMethod;

struct HttpContext {
    const HttpHeader& request_header;
    const butil::IOBuf& request_body;
    const std::map<std::string, std::string>& path_vars;
    HttpHeader& response_header;
    butil::IOBuf& response_body;

    HttpContext(const HttpHeader& req_header, const butil::IOBuf& req_body,
            const std::map<std::string, std::string>& vars,
            HttpHeader& resp_header, butil::IOBuf& resp_body) :
            request_header(req_header),
            request_body(req_body),
            path_vars(vars),
            response_header(resp_header),
            response_body(resp_body) {}
};

class FunctionMapper {
rellaf_singleton(FunctionMapper)

public:
    std::string fetch_name(const HttpHeader& header) {
        const std::string& api = header.uri().path();
        auto entry = _api_hdrs.find(api.front() == '/' ? api : ('/' + api));
        if (entry == _api_hdrs.end()) {
            RELLAF_DEBUG("api not exist : %s", api.c_str());
            return "";
        }
        return entry->second;
    }

    int invoke(const std::string& name, brpc::Controller* cntl, std::string& ret_body) {
        auto func_entry = _funcs.find(name);
        if (func_entry == _funcs.end()) {
            return -1;
        }

        std::map<std::string, std::string> vars;
        auto var_entry = _path_vars.find(name);
        if (var_entry != _path_vars.end()) {
            if (!UrlPattern::fetch_path_vars(cntl->http_request().uri().path(),
                    var_entry->second, vars)) {
                return -1;
            }
        }

        HttpContext ctx(cntl->http_request(), cntl->request_attachment(), vars,
                cntl->http_response(), cntl->response_attachment());
        int ret = (func_entry->second)(ctx, cntl->request_attachment().to_string(), ret_body);
        cntl->http_response().set_content_type("application/json");
        return ret;
    }

    void reg(const std::string& api, const std::string& name, HttpMethod method,
            std::function<int(HttpContext&, const std::string&, std::string&)> ctx_func) {
        reg_api(api, name);
        FunctionMapper::instance()._funcs.emplace(name, ctx_func);
        RELLAF_DEBUG("default handler %s registered", name.c_str());
    }

private:
    void reg_api(const std::string& api, const std::string& name) {
        std::string api_filter = api;
        trim(api_filter);
        if (api_filter.front() != '/') {
            api_filter = '/' + api_filter;
        }
        FunctionMapper::instance()._api_hdrs.emplace(api_filter, name);

        PatternErr err;
        std::map<uint32_t, std::string> vars;
        if (!UrlPattern::explode_path_vars(api_filter, vars, err)) {
            RELLAF_DEBUG("api %s invalid, explode path vars failed : %d", api_filter.c_str(), err);
            return;
        }

        _path_vars.emplace(api_filter, vars);
    }

private:

    // api ==> name
    std::unordered_map<std::string, std::string> _api_hdrs;

    // api ==> indexes to path variables
    std::unordered_map<std::string, std::map<uint32_t, std::string>> _path_vars;

    // request body as json string parsing to model,
    // return value as model convert json string as well
    // function with http context
    std::unordered_map<std::string,
            std::function<int(HttpContext&, const std::string&, std::string&)>> _funcs;
};

}