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
#include "brpc/http_method.h"
#include "common.h"
#include "model.h"
#include "function_mapper.hpp"
#include "http_arg_type.h"

namespace rellaf {

using ::google::protobuf::RpcController;
using ::google::protobuf::Service;
using ::google::protobuf::Message;
using ::google::protobuf::Closure;

using brpc::HttpHeader;
using brpc::HttpMethod;

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
    virtual void bind_api_sign(const std::string& sign, const std::string& api);

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
class Reg {                                                                                     \
public:                                                                                         \
    Reg(_clazz_* inst, const std::string& sign, const std::string& api,                         \
            const std::string& name, HttpMethod method,                                         \
            std::function<int(HttpContext&, const std::string&, std::string&)> func) {          \
        inst->bind_api_sign(sign, api);                                                         \
        FunctionMapper::instance().reg(api, name, method, func);                                \
    }                                                                                           \
}

// definition brpc request entry method signature fowarding call BrpcService::entry
#define RELLAF_BRPC_HTTP_DEF_SIGN(_sign_)                                                       \
public:                                                                                         \
void _sign_(RpcController* controller, const pb_req_t* request,                                 \
        pb_resp_t* response, Closure* done) override {                                          \
    BrpcService::entry(controller, (Message*)request, (Message*)response, done);                \
}

template<class T>
void flatten_args(std::deque<Model*>& args, T& arg) {
    if (std::is_base_of<Model, T>::value) {
        args.emplace_back(&arg);
    }
}

template<class ...Args>
bool prepare_args(HttpContext& ctx, const std::string& body, Args ... args) {

    std::deque<Model*> model_args;
    bool arr[] = {(flatten_args(model_args, args), true)...}; // for arguments expansion
    (void)(arr);// suppress warning

    bool s;
    for (Model* arg : model_args) {
        if (arg->rellaf_tags().front() == HttpArgTypeEnum::e().REQ_BODY.name) {
            if (is_plain(arg)) {
                s = arg->set_parse(body);
            } else {
                s = json_to_model(body, arg);
            }
            if (!s) {
                return false;
            }
            continue;
        }

        if (arg->rellaf_tags().front() == HttpArgTypeEnum::e().REQ_PARAM.name) {

            if (is_object(arg)) {
                const brpc::URI& uri = ctx.request_header.uri();
                for (auto iter = uri.QueryBegin(); iter != uri.QueryEnd(); ++iter) {
                    ((Object*)arg)->set_plain(iter->first, iter->second);
                }
            }
            continue;
        }

        if (arg->rellaf_tags().front() == HttpArgTypeEnum::e().PATH_VAR.name) {

            if (is_object(arg)) {
                for (auto& entry : ctx.path_vars) {
                    ((Object*)arg)->set_plain(entry.first, entry.second);
                }
            }
            continue;
        }
    }
    return true;
}


#define rellaf_brpc_http_def_api(_sign_, _api_, _method_, _func_, _Ret_, _Params_, _Vars_, _Body_) \
RELLAF_BRPC_HTTP_DEF_SIGN(_sign_)                                                                  \
private:                                                                                           \
    Reg _reg_##_sign_##_method_##_func_{this, #_sign_, _api_, #_sign_"-"#_method_"-"#_func_,       \
        HttpMethod::HTTP_METHOD_##_method_,                                                        \
        [this] (HttpContext& ctx, const std::string& body, std::string& ret_body) {                \
            _Params_ p;                                                                            \
            _Vars_ v;                                                                              \
            _Body_ b;                                                                              \
            if (!prepare_args<_Params_, _Vars_, _Body_>(ctx, body,                                 \
                    p.tag<_Params_>(HttpArgTypeEnum::e().REQ_PARAM.name),                          \
                    v.tag<_Vars_>(HttpArgTypeEnum::e().PATH_VAR.name),                             \
                    b.tag<_Body_>(HttpArgTypeEnum::e().REQ_BODY.name))) {                          \
                return -1;                                                                         \
            }                                                                                      \
            _Ret_ ret = _func_##_base(ctx, p, v, b);                                               \
            if (is_plain(&ret)) {                                                                  \
                ret_body = ((Model*)&ret)->str();                                                  \
            } else {                                                                               \
                if (!model_to_json(&ret, ret_body)) {                                              \
                    return -1;                                                                     \
                }                                                                                  \
            }                                                                                      \
            return 0;                                                                              \
        }                                                                                          \
    };                                                                                             \
    _Ret_ _func_##_base(HttpContext& ctx, const _Params_& p, const _Vars_& v, const _Body_& b)


#define rellaf_brpc_http_def_get(_sign_, _api_, _func_, _Ret_, _Params_, _Vars_)                \
private:                                                                                        \
    rellaf_brpc_http_def_api(_sign_, _api_, GET, _func_, _Ret_, _Params_, _Vars_, Void) {       \
        return std::forward<_Ret_>(_func_(ctx, p, v));                                          \
    }                                                                                           \
