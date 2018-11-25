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
#include "handler.h"

namespace rellaf {

template<class T>
class HandlerFactory {
public:
    static T* instance() {
        static T inst;
        return &inst;
    }
};

class HandlerMapper {
rellaf_singleton(HandlerMapper)

public:
    Handler* create(const HttpHeader& header, bool& exist) {
        const std::string& api = header.uri().path();
        auto ah_entry = _api_hdrs.find(api);
        if (ah_entry == _api_hdrs.end()) {
            RELLAF_DEBUG("api not exist : %s", api.c_str());
            exist = false;
            return nullptr;
        }
        const std::string& name = ah_entry->second;

        exist = true;
        Handler* inst = _creators[name]();
        if (inst != nullptr) {
            inst->_name = name;
        }
        return inst;
    }

    void free(Handler* handler) {
        if (handler == nullptr) {
            return;
        }
        _deletors[handler->_name](handler);
    }

    template<class T>
    class Reg {
    public:
        Reg(const std::string& name, const std::string& api, HttpMethod method, bool singleton) {
            std::string api_filter = api;
            trim(api_filter);
            HandlerMapper::instance()._api_hdrs.insert(std::make_pair(api_filter, name));
            HandlerMapper::instance()._creators[name] = [singleton]() {
                return singleton ? HandlerFactory<T>::instance() : new T();
            };
            HandlerMapper::instance()._deletors[name] = [singleton](Handler* handler) {
                if (!singleton) {
                    delete handler;
                }
            };
            RELLAF_DEBUG("default handler %s registered", name.c_str());
        }
    };

private:
    // handler creator functional
    std::unordered_map<std::string, std::function<Handler*()>> _creators;
    std::unordered_map<std::string, std::function<void(Handler*)>> _deletors;

    // api ==> handler_name
    std::unordered_map<std::string, std::string> _api_hdrs;

    // handler_name ==> ( method ==> Model prototypes )
    std::unordered_map<std::string, std::unordered_map<std::string, Model*>> _model_prototypes;
};

}