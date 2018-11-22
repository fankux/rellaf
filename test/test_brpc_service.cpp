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

#include "gtest/gtest.h"
#include "test_common.h"
#include "brpc_dispatcher.h"
#include "test_service.pb.h"
#include "http_client.hpp"

namespace rellaf {
namespace test {

static brpc::Server server;

int run_brpc() {
    brpc::ServerOptions options;
    options.idle_timeout_sec = 30;
    options.has_builtin_services = true;

    // it run background
    return server.Start(8123, &options);
}

int stop_brpc(int wait_time) {
    server.Stop(wait_time);
    server.Join();
    return 0;
}

class TestBrpcService : public testing::Test {
protected:
    TestBrpcService() = default;

    ~TestBrpcService() override = default;

    void SetUp() override {
        stop_brpc(-1);

        BrpcDispatcher::instance().reset(server);
        BrpcDispatcher::instance().reg_http_serivces(server);

        run_brpc();
    }
};

class EchoHandler : public Handler {
public:
    explicit EchoHandler(const HttpHeader& header) : Handler(header) {}

    int process(const butil::IOBuf& body, std::string& ret_body) override {
        RELLAF_DEBUG("echo income");
        return 404;
    }
};

class TestSerivceImpl : public BrpcService, public TestService {
public:
    TestSerivceImpl() {
        RELLAF_DEF_BRPC_HTTP_API("/aaa", echo, EchoHandler);
    };

RELLAF_DCL_BRPC_HTTP_SERVICE(TestSerivceImpl);

RELLAG_DEF_BRPC_HTTP_SIGN(echo, TestRequest, TestResponse);
};

RELLAF_DEF_BRPC_HTTP_SERVICE(TestSerivceImpl);

TEST_F(TestBrpcService, echo_service) {

    HttpResponse response;
    http_post("127.0.0.1:8123/aaa", {}, {}, response);

    sleep(10);
}

}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}