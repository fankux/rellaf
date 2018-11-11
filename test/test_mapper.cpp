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
#include "common.h"
#include "sql_pattern.h"

namespace rellaf {
namespace test {
class TestSqlPattern : public testing::Test {
protected:
    TestSqlPattern() = default;

    ~TestSqlPattern() override = default;

    void SetUp() override {}
};

void print_pieces(std::deque<SqlPattern::Stub>& pieces) {
    RELLAF_DEBUG("--------------");
    for (auto& item : pieces) {
        RELLAF_DEBUG("%d:%s", item.type(), item.value().c_str());
    }
    RELLAF_DEBUG("^^^^^^^^^^^^^^");
}

#define test_explode_pattern_item(pattern, expect)          \
{                                                           \
    SqlPattern::PatternErr err;                             \
    std::deque<SqlPattern::Stub> pieces;                    \
    SqlPattern::explode(pattern, pieces, err);              \
    ASSERT_EQ(err, expect);                                 \
    if (err == SqlPattern::OK) {                            \
        print_pieces(pieces);                               \
    }                                                       \
}

TEST_F(TestSqlPattern, process_deploy_finish) {
    test_explode_pattern_item("#", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("##", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("###", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("{", SqlPattern::ILL_TOKEN);
    test_explode_pattern_item("[", SqlPattern::ILL_TOKEN);
    test_explode_pattern_item("}", SqlPattern::ILL_TOKEN);
    test_explode_pattern_item("]", SqlPattern::ILL_TOKEN);
    test_explode_pattern_item("#{", SqlPattern::ILL_END);
    test_explode_pattern_item("#}", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("#[", SqlPattern::ILL_END);
    test_explode_pattern_item("#]", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("#{{", SqlPattern::ILL_END);
    test_explode_pattern_item("#[[", SqlPattern::ILL_END);
    test_explode_pattern_item("#}}", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("#]]", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("#{[", SqlPattern::ILL_END);
    test_explode_pattern_item("#[{", SqlPattern::ILL_END);
    test_explode_pattern_item("#}]", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("#]}", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("#[}", SqlPattern::UNMATCH);
    test_explode_pattern_item("#{]", SqlPattern::UNMATCH);
    test_explode_pattern_item("#{}", SqlPattern::NONE_FIELD);
    test_explode_pattern_item("#[]", SqlPattern::NONE_FIELD);

    test_explode_pattern_item("", SqlPattern::OK);
    test_explode_pattern_item("a", SqlPattern::OK);
    test_explode_pattern_item("aaa", SqlPattern::OK);
    test_explode_pattern_item("a#", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("a#b", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("a{", SqlPattern::ILL_TOKEN);
    test_explode_pattern_item("a{b", SqlPattern::ILL_TOKEN);
    test_explode_pattern_item("a##", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("a##b", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("a#{", SqlPattern::ILL_END);
    test_explode_pattern_item("a#{b", SqlPattern::ILL_END);
    test_explode_pattern_item("a#[", SqlPattern::ILL_END);
    test_explode_pattern_item("a#[b", SqlPattern::ILL_END);
    test_explode_pattern_item("a#}", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("a#}b", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("a#]", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("a#]b", SqlPattern::ILL_BEGIN);
    test_explode_pattern_item("a#{{", SqlPattern::ILL_END);
    test_explode_pattern_item("a#{{b", SqlPattern::ILL_END);
    test_explode_pattern_item("a#{[", SqlPattern::ILL_END);
    test_explode_pattern_item("a#{[b", SqlPattern::ILL_END);
    test_explode_pattern_item("a#[[", SqlPattern::ILL_END);
    test_explode_pattern_item("a#[[b", SqlPattern::ILL_END);
    test_explode_pattern_item("a#[{", SqlPattern::ILL_END);
    test_explode_pattern_item("a#[{b", SqlPattern::ILL_END);
    test_explode_pattern_item("a#{]", SqlPattern::UNMATCH);
    test_explode_pattern_item("a#{]b", SqlPattern::UNMATCH);
    test_explode_pattern_item("a#[}", SqlPattern::UNMATCH);
    test_explode_pattern_item("a#[}b", SqlPattern::UNMATCH);

    test_explode_pattern_item("a#{}", SqlPattern::NONE_FIELD);
    test_explode_pattern_item("a#{}b", SqlPattern::NONE_FIELD);
    test_explode_pattern_item("a#[]", SqlPattern::NONE_FIELD);
    test_explode_pattern_item("a#[]b", SqlPattern::NONE_FIELD);

    test_explode_pattern_item("#{c}", SqlPattern::OK);
    test_explode_pattern_item("a#{c}", SqlPattern::OK);
    test_explode_pattern_item("#{c}b", SqlPattern::OK);
    test_explode_pattern_item("a#{c}b", SqlPattern::OK);
    test_explode_pattern_item("#[c]", SqlPattern::OK);
    test_explode_pattern_item("a#[c]", SqlPattern::OK);
    test_explode_pattern_item("#[c]b", SqlPattern::OK);
    test_explode_pattern_item("a#[c]b", SqlPattern::OK);

    test_explode_pattern_item("#{c1}#{c2}", SqlPattern::OK);
    test_explode_pattern_item("#{c1}#{c2}#{c3}", SqlPattern::OK);
    test_explode_pattern_item("#[c1]#[c2]", SqlPattern::OK);
    test_explode_pattern_item("#[c1]#[c2]#[c3]", SqlPattern::OK);

    test_explode_pattern_item("a#{c1}#{c2}", SqlPattern::OK);
    test_explode_pattern_item("#{c1}a#{c2}", SqlPattern::OK);
    test_explode_pattern_item("#{c1}#{c2}a", SqlPattern::OK);
    test_explode_pattern_item("a#{c1}b#{c2}", SqlPattern::OK);
    test_explode_pattern_item("#{c1}a#{c2}b", SqlPattern::OK);
    test_explode_pattern_item("a#{c1}#{c2}b", SqlPattern::OK);
    test_explode_pattern_item("a#{c1}b#{c2}d", SqlPattern::OK);

    test_explode_pattern_item("a#[c1]#[c2]", SqlPattern::OK);
    test_explode_pattern_item("#[c1]a#[c2]", SqlPattern::OK);
    test_explode_pattern_item("#[c1]#[c2]a", SqlPattern::OK);
    test_explode_pattern_item("a#[c1]b#[c2]", SqlPattern::OK);
    test_explode_pattern_item("#[c1]a#[c2]b", SqlPattern::OK);
    test_explode_pattern_item("a#[c1]#[c2]b", SqlPattern::OK);
    test_explode_pattern_item("a#[c1]b#[c2]d", SqlPattern::OK);

    test_explode_pattern_item("#{c1}#[c2]", SqlPattern::OK);
    test_explode_pattern_item("a#{c1}#[c2]", SqlPattern::OK);
    test_explode_pattern_item("#{c1}a#[c2]", SqlPattern::OK);
    test_explode_pattern_item("#{c1}#[c2]a", SqlPattern::OK);
    test_explode_pattern_item("a#{c1}b#[c2]", SqlPattern::OK);
    test_explode_pattern_item("#{c1}a#[c2]b", SqlPattern::OK);
    test_explode_pattern_item("a#{c1}#[c2]b", SqlPattern::OK);
    test_explode_pattern_item("a#{c1}b#[c2]d", SqlPattern::OK);

    test_explode_pattern_item("#[c1]#{c2}", SqlPattern::OK);
    test_explode_pattern_item("a#[c1]#{c2}", SqlPattern::OK);
    test_explode_pattern_item("#[c1]a#{c2}", SqlPattern::OK);
    test_explode_pattern_item("#[c1]#{c2}a", SqlPattern::OK);
    test_explode_pattern_item("a#[c1]b#{c2}", SqlPattern::OK);
    test_explode_pattern_item("#[c1]a#{c2}b", SqlPattern::OK);
    test_explode_pattern_item("a#[c1]#{c2}b", SqlPattern::OK);
    test_explode_pattern_item("a#[c1]b#{c2}d", SqlPattern::OK);
}

}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}