public:                                                                                         \
    _Ret_ _func_(HttpContext& ctx, const _Params_& p, const _Vars_& v)

#define rellaf_brpc_http_def_get_param(_sign_, _api_, _func_, _Ret_, _Params_)                  \
private:                                                                                        \
    rellaf_brpc_http_def_get(_sign_, _api_, _func_, _Ret_, _Params_, Void) {                    \
        return std::forward<_Ret_>(_func_(ctx, p));                                             \
    }                                                                                           \
public:                                                                                         \
    _Ret_ _func_(HttpContext& ctx, const _Params_& p)

#define rellaf_brpc_http_def_get_pathvar(_sign_, _api_, _func_, _Ret_, _Vars_)                  \
private:                                                                                        \
    rellaf_brpc_http_def_get(_sign_, _api_, _func_, _Ret_, Void, _Vars_) {                      \
        return std::forward<_Ret_>(_func_(ctx, v));                                             \
    }                                                                                           \
public:                                                                                         \
    _Ret_ _func_(HttpContext& ctx, const _Vars_& v)


#define rellaf_brpc_http_def_post(_sign_, _api_, _func_, _Ret_, _Params_, _Vars_, _Body_)       \
private:                                                                                        \
    rellaf_brpc_http_def_api(_sign_, _api_, POST, _func_, _Ret_, _Params_, _Vars_, _Body_) {    \
        return std::forward<_Ret_>(_func_(ctx, p, v, b));                                       \
    }                                                                                           \
public:                                                                                         \
    _Ret_ _func_(HttpContext& ctx, const _Params_& p, const _Vars_& v, const _Body_& b)

#define rellaf_brpc_http_def_post_body(_sign_, _api_, _func_, _Ret_, _Body_)                    \
private:                                                                                        \
    rellaf_brpc_http_def_post(_sign_, _api_, _func_, _Ret_, Void, Void, _Body_) {               \
        return std::forward<_Ret_>(_func_(ctx, b));                                             \
    }                                                                                           \
public:                                                                                         \
    _Ret_ _func_(HttpContext& ctx, const _Body_& b)

#define rellaf_brpc_http_def_post_param(_sign_, _api_, _func_, _Ret_, _Params_)                 \
private:                                                                                        \
    rellaf_brpc_http_def_post(_sign_, _api_, _func_, _Ret_, _Params_, Void, Void) {             \
        return std::forward<_Ret_>(_func_(ctx, p));                                             \
    }                                                                                           \
public:                                                                                         \
    _Ret_ _func_(HttpContext& ctx, const _Params_& p)

#define rellaf_brpc_http_def_post_pathvar(_sign_, _api_, _func_, _Ret_, _Vars_)                 \
private:                                                                                        \
    rellaf_brpc_http_def_post(_sign_, _api_, _func_, _Ret_, Void, _Vars_, Void) {               \
        return std::forward<_Ret_>(_func_(ctx, v));                                             \
    }                                                                                           \
public:                                                                                         \
    _Ret_ _func_(HttpContext& ctx, const _Vars_& v)

#define rellaf_brpc_http_def_post_param_body(_sign_, _api_, _func_, _Ret_, _Params_, _Body_)    \
private:                                                                                        \
    rellaf_brpc_http_def_post(_sign_, _api_, _func_, _Ret_, _Params_, Void, _Body_) {           \
        return std::forward<_Ret_>(_func_(ctx, p, b));                                          \
    }                                                                                           \
public:                                                                                         \
    _Ret_ _func_(HttpContext& ctx, const _Params_& p, const _Body_& b)

#define rellaf_brpc_http_def_post_pathvar_body(_sign_, _api_, _func_, _Ret_, _Vars_, _Body_)    \
private:                                                                                        \
    rellaf_brpc_http_def_post(_sign_, _api_, _func_, _Ret_, Void, _Vars_, _Body_) {             \
        return std::forward<_Ret_>(_func_(ctx, v, b));                                          \
    }                                                                                           \
public:                                                                                         \
    _Ret_ _func_(HttpContext& ctx, const _Vars_& v, const _Body_& b)

#define rellaf_brpc_http_def_post_param_pathvar(_sign_, _api_, _func_, _Ret_, _Params_, _Vars_) \
private:                                                                                        \
    rellaf_brpc_http_def_post(_sign_, _api_, _func_, _Ret_, _Params_, _Vars_, Void) {           \
        return std::forward<_Ret_>(_func_(ctx, p, v));                                          \
    }                                                                                           \
public:                                                                                         \
    _Ret_ _func_(HttpContext& ctx, const _Params_& p, const _Vars_& v)


} // namespace
