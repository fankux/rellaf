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
#include "http_client.h"

namespace rellaf {
namespace test {

static brpc::Server server;

int run_brpc() {
    brpc::ServerOptions options;
    options.idle_timeout_sec = 30;
    options.has_builtin_services = false;

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

    ~TestBrpcService() override {
        stop_brpc(10);
    };

    void SetUp() override {
        BrpcDispatcher::instance().reg_http_serivces(server);
        run_brpc();
    }

    void TearDown() override {
        stop_brpc(-1);
        BrpcDispatcher::instance().reset(server);
    }
};

class EchoHandler : public Handler {
public:
    explicit EchoHandler(const HttpHeader& header) : Handler(header) {}

    int process(const butil::IOBuf& body, HttpHeader& ret_header, std::string& ret_body) override {
        ret_body = "EchoHandler";
        return 200;
    }
};

class HelloHandler : public Handler {
public:
    explicit HelloHandler(const HttpHeader& header) : Handler(header) {}

    int process(const butil::IOBuf& body, HttpHeader& ret_header, std::string& ret_body) override {
        ret_body = "HelloHandler";
        return 200;
    }
};

class TestSerivceImpl : public BrpcService, public TestService {

rellaf_dcl_brpc_http_service(TestSerivceImpl, TestRequest, TestResponse);

rellaf_def_brpc_http_api(echo, "/", EchoHandler);
rellaf_def_brpc_http_api(hello, "/hello", HelloHandler);

};

rellaf_def_brpc_http_service(TestSerivceImpl);

#define http_test_st_body_item(api, expect_status, expect_body, ig_body) do {       \
HttpResponse response;                                                              \
http_post("127.0.0.1:8123/" api, {}, {}, response);                                 \
ASSERT_EQ(response.status, expect_status);                                          \
if (!ig_body) {                                                                     \
    const char* str = response.body.to_string().c_str();                            \
    ASSERT_STREQ(str, expect_body);                                                 \
}} while(0)

TEST_F(TestBrpcService, echo_service) {
    http_test_st_body_item("/", 404, "", true);
    http_test_st_body_item("", 200, "EchoHandler", false);

    http_test_st_body_item("bbba", 404, "", true);
    http_test_st_body_item("bbb/", 404, "", true);
    http_test_st_body_item("hello/", 404, "", false);
    http_test_st_body_item("/hello", 404, "", false);
    http_test_st_body_item("hello", 200, "HelloHandler", false);

}

}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}