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

#include <mysql/mysql_simple_pool.h>
#include "cweb_service.h"

namespace rellaf {

rellaf_brpc_http_def(CWebSerivceImpl);

Plain<int> CWebSerivceImpl::add(HttpContext& ctx, const Info& info) {
    LOG(INFO) << "add input : " << info.debug_str();

    uint64_t key_id = 0;
    if (_dao.add_info(key_id, info)) {
        return -1;
    }
    return key_id;
}

Plain<int> CWebSerivceImpl::del(HttpContext& ctx, const Plain<uint64_t>& id) {
    LOG(INFO) << "delete input id : " << id.debug_str();

    return _dao.delete_info(id) >= 0;
}

Plain<int> CWebSerivceImpl::update(HttpContext& ctx, const Info& info) {
    LOG(INFO) << "update input : " << info.debug_str();

    return _dao.update_info(info) >= 0;
}

List CWebSerivceImpl::query(HttpContext& ctx, const Limit& limit) {
    LOG(INFO) << "query input : " << limit.debug_str();

    List ret;

    std::deque<Info> infos;
    if (_dao.query_infos(infos, limit) > 0) {
        for (auto& info : infos) {
            ret.push_back(info);
        }
    }
    return ret;
}

}

int main() {

    bool ret = rellaf::MysqlSimplePool::instance().init("127.0.0.1", 3306, "root", "root", "rellaf");
    if (!ret) {
        LOG(ERROR) << "connect to db failed";
        return -1;
    }

    brpc::Server server;

    rellaf::BrpcDispatcher::instance().reg_http_serivces(server);
    brpc::ServerOptions options;
    options.idle_timeout_sec = 30;
    options.has_builtin_services = false;

    // it run background
    if (server.Start(8124, &options) < 0) {
        LOG(ERROR) << "start server failed";
        return -1;
    }

    return server.Join();
}
