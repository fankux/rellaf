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
// TODO... valgrind mem check
// TODO... split file, static connect check

#include "gtest/gtest.h"
#include "test_common.h"
#include "common.h"
#include "model.h"

namespace rellaf {
namespace test {

class TestModel : public testing::Test {
protected:
    TestModel() = default;

    ~TestModel() override = default;

    void SetUp() override {}
};

class SubModel : public Object {
rellaf_model_dcl(SubModel);

rellaf_model_def_int(sub_model_id, 111);

};

rellaf_model_def(SubModel);

class SubObj : public Object {
rellaf_model_dcl(SubObj);

rellaf_model_def_int(list_id, 111);

};

rellaf_model_def(SubObj);

class Obj : public Object {
rellaf_model_dcl(Obj);

rellaf_model_def_char(val_char, 'c');
rellaf_model_def_int16(val_int16, -111);
rellaf_model_def_int(val_int, -111);
rellaf_model_def_int64(val_int64, -111);
rellaf_model_def_uint16(val_uint16, 111);
rellaf_model_def_uint32(val_uint32, 111);
rellaf_model_def_uint64(val_uint64, 111);
rellaf_model_def_bool(val_bool, false);
rellaf_model_def_float(val_float, 1.0001);
rellaf_model_def_double(val_double, 1.0001);
rellaf_model_def_str(val_str, "aaa");

rellaf_model_def_object(val_object, SubModel);

rellaf_model_def_list(val_list, SubObj);

rellaf_model_def_list(plain_list, Plain<uint32_t>);

};

rellaf_model_def(Obj);

TEST_F(TestModel, test_primitive) {

    Obj object;

    // member
    ASSERT_TRUE(Obj::plain_concern("val_char"));
    ASSERT_TRUE(Obj::plain_concern("val_int16"));
    ASSERT_TRUE(Obj::plain_concern("val_int"));
    ASSERT_TRUE(Obj::plain_concern("val_int64"));
    ASSERT_TRUE(Obj::plain_concern("val_uint16"));
    ASSERT_TRUE(Obj::plain_concern("val_uint32"));
    ASSERT_TRUE(Obj::plain_concern("val_uint64"));
    ASSERT_TRUE(Obj::plain_concern("val_bool"));
    ASSERT_TRUE(Obj::plain_concern("val_float"));
    ASSERT_TRUE(Obj::plain_concern("val_double"));
    ASSERT_TRUE(Obj::plain_concern("val_str"));
    ASSERT_FALSE(Obj::plain_concern("not_exist"));

    ASSERT_TRUE(object.is_plain_member("val_char"));
    ASSERT_TRUE(object.is_plain_member("val_int16"));
    ASSERT_TRUE(object.is_plain_member("val_int"));
    ASSERT_TRUE(object.is_plain_member("val_int64"));
    ASSERT_TRUE(object.is_plain_member("val_uint16"));
    ASSERT_TRUE(object.is_plain_member("val_uint32"));
    ASSERT_TRUE(object.is_plain_member("val_uint64"));
    ASSERT_TRUE(object.is_plain_member("val_bool"));
    ASSERT_TRUE(object.is_plain_member("val_float"));
    ASSERT_TRUE(object.is_plain_member("val_double"));
    ASSERT_TRUE(object.is_plain_member("val_str"));
    ASSERT_FALSE(object.is_plain_member("not_exist"));

    ASSERT_TRUE(object.is_plain_member("val_char"));
    ASSERT_TRUE(object.is_plain_member("val_int16"));
    ASSERT_TRUE(object.is_plain_member("val_int"));
    ASSERT_TRUE(object.is_plain_member("val_int64"));
    ASSERT_TRUE(object.is_plain_member("val_uint16"));
    ASSERT_TRUE(object.is_plain_member("val_uint32"));
    ASSERT_TRUE(object.is_plain_member("val_uint64"));
    ASSERT_TRUE(object.is_plain_member("val_bool"));
    ASSERT_TRUE(object.is_plain_member("val_float"));
    ASSERT_TRUE(object.is_plain_member("val_double"));
    ASSERT_TRUE(object.is_plain_member("val_str"));
    ASSERT_FALSE(object.is_plain_member("not_exist"));

    ASSERT_TRUE(Obj::plain_concern("val_char"));
    ASSERT_TRUE(Obj::plain_concern("val_int16"));
    ASSERT_TRUE(Obj::plain_concern("val_int"));
    ASSERT_TRUE(Obj::plain_concern("val_int64"));
    ASSERT_TRUE(Obj::plain_concern("val_uint16"));
    ASSERT_TRUE(Obj::plain_concern("val_uint32"));
    ASSERT_TRUE(Obj::plain_concern("val_uint64"));
    ASSERT_TRUE(Obj::plain_concern("val_bool"));
    ASSERT_TRUE(Obj::plain_concern("val_float"));
    ASSERT_TRUE(Obj::plain_concern("val_double"));
    ASSERT_TRUE(Obj::plain_concern("val_str"));
    ASSERT_FALSE(Obj::plain_concern("not_exist"));

    ASSERT_TRUE(map_keys_equal_set(Obj::plain_names(), {
            "val_char",
            "val_int16",
            "val_int",
            "val_int64",
            "val_uint16",
            "val_uint32",
            "val_uint64",
            "val_bool",
            "val_float",
            "val_double",
            "val_str"
    }));

    ASSERT_TRUE(map_keys_equal_set(object.get_plains(), {
            "val_char",
            "val_int16",
            "val_int",
            "val_int64",
            "val_uint16",
            "val_uint32",
            "val_uint64",
            "val_bool",
            "val_float",
            "val_double",
            "val_str"
    }));

    //default
    ASSERT_EQ(object.val_char(), 'c');
    ASSERT_EQ(object.val_int16(), -111);
    ASSERT_EQ(object.val_int(), -111);
    ASSERT_EQ(object.val_int64(), -111);
    ASSERT_EQ(object.val_uint16(), 111);
    ASSERT_EQ(object.val_uint32(), 111);
    ASSERT_EQ(object.val_uint64(), 111);
    ASSERT_EQ(object.val_bool(), false);
    ASSERT_FLOAT_EQ(object.val_float(), 1.0001);
    ASSERT_DOUBLE_EQ(object.val_double(), 1.0001);
    ASSERT_STREQ(object.val_str().c_str(), "aaa");

    ASSERT_EQ(object.get_plain<char>("val_char")->value(), 'c');
//    ASSERT_EQ(Obj::int16_value(object.get_plain("val_int16")), -111);
//    ASSERT_EQ(Obj::int_value(object.get_plain("val_int")), -111);
//    ASSERT_EQ(Obj::int64_value(object.get_plain("val_int64")), -111);
//    ASSERT_EQ(Obj::uint16_value(object.get_plain("val_uint16")), 111);
//    ASSERT_EQ(Obj::uint32_value(object.get_plain("val_uint32")), 111);
//    ASSERT_EQ(Obj::uint64_value(object.get_plain("val_uint64")), 111);
//    ASSERT_EQ(Obj::bool_value(object.get_plain("val_bool")), false);
//    ASSERT_FLOAT_EQ(Obj::float_value(object.get_plain("val_float")), 1.0001);
//    ASSERT_DOUBLE_EQ(Obj::double_value(object.get_plain("val_double")), 1.0001);
//    ASSERT_STREQ(Obj::str_value(object.get_plain("val_str")).c_str(), "aaa");

    ASSERT_TRUE(Obj::is_plain_default("val_char", "c"));
    ASSERT_TRUE(Obj::is_plain_default("val_int16", "-111"));
    ASSERT_TRUE(Obj::is_plain_default("val_int", "-111"));
    ASSERT_TRUE(Obj::is_plain_default("val_int64", "-111"));
    ASSERT_TRUE(Obj::is_plain_default("val_uint16", "111"));
    ASSERT_TRUE(Obj::is_plain_default("val_uint32", "111"));
    ASSERT_TRUE(Obj::is_plain_default("val_uint64", "111"));
    ASSERT_TRUE(Obj::is_plain_default("val_bool", "0"));
    ASSERT_TRUE(Obj::is_plain_default("val_bool", "000"));
    ASSERT_FALSE(Obj::is_plain_default("val_bool", "false"));
    ASSERT_FALSE(Obj::is_plain_default("val_bool", "true"));
    ASSERT_FALSE(Obj::is_plain_default("val_bool", "1"));
    ASSERT_TRUE(Obj::is_plain_default("val_float", "1.0001"));
    ASSERT_TRUE(Obj::is_plain_default("val_double", "1.0001"));
    ASSERT_TRUE(Obj::is_plain_default("val_str", "aaa"));

    ASSERT_FALSE(Obj::is_plain_default("val_char", "1c"));
    ASSERT_FALSE(Obj::is_plain_default("val_int16", "-1111"));
    ASSERT_FALSE(Obj::is_plain_default("val_int", "-1111"));
    ASSERT_FALSE(Obj::is_plain_default("val_int64", "-1111"));
    ASSERT_FALSE(Obj::is_plain_default("val_uint16", "1111"));
    ASSERT_FALSE(Obj::is_plain_default("val_uint32", "1111"));
    ASSERT_FALSE(Obj::is_plain_default("val_uint64", "1111"));
    ASSERT_FALSE(Obj::is_plain_default("val_bool", "01"));
    ASSERT_FALSE(Obj::is_plain_default("val_bool", "f1alse"));
    ASSERT_FALSE(Obj::is_plain_default("val_bool", "t1rue"));
    ASSERT_FALSE(Obj::is_plain_default("val_bool", "11"));
    ASSERT_FALSE(Obj::is_plain_default("val_float", "11.0001"));
    ASSERT_FALSE(Obj::is_plain_default("val_double", "11.0001"));
    ASSERT_FALSE(Obj::is_plain_default("val_str", "a1aa"));

    ASSERT_EQ(object.val_char_default(), 'c');
    ASSERT_EQ(object.val_int16_default(), -111);
    ASSERT_EQ(object.val_int_default(), -111);
    ASSERT_EQ(object.val_int64_default(), -111);
    ASSERT_EQ(object.val_uint16_default(), 111);
    ASSERT_EQ(object.val_uint32_default(), 111);
    ASSERT_EQ(object.val_uint64_default(), 111);
    ASSERT_EQ(object.val_bool_default(), false);
    ASSERT_FLOAT_EQ(object.val_float_default(), 1.0001);
    ASSERT_DOUBLE_EQ(object.val_double_default(), 1.0001);
    ASSERT_STREQ(object.val_str_default().c_str(), "aaa");

    // set value
    object.set_val_char('b');
    object.set_val_int16(-222);
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

//    ASSERT_EQ(Obj::int16_value(object.get_plain("val_int")), -222);
//    ASSERT_EQ(Obj::int64_value(object.get_plain("val_int64")), -222);
//    ASSERT_EQ(Obj::uint16_value(object.get_plain("val_uint16")), 222);
//    ASSERT_EQ(Obj::uint32_value(object.get_plain("val_uint32")), 222);
//    ASSERT_EQ(Obj::uint64_value(object.get_plain("val_uint64")), 222);
//    ASSERT_EQ(Obj::bool_value(object.get_plain("val_bool")), true);
//    ASSERT_FLOAT_EQ(Obj::float_value(object.get_plain("val_float")), 2.0002);
//    ASSERT_DOUBLE_EQ(Obj::double_value(object.get_plain("val_double")), 2.0002);
//    ASSERT_STREQ(Obj::str_value(object.get_plain("val_str")).c_str(), "bbb");

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

    Obj object;
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
    SubObj subobj;
    ASSERT_EQ(subobj.list_id(), subobj.list_id_default());
    ASSERT_EQ(subobj.list_id(), 111);

    RELLAF_DEBUG("subobj : %s", subobj.debug_str().c_str());

    Obj obj;
    ASSERT_EQ(obj.val_list().size(), 0);

    // push list
    obj.val_list().push_back(subobj);
    RELLAF_DEBUG("list size : %zu", obj.val_list().size());

    ASSERT_EQ(obj.val_list().size(), 1);
    ASSERT_NE(obj.val_list().front(), nullptr);
    ASSERT_NE(obj.val_list().back(), nullptr);
    ASSERT_EQ(obj.val_list().front(), obj.val_list().back());
    RELLAF_DEBUG("obj : %s", obj.debug_str().c_str());

    SubObj* subobj_ptr = obj.val_list().front<SubObj>();
    ASSERT_EQ(subobj_ptr->list_id(), 111);

    // modify list member
    ASSERT_EQ(obj.val_list().front<SubObj>()->get_plain<int>("list_id")->value(), 111);
    ASSERT_EQ(subobj.list_id(), 111);
    ASSERT_EQ(obj.val_list().size(), 1);
    RELLAF_DEBUG("val List : %s", obj.val_list().front()->debug_str().c_str());
    ASSERT_EQ(obj.val_list().at<SubObj>(1), nullptr);
    obj.val_list().front<SubObj>()->set_list_id(222);
    RELLAF_DEBUG("val List : %s", obj.val_list().front()->debug_str().c_str());
    ASSERT_EQ(obj.val_list().front<SubObj>()->get_plain<int>("list_id")->value(), 222);
    ASSERT_NE(obj.val_list().front<SubObj>()->get_plain<int>("list_id")->value(), subobj.list_id());

    subobj_ptr->set_list_id(111);
    ASSERT_EQ(obj.val_list().front<SubObj>()->list_id(), 111);

    // push another to list
    subobj.set_list_id(222);
    obj.val_list().push_back(subobj);
    ASSERT_EQ(obj.val_list().size(), 2);
    ASSERT_NE(obj.val_list().front(), obj.val_list().back());

    // idx 0 still
    subobj_ptr = (SubObj*)obj.val_list().front();
    ASSERT_EQ(subobj_ptr->list_id(), 111);
    ASSERT_EQ(obj.val_list().front<Object>()->get_plain<int>("list_id")->value(), 111);

    // idx 1 added

    subobj_ptr = (SubObj*)obj.val_list().back();
    ASSERT_NE(subobj_ptr, nullptr);
    ASSERT_EQ(subobj_ptr->list_id(), 222);
    ASSERT_EQ(obj.val_list().back<Object>()->get_plain<int>("list_id")->value(), 222);

    // modify idx 2
    subobj_ptr->set_list_id(333);
    ASSERT_EQ(subobj_ptr->list_id(), 333);
    ASSERT_EQ(obj.val_list().back<Object>()->get_plain<int>("list_id")->value(), 333);
    ASSERT_EQ(subobj.list_id(), 222);
    ASSERT_NE(obj.val_list().back<Object>()->get_plain<int>("list_id")->value(), subobj.list_id());
    ASSERT_EQ(obj.val_list().front<Object>()->get_plain<int>("list_id")->value(), 111);
    ASSERT_NE(obj.val_list().back<Object>()->get_plain<int>("list_id")->value(),
            obj.val_list().front<Object>()->get_plain<int>("list_id")->value());

    ASSERT_EQ(((Object*)obj.val_list()[0])->get_plain<int>("list_id")->value(), 111);
    ASSERT_EQ(((Object*)obj.val_list()[1])->get_plain<int>("list_id")->value(), 333);

    for (Model* item : obj.val_list()) {
        ASSERT_STREQ(item->rellaf_name().c_str(), "SubObj");
        ASSERT_NE(item, nullptr);
    }

    for (const Model* item : obj.val_list()) {
        ASSERT_STREQ(((Object*)item)->rellaf_name().c_str(), "SubObj");
        ASSERT_NE(item, nullptr);
    }

    auto iter = obj.val_list().begin();
    for (; iter != obj.val_list().end(); ++iter) {
        ASSERT_STREQ((*iter)->rellaf_name().c_str(), "SubObj");
        ASSERT_NE((*iter), nullptr);
    }

    // pop
    obj.val_list().pop_front();
    ASSERT_EQ(obj.val_list().size(), 1);
    ASSERT_EQ(obj.val_list().front(), obj.val_list().back());
    obj.val_list().pop_back();
    ASSERT_EQ(obj.val_list().size(), 0);
}

TEST_F(TestModel, test_plain) {
    Plain<uint32_t> list_int = 0;
    ASSERT_EQ(list_int.value(), 0);
    list_int.set(2);
    ASSERT_EQ(list_int.value(), 2);

    Obj object;
    RELLAF_DEBUG("lists size : %zu", object.get_lists().size());
    for (auto& entry : object.get_lists()) {
        RELLAF_DEBUG("list key : %s", entry.first.c_str());

    }

    object.plain_list().push_back(list_int);
    RELLAF_DEBUG("lists lists size : %zu", object.plain_list().size());

    list_int.set(3);
    object.plain_list().push_back(list_int);
    RELLAF_DEBUG("lists lists size : %zu", object.plain_list().size());

    Model* model = object.plain_list().front();
    RELLAF_DEBUG("model ptr : %zu", reinterpret_cast<size_t>(model));

    ASSERT_EQ(object.plain_list().front<Plain<uint32_t>>()->value(), 2);
    ASSERT_EQ(object.plain_list().back<Plain<uint32_t>>()->value(), 3);

    for (Model* item : object.val_list()) {
        ASSERT_STREQ(item->rellaf_name().c_str(), "ListInt");
        ASSERT_NE(item, nullptr);
    }
}

}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}