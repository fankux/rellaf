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

#define test_url_pattern_item(pattern, expect)                      \
{                                                                   \
    PatternErr err;                                                 \
    UrlPattern::explode_path_vars(pattern, pieces, err);            \
    ASSERT_EQ(expect, err);                                         \
}

#define test_url_fetch_item(pattern, path)                          \
    UrlPattern::explode_path_vars(pattern, pieces, err);            \
    ASSERT_TRUE(UrlPattern::fetch_path_vars(path, pieces, vals))

class TestUrl : public testing::Test {
protected:
    TestUrl() = default;

    ~TestUrl() override = default;;

    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(TestUrl, test_explode_path_vars) {
    std::map<uint32_t, std::string> pieces;
    test_url_pattern_item("", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item(" ", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("  ", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("xx", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("x/", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("x/x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("x//", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/x/", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("x/x/", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/x/x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("x//x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("x/x/x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("x///", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/x//", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//x/", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("x/x//", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/x/x/", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//x/x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("x/x/x/", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/x/x/x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("x/x/x/x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("{", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("}", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("{{", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("}}", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("}{", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("{}", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("/}", PatternErr::ILL_BEGIN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{/", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{}", PatternErr::NONE_FIELD);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("//}", PatternErr::ILL_BEGIN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//{{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//{}", PatternErr::NONE_FIELD);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("///}", PatternErr::ILL_BEGIN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///{{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///{}", PatternErr::NONE_FIELD);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("/{a}{", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}}", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/x{a}", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}x", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}x/", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}/{{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}/{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}/{/", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}/{}", PatternErr::NONE_FIELD);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("//{a}{", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//{a}}", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//x{a}", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//{a}x", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//{a}x//", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//{a}//{{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//{a}//{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//{a}//{/", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("//{a}//{}", PatternErr::NONE_FIELD);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("///{a}{", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///{a}}", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///x{a}", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///{a}x", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///{a}x///", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///{a}///{{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///{a}///{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///{a}///{/", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("///{a}///{}", PatternErr::NONE_FIELD);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("/{a}/{b}{", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}/{b}}", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/x{a}", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}/{b}x", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}/{b}x/", PatternErr::ILL_TOKEN);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}/{b}/{{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}/{b}/{", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}/{b}/{/", PatternErr::ILL_END);
    ASSERT_TRUE(map_equal(pieces, {}));
    test_url_pattern_item("/{a}/{b}/{}", PatternErr::NONE_FIELD);
    ASSERT_TRUE(map_equal(pieces, {}));

    test_url_pattern_item("/{a}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}}));
    test_url_pattern_item("//{a}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}}));
    test_url_pattern_item("///{a}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}}));
    test_url_pattern_item("/{a}/", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}}));
    test_url_pattern_item("//{a}//", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}}));
    test_url_pattern_item("///{a}///", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}}));

    test_url_pattern_item("/{a}/x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}}));
    test_url_pattern_item("//{a}//x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}}));
    test_url_pattern_item("///{a}///x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}}));

    test_url_pattern_item("x/{a}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}}));
    test_url_pattern_item("/x/{a}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}}));
    test_url_pattern_item("x/x/{a}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{2, "a"}}));
    test_url_pattern_item("x/{a}/x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}}));

    test_url_pattern_item("x//{a}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}}));
    test_url_pattern_item("//x//{a}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}}));
    test_url_pattern_item("x//x//{a}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{2, "a"}}));
    test_url_pattern_item("x//{a}//x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}}));

    test_url_pattern_item("x///{a}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}}));
    test_url_pattern_item("///x///{a}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}}));
    test_url_pattern_item("x///x///{a}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{2, "a"}}));
    test_url_pattern_item("x///{a}///x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}}));

    test_url_pattern_item("//{a}//x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}}));
    test_url_pattern_item("///{a}///x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}}));

    test_url_pattern_item("/{a}/{b}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}, {1, "b"}}));
    test_url_pattern_item("//{a}//{b}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}, {1, "b"}}));
    test_url_pattern_item("///{a}///{b}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}, {1, "b"}}));
    test_url_pattern_item("/{a}/{b}/", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}, {1, "b"}}));
    test_url_pattern_item("//{a}//{b}//", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}, {1, "b"}}));
    test_url_pattern_item("///{a}///{b}///", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}, {1, "b"}}));

    test_url_pattern_item("x/{a}/{b}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}, {2, "b"}}));
    test_url_pattern_item("/x/{a}/{b}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}, {2, "b"}}));
    test_url_pattern_item("/{a}/x/{b}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}, {2, "b"}}));
    test_url_pattern_item("/{a}/{b}/x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}, {1, "b"}}));
    test_url_pattern_item("/{a}/{b}/x/", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}, {1, "b"}}));
    test_url_pattern_item("x/{a}/x/{b}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}, {3, "b"}}));
    test_url_pattern_item("/x/{a}/x/{b}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{1, "a"}, {3, "b"}}));
    test_url_pattern_item("/{a}/x/{b}/x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}, {2, "b"}}));
    test_url_pattern_item("/{a}/x/{b}/x", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}, {2, "b"}}));
    test_url_pattern_item("/{a}/x/x/{b}", PatternErr::OK);
    ASSERT_TRUE(map_equal(pieces, {{0, "a"}, {3, "b"}}));
}

TEST_F(TestUrl, test_fetch_path_vars) {
    PatternErr err;
    std::map<uint32_t, std::string> pieces;
    std::map<std::string, std::string> vals;
    test_url_fetch_item("", "");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item(" ", " ");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("  ", "  ");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("x", "x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("xx", "xx");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/", "/");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//", "//");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///", "///");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("x/", "x/");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/x", "/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("x/x", "x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("x//", "x//");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/x/", "/x/");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//x", "//x");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("x/x/", "x/x/");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/x/x", "/x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("x//x", "x//x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("x/x/x", "x/x/x");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("x///", "x///");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/x//", "/x//");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//x/", "//x/");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///x", "///x");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("x/x//", "x/x//");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/x/x/", "/x/x/");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//x/x", "//x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("x/x/x/", "x/x/x/");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/x/x/x", "/x/x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("x/x/x/x", "x/x/x/x");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("{", "{");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("}", "}");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("{{", "{{");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("}}", "}}");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("}{", "}{");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("{}", "{}");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("/}", "/}");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{", "/{");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{{", "/{{");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{/", "/{/");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{}", "/{}");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("//}", "//}");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//{", "//{");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//{{", "//{{");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//{}", "//{}");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("///}", "///}");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///{", "///{");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///{{", "///{{");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///{}", "///{}");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("/{a}{", "/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}}", "/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/x{a}", "/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}x", "/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}x/", "/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/{{", "/x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/{", "/x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/{/", "/x/x/");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/{}", "/x/x");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("//{a}{", "//x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//{a}}", "//x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//x{a}", "//x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//{a}x", "//x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//{a}x//", "//x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//{a}//{{", "//x//x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//{a}//{", "//x//x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//{a}//{//", "//x//x//");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("//{a}//{}", "//x//x");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("///{a}{", "///x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///{a}}", "///x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///x{a}", "///x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///{a}x", "///x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///{a}x///", "///x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///{a}///{{", "///x///x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///{a}///{", "///x///x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///{a}///{///", "///x///x///");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("///{a}///{}", "///x///x");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("/{a}/{b}{", "/x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/{b}}", "/x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/x{a}", "/x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/{b}x", "/x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/{b}x/", "/x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/{b}/{{", "/x/x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/{b}/{", "/x/x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/{b}/{/", "/x/x/x");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/{b}/{}", "/x/x/x");
    ASSERT_TRUE(map_equal(vals, {}));

    test_url_fetch_item("/{a}", "/");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}", "//");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/", "/");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/", "//");
    ASSERT_TRUE(map_equal(vals, {}));
    test_url_fetch_item("/{a}/{b}", "/x");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}}));
    test_url_fetch_item("/{a}/{b}", "//x");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}}));

    test_url_fetch_item("/{a}", "/x");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}}));
    test_url_fetch_item("//{a}", "//x");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}}));
    test_url_fetch_item("///{a}", "///x");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}}));
    test_url_fetch_item("/{a}/", "/x/");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}}));
    test_url_fetch_item("//{a}//", "//x//");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}}));
    test_url_fetch_item("///{a}///", "///x///x");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}}));

    test_url_fetch_item("/{a}/x", "/y/x");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));
    test_url_fetch_item("//{a}//x", "//y//x");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));
    test_url_fetch_item("///{a}///x", "///y///x");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));

    test_url_fetch_item("x/{a}", "x/y");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));
    test_url_fetch_item("/x/{a}", "/x/y");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));
    test_url_fetch_item("x/x/{a}", "x/x/y");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));
    test_url_fetch_item("x/{a}/x", "x/y/x");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));

    test_url_fetch_item("x//{a}", "x//y");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));
    test_url_fetch_item("//x//{a}", "//x//y");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));
    test_url_fetch_item("x//x//{a}", "x//x//y");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));
    test_url_fetch_item("x//{a}//x", "x//y//x");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));

    test_url_fetch_item("x///{a}", "x///yy");
    ASSERT_TRUE(map_equal(vals, {{"a", "yy"}}));
    test_url_fetch_item("///x///{a}", "///x///yy");
    ASSERT_TRUE(map_equal(vals, {{"a", "yy"}}));
    test_url_fetch_item("x///x///{a}", "x///x///yyy");
    ASSERT_TRUE(map_equal(vals, {{"a", "yyy"}}));
    test_url_fetch_item("x///{a}///x", "x///yy///x");
    ASSERT_TRUE(map_equal(vals, {{"a", "yy"}}));

    test_url_fetch_item("//{a}//x", "//y//x");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));
    test_url_fetch_item("///{a}///x", "///y///x");
    ASSERT_TRUE(map_equal(vals, {{"a", "y"}}));

    test_url_fetch_item("/{a}/{b}", "/x/y");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}, {"b", "y"}}));
    test_url_fetch_item("//{a}//{b}", "//x//y");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}, {"b", "y"}}));
    test_url_fetch_item("///{a}///{b}", "///x///y");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}, {"b", "y"}}));
    test_url_fetch_item("/{a}/{b}/", "/x/y/");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}, {"b", "y"}}));
    test_url_fetch_item("//{a}//{b}//", "//x//y//");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}, {"b", "y"}}));
    test_url_fetch_item("///{a}///{b}///", "///x///y///");
    ASSERT_TRUE(map_equal(vals, {{"a", "x"}, {"b", "y"}}));

    test_url_fetch_item("x/{a}/{b}", "x/mmm/nnn");
    ASSERT_TRUE(map_equal(vals, {{"a", "mmm"}, {"b", "nnn"}}));
    test_url_fetch_item("/x/{a}/{b}", "/x/mmm/nnn");
    ASSERT_TRUE(map_equal(vals, {{"a", "mmm"}, {"b", "nnn"}}));
    test_url_fetch_item("/{a}/x/{b}", "/mmm/x/nnn");
    ASSERT_TRUE(map_equal(vals, {{"a", "mmm"}, {"b", "nnn"}}));
    test_url_fetch_item("/{a}/{b}/x", "/mmm/nnn/x");
    ASSERT_TRUE(map_equal(vals, {{"a", "mmm"}, {"b", "nnn"}}));
    test_url_fetch_item("/{a}/{b}/x/", "/mmm/nnn/x");
    ASSERT_TRUE(map_equal(vals, {{"a", "mmm"}, {"b", "nnn"}}));
    test_url_fetch_item("x/{a}/x/{b}", "x/mmm/x/nnn");
    ASSERT_TRUE(map_equal(vals, {{"a", "mmm"}, {"b", "nnn"}}));
    test_url_fetch_item("/x/{a}/x/{b}", "/x/mmm/x/nnn");
    ASSERT_TRUE(map_equal(vals, {{"a", "mmm"}, {"b", "nnn"}}));
    test_url_fetch_item("/{a}/x/{b}/x", "/mmm/x/nnn/x");
    ASSERT_TRUE(map_equal(vals, {{"a", "mmm"}, {"b", "nnn"}}));
    test_url_fetch_item("/x/{a}/{b}/x", "/x/mmm/nnn/x");
    ASSERT_TRUE(map_equal(vals, {{"a", "mmm"}, {"b", "nnn"}}));
    test_url_fetch_item("/{a}/x/x/{b}", "/mmm/x/x/nnn");
    ASSERT_TRUE(map_equal(vals, {{"a", "mmm"}, {"b", "nnn"}}));
}

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

