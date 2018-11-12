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
// NEVER use this as member at STATIC context !

#pragma once

#include <string>
#include <map>
#include "common.h"

namespace rellaf {

#define RELLAF_ENUM_DCL(_clazz_)                                                \
public:                                                                         \
    static _clazz_& e() {                                                       \
        static _clazz_ _enum_(#_clazz_);                                        \
        return _enum_;                                                          \
}                                                                               \
const std::map<std::string, const EnumItem*>& names() const override {          \
    return _clazz_::_s_name_refs;                                               \
}                                                                               \
const std::map<int, const EnumItem*>& codes() const override {                  \
    return _clazz_::_s_code_refs;                                               \
}                                                                               \
private:                                                                        \
    explicit _clazz_(const std::string& name): IEnum(name) {}                   \
    static std::map<std::string, const EnumItem*> _s_name_refs;                 \
    static std::map<int, const EnumItem*> _s_code_refs;                         \
class Reg {                                                                     \
public:                                                                         \
    Reg(int c, const std::string& n, const EnumItem* ptr) {                     \
        _clazz_::_s_name_refs.emplace(n, ptr);                                  \
        _clazz_::_s_code_refs.emplace(c, ptr);                                  \
    }                                                                           \
}

#define RELLAF_ENUM_DEF(_clazz_)                                                \
std::map<std::string, const EnumItem*> _clazz_::_s_name_refs;                   \
std::map<int, const EnumItem*> _clazz_::_s_code_refs;                           \


#define RELLAF_ENUM_ITEM_DEF(_code_, _name_)                                    \
public:                                                                         \
    EnumItem _name_{_code_, #_name_};                                           \
private:                                                                        \
    Reg _reg_##_name_{_code_, #_name_, &_name_}

class IEnum {
public:
    class EnumItem {

    public:
        EnumItem() = default;

        EnumItem(int c, std::string n) : code(c), name(std::move(n)) {}

        std::string str() const {
            return name + "[" + std::to_string(code) + "]";
        }

        EnumItem& operator=(const EnumItem&) = default;

        friend bool operator==(const EnumItem& lhs, const EnumItem& rhs) {
            return lhs.code == rhs.code;
        }

        friend bool operator!=(const EnumItem& lhs, const EnumItem& rhs) {
            return !(rhs == lhs);
        }

        friend bool operator<(const EnumItem& lhs, const EnumItem& rhs) {
            return lhs.code < rhs.code;
        }

        int code = 0;
        std::string name;
    };

public:

    explicit IEnum(std::string name) : _name(std::move(name)) {}

    virtual const std::map<std::string, const EnumItem*>& names() const = 0;

    virtual const std::map<int, const EnumItem*>& codes() const = 0;

    const EnumItem* get_by_name(const std::string& name);

    const EnumItem* get_by_code(int code);

    bool exist(const std::string& name);

    bool exist(int code);

protected:
    std::string _name;
};

typedef IEnum::EnumItem EnumItem;
}
