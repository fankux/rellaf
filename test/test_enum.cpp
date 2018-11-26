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
#include "enum.h"
#include "test_common.h"

namespace rellaf {
namespace test {

class TestEnum : public testing::Test {
protected:
    TestEnum() = default;

    ~TestEnum() override = default;

    void SetUp() override {}
};

class TestType : public IEnum {
rellaf_enum_dcl(TestType);

rellaf_enum_item_def(0, INT);
rellaf_enum_item_def(1, UINT);
rellaf_enum_item_def(2, FLOAT);
};

rellaf_enum_def(TestType);

TEST_F(TestEnum, test_enum) {
    ASSERT_TRUE(map_keys_equal_set(rellaf_enum(TestType).names(), {"INT", "UINT", "FLOAT"}));
    ASSERT_TRUE(map_keys_equal_set(rellaf_enum(TestType).codes(), {0, 1, 2}));

#if __cplusplus >= 201703L
    ASSERT_EQ(TestType::INT_code, 0);
    ASSERT_EQ(TestType::UINT_code, 1);
    ASSERT_EQ(TestType::FLOAT_code, 2);
#endif

    ASSERT_EQ(rellaf_enum(TestType).INT.code, 0);
    ASSERT_EQ(rellaf_enum(TestType).UINT.code, 1);
    ASSERT_EQ(rellaf_enum(TestType).FLOAT.code, 2);

    ASSERT_STREQ(rellaf_enum(TestType).INT.name.c_str(), "INT");
    ASSERT_STREQ(rellaf_enum(TestType).UINT.name.c_str(), "UINT");
    ASSERT_STREQ(rellaf_enum(TestType).FLOAT.name.c_str(), "FLOAT");

    ASSERT_TRUE(TestType::e().exist(0));
    ASSERT_TRUE(TestType::e().exist(1));
    ASSERT_TRUE(TestType::e().exist(2));
    ASSERT_FALSE(TestType::e().exist(3));

    ASSERT_TRUE(TestType::e().exist("INT"));
    ASSERT_TRUE(TestType::e().exist("UINT"));
    ASSERT_TRUE(TestType::e().exist("FLOAT"));
    ASSERT_FALSE(TestType::e().exist("vvv"));

    ASSERT_TRUE(TestType::e().exist("INT"));
    ASSERT_TRUE(TestType::e().exist("UINT"));
    ASSERT_TRUE(TestType::e().exist("FLOAT"));
    ASSERT_FALSE(TestType::e().exist("vvv"));

    ASSERT_EQ(TestType::e().get_by_name("INT"), TestType::e().INT);
    ASSERT_EQ(TestType::e().get_by_name("UINT"), TestType::e().UINT);
    ASSERT_EQ(TestType::e().get_by_name("FLOAT"), TestType::e().FLOAT);
    ASSERT_FALSE(TestType::e().get_by_name("vvv").available());

    ASSERT_EQ(TestType::e().get_by_code(0), TestType::e().INT);
    ASSERT_EQ(TestType::e().get_by_code(1), TestType::e().UINT);
    ASSERT_EQ(TestType::e().get_by_code(2), TestType::e().FLOAT);
    ASSERT_FALSE(TestType::e().get_by_code(4).available());

    ASSERT_EQ(TestType::e().get("INT"), TestType::e().INT);
    ASSERT_EQ(TestType::e().get("UINT"), TestType::e().UINT);
    ASSERT_EQ(TestType::e().get("FLOAT"), TestType::e().FLOAT);
    ASSERT_FALSE(TestType::e().get("vvv").available());

    ASSERT_EQ(TestType::e().get(0), TestType::e().INT);
    ASSERT_EQ(TestType::e().get(1), TestType::e().UINT);
    ASSERT_EQ(TestType::e().get(2), TestType::e().FLOAT);
    ASSERT_FALSE(TestType::e().get(4).available());

}

}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}