class Params : public Object {
rellaf_model_dcl(Params);
};

rellaf_model_def(Params);

class Vars : public Object {
rellaf_model_dcl(Vars);
};

rellaf_model_def(Vars);


class TestSerivceImpl : public BrpcService, public TestService {

rellaf_brpc_http_dcl(TestSerivceImpl, TestRequest, TestResponse);

rellaf_brpc_http_def_post(hi1, "/hi1", hi1, HelloRet, Params, Vars, HelloRequest);

rellaf_brpc_http_def_post_body(hi2, "/hi2", hi2, HelloRet, HelloRequest);

rellaf_brpc_http_def_post_param(hi3, "/hi3", hi3, HelloRet, HelloRequest);

rellaf_brpc_http_def_post_pathvar(hi4, "/hi4", hi4, HelloRet, HelloRequest);

rellaf_brpc_http_def_post_param_body(hi5, "/hi5", hi5, HelloRet, HelloRequest, HelloRequest) {
        HelloRet ret;
        return ret;
    }

rellaf_brpc_http_def_post_pathvar_body(hi6, "/hi6", hi6, HelloRet, HelloRequest, HelloRequest) {
        HelloRet ret;
        return ret;
    }

rellaf_brpc_http_def_post_param_pathvar(hi7, "/hi7", hi7, HelloRet, HelloRequest, HelloRequest) {
        HelloRet ret;
        return ret;
    }

rellaf_brpc_http_def_get(hi8, "/hi8", hi8, HelloRet, HelloRequest, HelloRequest) {
        HelloRet ret;
        return ret;
    }

rellaf_brpc_http_def_get_param(hi9, "/hi9", hi9, HelloRet, HelloRequest) {
        HelloRet ret;
        return ret;
    }

rellaf_brpc_http_def_get_pathvar(hi10, "/hi10", hi10, HelloRet, HelloRequest) {
        HelloRet ret;
        return ret;
    }

};

rellaf_brpc_http_def(TestSerivceImpl);

HelloRet TestSerivceImpl::hi1(HttpContext& context, const Params& params, const Vars& vars,
        const HelloRequest& request) {
    HelloRet ret;
    return ret;
}

HelloRet TestSerivceImpl::hi2(HttpContext& context, const HelloRequest& request) {
    HelloRet ret;
    return ret;
}

HelloRet TestSerivceImpl::hi3(HttpContext& context, const HelloRequest& request) {
    HelloRet ret;
    return ret;
}

HelloRet TestSerivceImpl::hi4(HttpContext& context, const HelloRequest& request) {
    HelloRet ret;
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
    http_test_st_body_item("hello", 404, json2str(json).c_str(), false);

    http_test_st_body_item("", 200, "111", false);
}

}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}