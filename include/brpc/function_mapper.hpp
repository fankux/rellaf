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
#include "str.hpp"

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
    /**
     * @param header    HTTP header
     * @param vars      path variables, <variable name, input value>
     * @return
     */
    std::string fetch_name(const HttpHeader& header, std::map<std::string, std::string>& vars) {
        std::string api = header.uri().path();
        FLOG(DEBUG) << "input api: " << api;
        if (api.front() != '/') {
            api = "/" + api;
        }
        // FIXME.. generalize HTTP API

        auto entry = _api_hdrs.find(api);
        if (entry != _api_hdrs.end()) {
            return entry->second;
        }

        std::string name;
        if (!_path_vars.fetch_vars(api, name, vars)) {
            RELLAF_DEBUG("api not exist : %s", api.c_str());
            return "";
        }
        FLOG(DEBUG) << "vars: " << vars << ", name: " << name;
        return name;
    }

    /**
     * @param name      handler name
     * @param vars      path variables, <variable name, input value>
     * @param cntl      http context
     * @param ret_body
     * @return
     */
    int invoke(const std::string& name, const std::map<std::string, std::string>& vars,
            brpc::Controller* cntl, std::string& ret_body) {
        auto func_entry = _funcs.find(name);
        if (func_entry == _funcs.end()) {
            return -1;
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
        // FIXME.. generalize HTTP API
        std::string api_filter = api;
        trim(api_filter);
        if (api_filter.front() != '/') {
            api_filter = '/' + api_filter;
        }
        FunctionMapper::instance()._api_hdrs.emplace(api_filter, name);

        std::string path_var_prefix;
        if (UrlPattern::fetch_path_vars_prefix(api_filter, path_var_prefix)) {
            _path_vars.put(api_filter, name);
        }
    }

private:

    // <api, name>
    std::unordered_map<std::string, std::string> _api_hdrs;

    UrlTrie _path_vars;

    // request body as json string parsing to model,
    // return value as model convert json string as well
    // function with http context
    std::unordered_map<std::string,
            std::function<int(HttpContext&, const std::string&, std::string&)>> _funcs;
};

}