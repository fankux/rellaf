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
#include "common.h"
#include "model.h"
#include "test_common.h"
#include "json/json_to_model.h"

namespace rellaf {
namespace test {

class TestJson : public testing::Test {
protected:
    TestJson() = default;

    ~TestJson() override = default;

    void SetUp() override {}
};

class Empty : public Object {
rellaf_model_dcl(Empty);

};

rellaf_model_def(Empty);

TEST_F(TestJson, test_empty) {
    Empty empty;
    std::string json;
    ASSERT_TRUE(model_to_json(&empty, json));
    ASSERT_STREQ(json.c_str(), "{}");
}

class Obj : public Object {
rellaf_model_dcl(Obj);

rellaf_model_def_int(id, 0);
rellaf_model_def_str(name, "");
};

rellaf_model_def(Obj);

static inline std::string json2str(const Json::Value& json, bool is_format = false) {
    Json::StreamWriterBuilder builder;
    if (!is_format) {
        builder.settings_["indentation"] = "";
    }
    return Json::writeString(builder, json);
}

TEST_F(TestJson, test_plain) {
    Obj obj;
    std::string json_str;
    ASSERT_TRUE(model_to_json(&obj, json_str));

    Json::Value json(Json::objectValue);
    json["id"] = (Json::Int)0;
    json["name"] = "";
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());
}

class WithObjectOnly : public Object {
rellaf_model_dcl(WithObjectOnly);

rellaf_model_def_object(empty, Empty);
};

rellaf_model_def(WithObjectOnly);

TEST_F(TestJson, test_object_only) {
    WithObjectOnly object_only;
    std::string json_str;

    ASSERT_TRUE(model_to_json(&object_only, json_str));
    Json::Value json(Json::objectValue);
    json["empty"] = Json::Value(Json::nullValue);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());

    Empty empty;
    object_only.set_empty(&empty);
    ASSERT_TRUE(model_to_json(&object_only, json_str));

    json.clear();
    json["empty"] = Json::Value(Json::objectValue);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());
}

class WithObjectOnly2 : public Object {
rellaf_model_dcl(WithObjectOnly2);

rellaf_model_def_object(empty, Empty);
rellaf_model_def_object(plain, Model);
};

rellaf_model_def(WithObjectOnly2);

TEST_F(TestJson, test_object_only2) {
    WithObjectOnly2 object_only;
    std::string json_str;
    ASSERT_TRUE(model_to_json(&object_only, json_str));
    Json::Value json(Json::objectValue);
    json["empty"] = Json::Value(Json::nullValue);
    json["plain"] = Json::Value(Json::nullValue);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());

    Empty empty;
    object_only.set_empty(&empty);
    ASSERT_TRUE(model_to_json(&object_only, json_str));
    json.clear();
    json["empty"] = Json::Value(Json::objectValue);
    json["plain"] = Json::Value(Json::nullValue);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());

    object_only.set_empty(nullptr);

    Obj obj;
    object_only.set_plain(&obj);
    ASSERT_TRUE(model_to_json(&object_only, json_str));
    json.clear();
    json["plain"] = Json::Value(Json::objectValue);
    json["plain"]["id"] = (Json::Int)0;
    json["plain"]["name"] = "";
    json["empty"] = Json::Value(Json::nullValue);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());

    object_only.set_empty(&empty);
    obj.set_id(1111);
    obj.set_name("1111");
    object_only.set_plain(&obj);
    ASSERT_TRUE(model_to_json(&object_only, json_str));
    json.clear();
    json["plain"] = Json::Value(Json::objectValue);
    json["plain"]["id"] = (Json::Int)1111;
    json["plain"]["name"] = "1111";
    json["empty"] = Json::Value(Json::objectValue);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());
}

class WithObjecPlain : public Object {
rellaf_model_dcl(WithObjecPlain);

rellaf_model_def_int(id, 0);
rellaf_model_def_uint64(inc, 0);
rellaf_model_def_object(object, Model);
};

rellaf_model_def(WithObjecPlain);

TEST_F(TestJson, test_object_plain) {
    WithObjecPlain object_plain;
    std::string json_str;
    ASSERT_TRUE(model_to_json(&object_plain, json_str));
    Json::Value json(Json::objectValue);
    json["id"] = (Json::Int)0;
    json["inc"] = (Json::UInt64)0;
    json["object"] = Json::Value(Json::nullValue);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());

    Obj obj;
    object_plain.set_object(&obj);
    ASSERT_TRUE(model_to_json(&object_plain, json_str));
    json["object"] = Json::Value(Json::nullValue);
    json["object"]["id"] = (Json::Int)0;
    json["object"]["name"] = "";
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());
}

class WithListOnly : public Object {
rellaf_model_dcl(WithListOnly);
rellaf_model_def_list(list, Plain<int>);
};

rellaf_model_def(WithListOnly);

TEST_F(TestJson, test_list_only) {
    WithListOnly list_only;
    std::string json_str;
    ASSERT_TRUE(model_to_json(&list_only, json_str));
    Json::Value json;
    json["list"] = Json::Value(Json::arrayValue);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());

    Plain<int> int_item(111);
    list_only.list().push_back(int_item);
    ASSERT_TRUE(model_to_json(&list_only, json_str));
    json.clear();
    json["list"].append(111);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());

    int_item.set(222);
    list_only.list().push_back((Model*)&int_item);
    ASSERT_TRUE(model_to_json(&list_only, json_str));
    json["list"].append(222);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());

    Json::Value removed;
    list_only.list().pop_front();
    ASSERT_TRUE(model_to_json(&list_only, json_str));
    json["list"].removeIndex(0, &removed);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());

    list_only.list().pop_front();
    ASSERT_TRUE(model_to_json(&list_only, json_str));
    json["list"].removeIndex(0, &removed);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());

    json.clear();
    json["list"] = Json::Value(Json::arrayValue);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());
}

class Complex : public Object {
rellaf_model_dcl(Complex);

rellaf_model_def_int(id, 0);
rellaf_model_def_object(plain, Model);
rellaf_model_def_list(list, Plain<int>);
};

rellaf_model_def(Complex);

TEST_F(TestJson, test_compex) {
    Complex complex;
    std::string json_str;
    ASSERT_TRUE(model_to_json(&complex, json_str));
    Json::Value json;
    json["id"] = 0;
    json["plain"] = Json::Value(Json::nullValue);
    json["list"] = Json::Value(Json::arrayValue);
    ASSERT_STREQ(json_str.c_str(), json2str(json).c_str());
}

} // namespace
} // namespace

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}