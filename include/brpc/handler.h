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

#include "brpc/http_method.h"

namespace rellaf {

using HttpHeader = brpc::HttpHeader;
using HttpMethod = brpc::HttpMethod;

// handler_mapper new an object as a std::shared_ptr at each request
class Handler {
public:
    friend class HandlerMapper;

    template<class T> friend class SingletonFactory;

    virtual ~Handler() = default;

    /**
     * @brief   processing in one step
     * @param   body
     * @param   ret_head response header
     * @param   ret_body response paylaod
     * @return  http status code
     */
    virtual int process(const HttpHeader& head, const butil::IOBuf& body, HttpHeader& ret_head,
            std::string& ret_body) = 0;

protected:
    explicit Handler() = default;

protected:
    std::string _name;
};

}