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

#include "brpc_dispatcher.h"
#include "cweb_service.pb.h"
#include "cweb_dao.h"
#include "info.h"
#include "limit_model.h"

namespace rellaf {

class CWebSerivceImpl : public BrpcService, public CWebService {

rellaf_brpc_http_dcl(CWebSerivceImpl, CWebRequest, CWebResponse);

rellaf_brpc_http_def_post_body(add, "/rellaf/demo/add", add, Plain<int>, Info);

rellaf_brpc_http_def_post_pathvar(del, "/rellaf/demo/del/{id}", del, Plain<int>, Plain<uint64_t>);

rellaf_brpc_http_def_post_body(update, "/rellaf/demo/update", update, Plain<int>, Info);

rellaf_brpc_http_def_get_pathvar(query, "/rellaf/demo/query/{start}", query, List, Limit);


private:
    CWebDao& _dao = CWebDao::instance();
};

}
