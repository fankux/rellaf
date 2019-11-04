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
#include "sql_builder.h"

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
    PatternErr err;                                         \
    std::deque<SqlPattern::Stub> pieces;                    \
    SqlPattern::explode(pattern, pieces, err);              \
    ASSERT_EQ(err, expect);                                 \
    if (err == PatternErr::OK) {                            \
        print_pieces(pieces);                               \
    }                                                       \
}

TEST_F(TestSqlPattern, test_pattern) {
    test_explode_pattern_item("#", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("##", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("###", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("{", PatternErr::ILL_TOKEN);
    test_explode_pattern_item("[", PatternErr::ILL_TOKEN);
    test_explode_pattern_item("}", PatternErr::ILL_TOKEN);
    test_explode_pattern_item("]", PatternErr::ILL_TOKEN);
    test_explode_pattern_item("#{", PatternErr::ILL_END);
    test_explode_pattern_item("#}", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("#[", PatternErr::ILL_END);
    test_explode_pattern_item("#]", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("#{{", PatternErr::ILL_END);
    test_explode_pattern_item("#[[", PatternErr::ILL_END);
    test_explode_pattern_item("#}}", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("#]]", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("#{[", PatternErr::ILL_END);
    test_explode_pattern_item("#[{", PatternErr::ILL_END);
    test_explode_pattern_item("#}]", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("#]}", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("#[}", PatternErr::UNMATCH);
    test_explode_pattern_item("#{]", PatternErr::UNMATCH);
    test_explode_pattern_item("#{}", PatternErr::NONE_FIELD);
    test_explode_pattern_item("#[]", PatternErr::NONE_FIELD);

    test_explode_pattern_item("", PatternErr::OK);
    test_explode_pattern_item("a", PatternErr::OK);
    test_explode_pattern_item("aaa", PatternErr::OK);
    test_explode_pattern_item("a#", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("a#b", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("a{", PatternErr::ILL_TOKEN);
    test_explode_pattern_item("a{b", PatternErr::ILL_TOKEN);
    test_explode_pattern_item("a##", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("a##b", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("a#{", PatternErr::ILL_END);
    test_explode_pattern_item("a#{b", PatternErr::ILL_END);
    test_explode_pattern_item("a#[", PatternErr::ILL_END);
    test_explode_pattern_item("a#[b", PatternErr::ILL_END);
    test_explode_pattern_item("a#}", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("a#}b", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("a#]", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("a#]b", PatternErr::ILL_BEGIN);
    test_explode_pattern_item("a#{{", PatternErr::ILL_END);
    test_explode_pattern_item("a#{{b", PatternErr::ILL_END);
    test_explode_pattern_item("a#{[", PatternErr::ILL_END);
    test_explode_pattern_item("a#{[b", PatternErr::ILL_END);
    test_explode_pattern_item("a#[[", PatternErr::ILL_END);
    test_explode_pattern_item("a#[[b", PatternErr::ILL_END);
    test_explode_pattern_item("a#[{", PatternErr::ILL_END);
    test_explode_pattern_item("a#[{b", PatternErr::ILL_END);
    test_explode_pattern_item("a#{]", PatternErr::UNMATCH);
    test_explode_pattern_item("a#{]b", PatternErr::UNMATCH);
    test_explode_pattern_item("a#[}", PatternErr::UNMATCH);
    test_explode_pattern_item("a#[}b", PatternErr::UNMATCH);

    test_explode_pattern_item("a#{}", PatternErr::NONE_FIELD);
    test_explode_pattern_item("a#{}b", PatternErr::NONE_FIELD);
    test_explode_pattern_item("a#[]", PatternErr::NONE_FIELD);
    test_explode_pattern_item("a#[]b", PatternErr::NONE_FIELD);

    test_explode_pattern_item("#{c}", PatternErr::OK);
    test_explode_pattern_item("a#{c}", PatternErr::OK);
    test_explode_pattern_item("#{c}b", PatternErr::OK);
    test_explode_pattern_item("a#{c}b", PatternErr::OK);
    test_explode_pattern_item("#[c]", PatternErr::OK);
    test_explode_pattern_item("a#[c]", PatternErr::OK);
    test_explode_pattern_item("#[c]b", PatternErr::OK);
    test_explode_pattern_item("a#[c]b", PatternErr::OK);

    test_explode_pattern_item("#{c1}#{c2}", PatternErr::OK);
    test_explode_pattern_item("#{c1}#{c2}#{c3}", PatternErr::OK);
    test_explode_pattern_item("#[c1]#[c2]", PatternErr::OK);
    test_explode_pattern_item("#[c1]#[c2]#[c3]", PatternErr::OK);

    test_explode_pattern_item("a#{c1}#{c2}", PatternErr::OK);
    test_explode_pattern_item("#{c1}a#{c2}", PatternErr::OK);
    test_explode_pattern_item("#{c1}#{c2}a", PatternErr::OK);
    test_explode_pattern_item("a#{c1}b#{c2}", PatternErr::OK);
    test_explode_pattern_item("#{c1}a#{c2}b", PatternErr::OK);
    test_explode_pattern_item("a#{c1}#{c2}b", PatternErr::OK);
    test_explode_pattern_item("a#{c1}b#{c2}d", PatternErr::OK);

    test_explode_pattern_item("a#[c1]#[c2]", PatternErr::OK);
    test_explode_pattern_item("#[c1]a#[c2]", PatternErr::OK);
    test_explode_pattern_item("#[c1]#[c2]a", PatternErr::OK);
    test_explode_pattern_item("a#[c1]b#[c2]", PatternErr::OK);
    test_explode_pattern_item("#[c1]a#[c2]b", PatternErr::OK);
    test_explode_pattern_item("a#[c1]#[c2]b", PatternErr::OK);
    test_explode_pattern_item("a#[c1]b#[c2]d", PatternErr::OK);

    test_explode_pattern_item("#{c1}#[c2]", PatternErr::OK);
    test_explode_pattern_item("a#{c1}#[c2]", PatternErr::OK);
    test_explode_pattern_item("#{c1}a#[c2]", PatternErr::OK);
    test_explode_pattern_item("#{c1}#[c2]a", PatternErr::OK);
    test_explode_pattern_item("a#{c1}b#[c2]", PatternErr::OK);
    test_explode_pattern_item("#{c1}a#[c2]b", PatternErr::OK);
    test_explode_pattern_item("a#{c1}#[c2]b", PatternErr::OK);
    test_explode_pattern_item("a#{c1}b#[c2]d", PatternErr::OK);

    test_explode_pattern_item("#[c1]#{c2}", PatternErr::OK);
    test_explode_pattern_item("a#[c1]#{c2}", PatternErr::OK);
    test_explode_pattern_item("#[c1]a#{c2}", PatternErr::OK);
    test_explode_pattern_item("#[c1]#{c2}a", PatternErr::OK);
    test_explode_pattern_item("a#[c1]b#{c2}", PatternErr::OK);
    test_explode_pattern_item("#[c1]a#{c2}b", PatternErr::OK);
    test_explode_pattern_item("a#[c1]#{c2}b", PatternErr::OK);
    test_explode_pattern_item("a#[c1]b#{c2}d", PatternErr::OK);
}

class Arg : public Object {
rellaf_model_dcl(Arg);

rellaf_model_def_str(cond, "str' cond");
rellaf_model_def_list(ids, Plain<int>);

};

rellaf_model_def(Arg);

class Ret : public Object {
rellaf_model_dcl(Ret);

rellaf_model_def_str(a, "");
rellaf_model_def_int(b, 0);
rellaf_model_def_float(c, 0);
};

rellaf_model_def(Ret);

class TestBuilder : public SqlBuilder {
rellaf_singleton(TestBuilder);

rellaf_sql_select(select, "SELECT a, b, c FROM table WHERE cond=#{cond}", Ret);

rellaf_sql_select(select_single, "SELECT a, b, c FROM table WHERE cond=#{a.cond}", Ret);

rellaf_sql_select(select_multi,
        "SELECT a, b, c FROM table WHERE cond=#{a.cond} AND id IN (#[b.ids])", Ret);

rellaf_sql_select_list(select_list,
        "SELECT a, b, c FROM table WHERE cond=#{a.cond} AND id IN (#[b.ids])", Ret);

rellaf_sql_select_list(select_list_base_type,
        "SELECT a FROM table WHERE cond=#{a.cond} AND id IN (#[b.ids])", int);

rellaf_sql_select_list(select_list_str,
        "SELECT a FROM table WHERE cond=#{a.cond} AND id IN (#[b.ids])", std::string);

rellaf_sql_insert(insert,
        "INSERT table(a, b, c) VALUES (#{a}, #{b}, #{c})");

rellaf_sql_update(update,
        "UPDTE table SET a=#{a}, b=#{b}, c=#{c} WHERE 1=1");

rellaf_sql_delete(del,
        "DELETE FROM table WHERE a=#{a} AND b=#{b} AND c=#{c}");

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
    TestBuilder::instance().test_split_sections(section_str, sections);
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
    ASSERT_TRUE(test_split_section_item("aa.bb", {"aa", "bb"}));
    ASSERT_TRUE(test_split_section_item("a.b.c", {"a", "b", "c"}));
    ASSERT_TRUE(test_split_section_item("aa.bb.cc", {"aa", "bb", "cc"}));

}

TEST_F(TestSqlPattern, test_sql_mapper) {
    Ret ret;
    Arg arg;
    Arg argb;
    Plain<int> id = 1;
    arg.ids().push_back(id);
    argb.ids().push_back(id);
    id = 2;
    arg.ids().push_back(id);
    argb.ids().push_back(id);

    Plain<int> idb = 2;
    Plain<int> idc = 2;

    std::string sql;

    TestBuilder& bd = TestBuilder::instance();

    ASSERT_GE(bd.select(ret, arg), 0);
    ASSERT_GE(bd.select_sql(sql, arg), 0);
    ASSERT_STREQ(sql.c_str(), R"(SELECT a, b, c FROM table WHERE cond='str\' cond')");

    ASSERT_GE(bd.select(ret, id), 0);
    ASSERT_GE(bd.select_sql(sql, id), 0);
    ASSERT_STREQ(sql.c_str(), R"(SELECT a, b, c FROM table WHERE cond=2)");

    ASSERT_EQ(bd.select_single(ret, arg), -1);
    ASSERT_EQ(bd.select_single_sql(sql, arg), -1);

    ASSERT_GE(bd.select_multi(ret, arg.tag("a"), argb.tag("b")), 0);
    ASSERT_GE(bd.select_multi_sql(sql, arg.tag("a"), argb.tag("b")), 0);
    ASSERT_STREQ(sql.c_str(),
            R"(SELECT a, b, c FROM table WHERE cond='str\' cond' AND id IN ('1','2'))");

    ASSERT_GE(bd.select_multi(ret, arg.tag("a"), idb.tag("b")), -1);
    ASSERT_GE(bd.select_multi_sql(sql, arg.tag("a"), idb.tag("b")), -1);

    ASSERT_GE(bd.insert(id.tag("a"), idb.tag("b"), idc.tag("c")), 0);
    ASSERT_GE(bd.insert_sql(sql, id.tag("a"), idb.tag("b"), idc.tag("c")), 0);
    ASSERT_STREQ(sql.c_str(), R"(INSERT table(a, b, c) VALUES (2, 2, 2))");

    ASSERT_GE(bd.update(id.tag("a"), idb.tag("b"), idc.tag("c")), 0);
    ASSERT_GE(bd.update_sql(sql, id.tag("a"), idb.tag("b"), idc.tag("c")), 0);
    ASSERT_STREQ(sql.c_str(), R"(UPDTE table SET a=2, b=2, c=2 WHERE 1=1)");

    ASSERT_GE(bd.del(id.tag("a"), idb.tag("b"), idc.tag("c")), 0);
    ASSERT_GE(bd.del_sql(sql, id.tag("a"), idb.tag("b"), idc.tag("c")), 0);
    ASSERT_STREQ(sql.c_str(), R"(DELETE FROM table WHERE a=2 AND b=2 AND c=2)");

    std::vector<Ret> results;
    ASSERT_GE(bd.select_list(results, arg.tag("a"), argb.tag("b")), 0);
    ASSERT_GE(bd.select_list_sql(sql, arg.tag("a"), argb.tag("b")), 0);
    ASSERT_STREQ(sql.c_str(),
            R"(SELECT a, b, c FROM table WHERE cond='str\' cond' AND id IN ('1','2'))");

    std::deque<Ret> dq_results;
    ASSERT_GE(bd.select_list(dq_results, arg.tag("a"), argb.tag("b")), 0);

    std::vector<int> base_type_results;
    ASSERT_GE(bd.select_list_base_type(base_type_results, arg.tag("a"), argb.tag("b")), 0);
    ASSERT_GE(bd.select_list_base_type_sql(sql, arg.tag("a"), argb.tag("b")), 0);
    ASSERT_STREQ(sql.c_str(),
            R"(SELECT a FROM table WHERE cond='str\' cond' AND id IN ('1','2'))");

    std::vector<std::string> str_results;
    ASSERT_GE(bd.select_list_str(str_results, arg.tag("a"), argb.tag("b")), 0);
    ASSERT_GE(bd.select_list_str_sql(sql, arg.tag("a"), argb.tag("b")), 0);
    ASSERT_STREQ(sql.c_str(),
            R"(SELECT a FROM table WHERE cond='str\' cond' AND id IN ('1','2'))");
}

}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}