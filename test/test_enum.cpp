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

namespace rellaf {
namespace test {

class TestEnum : public testing::Test {
protected:
    TestEnum() = default;

    ~TestEnum() override = default;

    void SetUp() override {}
};

class TestType : IEnum {
RELLAF_ENUM_DCL(TestType);

RELLAF_ENUM_ITEM_DEF(0, INT);
RELLAF_ENUM_ITEM_DEF(1, UINT);
RELLAF_ENUM_ITEM_DEF(2, FLOAT);
};

RELLAF_ENUM_DEF(TestType);

template<class K, class V>
static bool map_keys_equal_set(const std::map<K, V>& map, const std::set<K>& set) {
    if (map.size() != set.size()) {
        return false;
    }
    for (auto& entry : map) {
        if (set.count(entry.first) == 0) {
            return false;
        }
    }
    return true;
}

TEST_F(TestEnum, test_enum) {
    ASSERT_TRUE(map_keys_equal_set(TestType::e().names(), {"INT", "UINT", "FLOAT"}));
    ASSERT_TRUE(map_keys_equal_set(TestType::e().codes(), {0, 1, 2}));

    ASSERT_EQ(TestType::e().INT.code, 0);
    ASSERT_EQ(TestType::e().UINT.code, 1);
    ASSERT_EQ(TestType::e().FLOAT.code, 2);

    ASSERT_STREQ(TestType::e().INT.name.c_str(), "INT");
    ASSERT_STREQ(TestType::e().UINT.name.c_str(), "UINT");
    ASSERT_STREQ(TestType::e().FLOAT.name.c_str(), "FLOAT");

//    ASSERT_TRUE(TestType::e().exist(0));
//    ASSERT_TRUE(TestType::e().exist(1));
//    ASSERT_TRUE(TestType::e().exist(2));

}

}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}