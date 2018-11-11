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
// TODO.. valgrind mem check


#include "gtest/gtest.h"
#include "common.h"
#include "model.h"
#include "sql_model_tool.h"

namespace rellaf {
namespace test {

class TestModel : public testing::Test {
protected:
    TestModel() = default;

    ~TestModel() override = default;

    void SetUp() override {}
};

class SubModel : public Model {
RELLAF_MODEL_DCL(SubModel)

RELLAF_MODEL_DEF_int(sub_model_id, 111);

};

RELLAF_MODEL_DEF(SubModel);

class List : public Model {
RELLAF_MODEL_DCL(List)

RELLAF_MODEL_DEF_int(list_id, 111);

};

RELLAF_MODEL_DEF(List);

class Object : public Model {
RELLAF_MODEL_DCL(Object)

RELLAF_MODEL_DEF_int(val_int, -111);
RELLAF_MODEL_DEF_int64(val_int64, -111);
RELLAF_MODEL_DEF_uint16(val_uint16, 111);
RELLAF_MODEL_DEF_uint32(val_uint32, 111);
RELLAF_MODEL_DEF_uint64(val_uint64, 111);
RELLAF_MODEL_DEF_bool(val_bool, false);
RELLAF_MODEL_DEF_float(val_float, 1.0001);
RELLAF_MODEL_DEF_double(val_double, 1.0001);
RELLAF_MODEL_DEF_str(val_str, "aaa");

RELLAF_MODEL_DEF_object(val_object, SubModel);

RELLAF_MODEL_DEF_list(val_list, List);

};

RELLAF_MODEL_DEF(Object);

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

TEST_F(TestModel, test_plain) {

    Object object;

    // member
    ASSERT_TRUE(Object::is_member("val_int"));
    ASSERT_TRUE(Object::is_member("val_int64"));
    ASSERT_TRUE(Object::is_member("val_uint16"));
    ASSERT_TRUE(Object::is_member("val_uint32"));
    ASSERT_TRUE(Object::is_member("val_uint64"));
    ASSERT_TRUE(Object::is_member("val_bool"));
    ASSERT_TRUE(Object::is_member("val_float"));
    ASSERT_TRUE(Object::is_member("val_double"));
    ASSERT_TRUE(Object::is_member("val_str"));
    ASSERT_FALSE(Object::is_member("not_exist"));

    ASSERT_TRUE(object.is_int_member("val_int"));
    ASSERT_TRUE(object.is_int64_member("val_int64"));
    ASSERT_TRUE(object.is_uint16_member("val_uint16"));
    ASSERT_TRUE(object.is_uint32_member("val_uint32"));
    ASSERT_TRUE(object.is_uint64_member("val_uint64"));
    ASSERT_TRUE(object.is_bool_member("val_bool"));
    ASSERT_TRUE(object.is_float_member("val_float"));
    ASSERT_TRUE(object.is_double_member("val_double"));
    ASSERT_TRUE(object.is_str_member("val_str"));
    ASSERT_FALSE(object.is_str_member("not_exist"));

    ASSERT_TRUE(Object::int_concern("val_int"));
    ASSERT_TRUE(Object::int64_concern("val_int64"));
    ASSERT_TRUE(Object::uint16_concern("val_uint16"));
    ASSERT_TRUE(Object::uint32_concern("val_uint32"));
    ASSERT_TRUE(Object::uint64_concern("val_uint64"));
    ASSERT_TRUE(Object::bool_concern("val_bool"));
    ASSERT_TRUE(Object::float_concern("val_float"));
    ASSERT_TRUE(Object::double_concern("val_double"));
    ASSERT_TRUE(Object::str_concern("val_str"));
    ASSERT_FALSE(Object::str_concern("not_exist"));

    ASSERT_TRUE(map_keys_equal_set(Object::int_names(), {"val_int"}));
    ASSERT_TRUE(map_keys_equal_set(Object::int64_names(), {"val_int64"}));
    ASSERT_TRUE(map_keys_equal_set(Object::uint16_names(), {"val_uint16"}));
    ASSERT_TRUE(map_keys_equal_set(Object::uint32_names(), {"val_uint32"}));
    ASSERT_TRUE(map_keys_equal_set(Object::uint64_names(), {"val_uint64"}));
    ASSERT_TRUE(map_keys_equal_set(Object::bool_names(), {"val_bool"}));
    ASSERT_TRUE(map_keys_equal_set(Object::float_names(), {"val_float"}));
    ASSERT_TRUE(map_keys_equal_set(Object::double_names(), {"val_double"}));
    ASSERT_TRUE(map_keys_equal_set(Object::str_names(), {"val_str"}));

    ASSERT_TRUE(map_keys_equal_set(object.ints(), {"val_int"}));
    ASSERT_TRUE(map_keys_equal_set(object.int64s(), {"val_int64"}));
    ASSERT_TRUE(map_keys_equal_set(object.uint16s(), {"val_uint16"}));
    ASSERT_TRUE(map_keys_equal_set(object.uint32s(), {"val_uint32"}));
    ASSERT_TRUE(map_keys_equal_set(object.uint64s(), {"val_uint64"}));
    ASSERT_TRUE(map_keys_equal_set(object.bools(), {"val_bool"}));
    ASSERT_TRUE(map_keys_equal_set(object.floats(), {"val_float"}));
    ASSERT_TRUE(map_keys_equal_set(object.doubles(), {"val_double"}));
    ASSERT_TRUE(map_keys_equal_set(object.strs(), {"val_str"}));


    //default
    ASSERT_EQ(object.val_int(), -111);
    ASSERT_EQ(object.val_int64(), -111);
    ASSERT_EQ(object.val_uint16(), 111);
    ASSERT_EQ(object.val_uint32(), 111);
    ASSERT_EQ(object.val_uint64(), 111);
    ASSERT_EQ(object.val_bool(), false);
    ASSERT_FLOAT_EQ(object.val_float(), 1.0001);
    ASSERT_DOUBLE_EQ(object.val_double(), 1.0001);
    ASSERT_STREQ(object.val_str().c_str(), "aaa");

    ASSERT_EQ(object.get_int("val_int"), -111);
    ASSERT_EQ(object.get_int64("val_int64"), -111);
    ASSERT_EQ(object.get_uint16("val_uint16"), 111);
    ASSERT_EQ(object.get_uint32("val_uint32"), 111);
    ASSERT_EQ(object.get_uint64("val_uint64"), 111);
    ASSERT_EQ(object.get_bool("val_bool"), false);
    ASSERT_FLOAT_EQ(object.get_float("val_float"), 1.0001);
    ASSERT_DOUBLE_EQ(object.get_double("val_double"), 1.0001);
    ASSERT_STREQ(object.get_str("val_str").c_str(), "aaa");

    ASSERT_TRUE(Object::is_default("val_int", "-111"));
    ASSERT_TRUE(Object::is_default("val_int64", "-111"));
    ASSERT_TRUE(Object::is_default("val_uint16", "111"));
    ASSERT_TRUE(Object::is_default("val_uint32", "111"));
    ASSERT_TRUE(Object::is_default("val_uint64", "111"));
    ASSERT_TRUE(Object::is_default("val_bool", "0"));
    ASSERT_TRUE(Object::is_default("val_bool", "000"));
    ASSERT_FALSE(Object::is_default("val_bool", "false"));
    ASSERT_FALSE(Object::is_default("val_bool", "true"));
    ASSERT_FALSE(Object::is_default("val_bool", "1"));
    ASSERT_TRUE(Object::is_default("val_float", "1.0001"));
    ASSERT_TRUE(Object::is_default("val_double", "1.0001"));
    ASSERT_TRUE(Object::is_default("val_str", "aaa"));

    ASSERT_FALSE(Object::is_default("val_int", "-1111"));
    ASSERT_FALSE(Object::is_default("val_int64", "-1111"));
    ASSERT_FALSE(Object::is_default("val_uint16", "1111"));
    ASSERT_FALSE(Object::is_default("val_uint32", "1111"));
    ASSERT_FALSE(Object::is_default("val_uint64", "1111"));
    ASSERT_FALSE(Object::is_default("val_bool", "01"));
    ASSERT_FALSE(Object::is_default("val_bool", "f1alse"));
    ASSERT_FALSE(Object::is_default("val_bool", "t1rue"));
    ASSERT_FALSE(Object::is_default("val_bool", "11"));
    ASSERT_FALSE(Object::is_default("val_float", "11.0001"));
    ASSERT_FALSE(Object::is_default("val_double", "11.0001"));
    ASSERT_FALSE(Object::is_default("val_str", "a1aa"));

    ASSERT_EQ(object.val_int_default(), -111);
    ASSERT_EQ(object.val_int64_default(), -111);
    ASSERT_EQ(object.val_uint16_default(), 111);
    ASSERT_EQ(object.val_uint32_default(), 111);
    ASSERT_EQ(object.val_uint64_default(), 111);
    ASSERT_EQ(object.val_bool_default(), false);
    ASSERT_FLOAT_EQ(object.val_float_default(), 1.0001);
    ASSERT_DOUBLE_EQ(object.val_double_default(), 1.0001);
    ASSERT_STREQ(object.val_str_default().c_str(), "aaa");

    ASSERT_TRUE(object.is_int_default("val_int", -111));
    ASSERT_TRUE(object.is_int64_default("val_int64", -111));
    ASSERT_TRUE(object.is_uint16_default("val_uint16", 111));
    ASSERT_TRUE(object.is_uint32_default("val_uint32", 111));
    ASSERT_TRUE(object.is_uint64_default("val_uint64", 111));
    ASSERT_TRUE(object.is_bool_default("val_bool", false));
    ASSERT_TRUE(object.is_float_default("val_float", 1.0001));
    ASSERT_TRUE(object.is_double_default("val_double", 1.0001));
    ASSERT_TRUE(object.is_str_default("val_str", "aaa"));

    ASSERT_FALSE(object.is_int_default("val_int", -1111));
    ASSERT_FALSE(object.is_int64_default("val_int64", -1111));
    ASSERT_FALSE(object.is_uint16_default("val_uint16", 1111));
    ASSERT_FALSE(object.is_uint32_default("val_uint32", 1111));
    ASSERT_FALSE(object.is_uint64_default("val_uint64", 1111));
    ASSERT_FALSE(object.is_bool_default("val_bool", true));
    ASSERT_FALSE(object.is_float_default("val_float", 21.0001));
    ASSERT_FALSE(object.is_double_default("val_double", 11.0001));
    ASSERT_FALSE(object.is_str_default("val_str", "1aaa"));

    // set value
    object.set_val_int(-222);
    object.set_val_int64(-222);
    object.set_val_uint16(222);
    object.set_val_uint32(222);
    object.set_val_uint64(222);
    object.set_val_bool(true);
    object.set_val_float(2.0002);
    object.set_val_double(2.0002);
    object.set_val_str("bbb");

    ASSERT_EQ(object.val_int(), -222);
    ASSERT_EQ(object.val_int64(), -222);
    ASSERT_EQ(object.val_uint16(), 222);
    ASSERT_EQ(object.val_uint32(), 222);
    ASSERT_EQ(object.val_uint64(), 222);
    ASSERT_EQ(object.val_bool(), true);
    ASSERT_FLOAT_EQ(object.val_float(), 2.0002);
    ASSERT_DOUBLE_EQ(object.val_double(), 2.0002);
    ASSERT_STREQ(object.val_str().c_str(), "bbb");

    ASSERT_EQ(object.get_int("val_int"), -222);
    ASSERT_EQ(object.get_int64("val_int64"), -222);
    ASSERT_EQ(object.get_uint16("val_uint16"), 222);
    ASSERT_EQ(object.get_uint32("val_uint32"), 222);
    ASSERT_EQ(object.get_uint64("val_uint64"), 222);
    ASSERT_EQ(object.get_bool("val_bool"), true);
    ASSERT_FLOAT_EQ(object.get_float("val_float"), 2.0002);
    ASSERT_DOUBLE_EQ(object.get_double("val_double"), 2.0002);
    ASSERT_STREQ(object.get_str("val_str").c_str(), "bbb");

    ASSERT_NE(object.val_int(), object.val_int_default());
    ASSERT_NE(object.val_int64(), object.val_int64_default());
    ASSERT_NE(object.val_uint16(), object.val_uint16_default());
    ASSERT_NE(object.val_uint32(), object.val_uint32_default());
    ASSERT_NE(object.val_uint64(), object.val_uint64_default());
    ASSERT_NE(object.val_bool(), object.val_bool_default());
    ASSERT_FALSE(object.val_float() == object.val_float_default());
    ASSERT_FALSE(object.val_double() == object.val_double_default());
    ASSERT_STRNE(object.val_str().c_str(), object.val_str_default().c_str());

}

TEST_F(TestModel, test_object) {
    SubModel sub_object;
    ASSERT_EQ(sub_object.sub_model_id(), sub_object.sub_model_id_default());
    ASSERT_EQ(sub_object.sub_model_id(), 111);

    Object object;
    ASSERT_EQ(object.val_object(), nullptr);

    // set sub object
    object.set_val_object(&sub_object);
    ASSERT_NE(object.val_object(), nullptr);
    ASSERT_EQ(object.val_object()->sub_model_id(), 111);

    // modify sub object value
    object.val_object()->set_sub_model_id(222);
    ASSERT_EQ(sub_object.sub_model_id(), 111);
    ASSERT_EQ(object.val_object()->sub_model_id(), 222);
    ASSERT_NE(object.val_object()->sub_model_id(), sub_object.sub_model_id());

    // set another sub object
    object.set_val_object(&sub_object);
    ASSERT_EQ(object.val_object()->sub_model_id(), 111);

    // set nullptr
    object.set_val_object(nullptr);
    ASSERT_EQ(object.val_object(), nullptr);

    // TODO.. more recursive level
}

TEST_F(TestModel, test_list) {
    List list;
    ASSERT_EQ(list.list_id(), list.list_id_default());
    ASSERT_EQ(list.list_id(), 111);

    Object object;
    ASSERT_EQ(object.val_list().size(), 0);

    // push list
    object.val_list().push_front(&list);
    ASSERT_EQ(object.val_list().size(), 1);
    ASSERT_NE(object.val_list().front(), nullptr);
    ASSERT_NE(object.val_list().back(), nullptr);
    ASSERT_EQ(object.val_list().front(), object.val_list().back());

    List* list_ptr = (List*)object.val_list().front();
    ASSERT_EQ(list_ptr->list_id(), 111);

    // modify list member
    list_ptr->set_list_id(222);
    ASSERT_EQ(list_ptr->list_id(), 222);
    ASSERT_EQ(object.val_list().front()->get_int("list_id"), 222);
    ASSERT_EQ(list.list_id(), 111);
    ASSERT_NE(object.val_list().front()->get_int("list_id"), list.list_id());

    // push another to list
    object.val_list().push_back(&list);
    ASSERT_EQ(object.val_list().size(), 2);
    ASSERT_NE(object.val_list().front(), object.val_list().back());

    // idx 0 still
    list_ptr = (List*)object.val_list().front();
    ASSERT_EQ(list_ptr->list_id(), 222);
    ASSERT_EQ(object.val_list().front()->get_int("list_id"), 222);

    // idx 1 added
    list_ptr = (List*)object.val_list().back();
    ASSERT_NE(list_ptr, nullptr);
    ASSERT_EQ(list_ptr->list_id(), 111);
    ASSERT_EQ(object.val_list().back()->get_int("list_id"), 111);

    // modify idx 2
    list_ptr->set_list_id(333);
    ASSERT_EQ(list_ptr->list_id(), 333);
    ASSERT_EQ(object.val_list().back()->get_int("list_id"), 333);
    ASSERT_EQ(list.list_id(), 111);
    ASSERT_NE(object.val_list().back()->get_int("list_id"), list.list_id());
    ASSERT_EQ(object.val_list().front()->get_int("list_id"), 222);
    ASSERT_NE(object.val_list().back()->get_int("list_id"),
            object.val_list().front()->get_int("list_id"));

    ASSERT_EQ(object.val_list()[0]->get_int("list_id"), 222);
    ASSERT_EQ(object.val_list()[1]->get_int("list_id"), 333);

    for (Model* item : object.val_list()) {
        ASSERT_STREQ(item->name().c_str(), "List");
        ASSERT_NE(item, nullptr);
    }

    for (const Model* item : object.val_list()) {
        ASSERT_STREQ(item->name().c_str(), "List");
        ASSERT_NE(item, nullptr);
    }

    auto iter = object.val_list().begin();
    for (; iter != object.val_list().end(); ++iter) {
        ASSERT_STREQ((*iter)->name().c_str(), "List");
        ASSERT_NE((*iter), nullptr);
    }

    // pop
    object.val_list().pop_front();
    ASSERT_EQ(object.val_list().size(), 1);
    ASSERT_EQ(object.val_list().front(), object.val_list().back());
    object.val_list().pop_back();
    ASSERT_EQ(object.val_list().size(), 0);
}

}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}