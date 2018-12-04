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
#include "json/json.h"
#include "test_common.h"
#include "brpc_dispatcher.h"
#include "test_service.pb.h"
#include "http_client.h"

namespace rellaf {
namespace test {

static inline std::string json2str(const Json::Value& json, bool is_format = false) {
    Json::StreamWriterBuilder builder;
    if (!is_format) {
        builder.settings_["indentation"] = "";
    }
    return Json::writeString(builder, json);
}

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

class HelloRet : public Object {
rellaf_model_dcl(HelloRet);

rellaf_model_def_int(status, 200);
};

rellaf_model_def(HelloRet);

class HelloRequest : public Object {
rellaf_model_dcl(HelloRequest);

rellaf_model_def_int(id, 0);
rellaf_model_def_str(name, "");
rellaf_model_def_object(ret, HelloRet);
};

rellaf_model_def(HelloRequest);

class TestSerivceImpl : public BrpcService, public TestService {

rellaf_brpc_http_dcl(TestSerivceImpl, TestRequest, TestResponse);

rellaf_brpc_http_def_api(echo, "/", GET, echo, Plain<int>, Plain<int>);

rellaf_brpc_http_def_api_ctx(hello, "/hello", POST, hello, HelloRet, HelloRequest);

};

rellaf_brpc_http_def(TestSerivceImpl);

Plain<int> TestSerivceImpl::echo(const Plain<int>& request) {
    return Plain<int>{111};
};

HelloRet TestSerivceImpl::hello(const HelloRequest& request, HttpContext& context) {
    HelloRet ret;
    ret.set_status(233);
    return ret;
}

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

    http_test_st_body_item("bbba", 404, "", true);
    http_test_st_body_item("bbb/", 404, "", true);
    http_test_st_body_item("hello/", 404, "", false);
    http_test_st_body_item("/hello", 404, "", false);

    Json::Value json;
    json["status"] = 233;
    http_test_st_body_item("hello", 200, json2str(json).c_str(), false);

    http_test_st_body_item("", 200, "111", false);
}

}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}