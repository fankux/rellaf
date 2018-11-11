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
    map_keys_equal_set<std::string, const EnumItem*>(TestType::instance().names(), {"INT", "UINT",
            "FLOAT"});
    map_keys_equal_set<int, const EnumItem*>(TestType::instance().codes(), {0, 1, 2});
}

}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}