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
#include "dao.h"

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

TEST_F(TestSqlPattern, test_pattern) {
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

class Arg : public Model {
rellaf_model_dcl(Arg);

rellaf_model_def_str(cond, "str' cond");
rellaf_model_def_list(ids, PlainWrap<int>);

};

rellaf_model_def(Arg);

class Ret : public Model {
rellaf_model_dcl(Ret);

rellaf_model_def_str(a, "");
rellaf_model_def_int(b, 0);
rellaf_model_def_float(c, 0);
};

rellaf_model_def(Ret);

class TestDao : public Dao {
rellaf_singleton(TestDao);

rellaf_dao_select(select, "SELECT a, b, c FROM table WHERE cond=#{cond}", Ret);

rellaf_dao_select(select_single, "SELECT a, b, c FROM table WHERE cond=#{a.cond}", Ret);

rellaf_dao_select(select_multi,
        "SELECT a, b, c FROM table WHERE cond=#{a.cond} AND id IN #[b.ids]", Ret);

rellaf_dao_select_list(select_list,
        "SELECT a, b, c FROM table WHERE cond=#{a.cond} AND id IN #[b.ids]", Ret);

public:
    void test_split_sections(const std::string& section_str, std::deque<std::string>& sections) {
        return split_section(section_str, sections);
    }
};

static bool deque_equal(const std::deque<std::string>& a, const std::deque<std::string>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

static bool test_split_section_item(const std::string& section_str,
        const std::deque<std::string>& expect) {
    std::deque<std::string> sections;
    TestDao::instance().test_split_sections(section_str, sections);
    return deque_equal(sections, expect);
}

TEST_F(TestSqlPattern, test_split_section) {
    ASSERT_TRUE(test_split_section_item("", {}));
    ASSERT_TRUE(test_split_section_item(" ", {" "}));
    ASSERT_TRUE(test_split_section_item("  ", {"  "}));
    ASSERT_TRUE(test_split_section_item(".", {}));
    ASSERT_TRUE(test_split_section_item(" .", {" "}));
    ASSERT_TRUE(test_split_section_item(". ", {" "}));
    ASSERT_TRUE(test_split_section_item("..", {}));
    ASSERT_TRUE(test_split_section_item(" ..", {" "}));
    ASSERT_TRUE(test_split_section_item(".. ", {" "}));
    ASSERT_TRUE(test_split_section_item(". .", {" "}));
    ASSERT_TRUE(test_split_section_item("...", {}));
    ASSERT_TRUE(test_split_section_item(". . .", {" ", " "}));
    ASSERT_TRUE(test_split_section_item("a.", {"a"}));
    ASSERT_TRUE(test_split_section_item(" a.", {" a",}));
    ASSERT_TRUE(test_split_section_item("a . ", {"a ", " "}));
    ASSERT_TRUE(test_split_section_item(" a . ", {" a ", " "}));
    ASSERT_TRUE(test_split_section_item(".a", {"a"}));
    ASSERT_TRUE(test_split_section_item(". a", {" a"}));
    ASSERT_TRUE(test_split_section_item(".a ", {"a "}));
    ASSERT_TRUE(test_split_section_item(". a ", {" a "}));
    ASSERT_TRUE(test_split_section_item(" .a", {" ", "a"}));
    ASSERT_TRUE(test_split_section_item("a.b", {"a", "b"}));

}

TEST_F(TestSqlPattern, test_sql_mapper) {
    Ret ret;
    Arg arg;
    PlainWrap<int> id(1);
    arg.ids().push_back(&id);
    id.set_value(2);
    arg.ids().push_back(&id);

    ASSERT_GE(TestDao::instance().select(ret, DaoModel(arg)), 0);
    ASSERT_EQ(TestDao::instance().select_single(ret, DaoModel("a", arg)), -1);
    ASSERT_GE(TestDao::instance().select_multi(ret, DaoModel("a", arg), DaoModel("b", arg)), 0);
}

}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}