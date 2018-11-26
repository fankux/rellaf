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

#define rellaf_enum(_clazz_) _clazz_::e()

#define rellaf_enum_dcl(_clazz_)                                                \
public:                                                                         \
static _clazz_& e();                                                            \
const std::map<std::string, int>& names() const override {                      \
    return _name_refs;                                                          \
}                                                                               \
const std::map<int, std::string>& codes() const override {                      \
    return _code_refs;                                                          \
}                                                                               \
private:                                                                        \
    explicit _clazz_(const std::string& name): IEnum(name) {}                   \
    class Reg {                                                                 \
    public:                                                                     \
        Reg(_clazz_* inst, int c, const std::string& n);                        \
    };                                                                          \
private:                                                                        \
    std::map<std::string, int> _name_refs;                                      \
    std::map<int, std::string> _code_refs

#define rellaf_enum_def(_clazz_)                                                \
_clazz_::Reg::Reg(_clazz_* inst, int c, const std::string& n) {                 \
    inst->_name_refs.emplace(n, c);                                             \
    inst->_code_refs.emplace(c, n);                                             \
}                                                                               \
_clazz_& _clazz_::e() {                                                         \
    static _clazz_ _enum_(#_clazz_);                                            \
    return _enum_;                                                              \
}
#if __cplusplus >= 201703L
#define rellaf_enum_item_def(_code_, _name_)                                    \
public:                                                                         \
    constexpr static int _name_##_code{_code_};                                 \
    const EnumItem _name_{_code_, #_name_};                                     \
private:                                                                        \
    const Reg _reg_##_name_{this, _code_, #_name_}
#else
#define rellaf_enum_item_def(_code_, _name_)                                    \
public:                                                                         \
    const EnumItem _name_{_code_, #_name_};                                     \
private:                                                                        \
    const Reg _reg_##_name_{this, _code_, #_name_}
#endif

class IEnum {
public:
    virtual ~IEnum() = default;

    class EnumItem {
    RELLAF_DEFMOVE_NO_CTOR(EnumItem)

    public:
        EnumItem() = default;

        EnumItem(int c, const std::string& n) : code(c), name(n) {}

        bool available() const {
            return !name.empty();
        }

        std::string str() const {
            return name + "[" + std::to_string(code) + "]";
        }

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

    virtual const std::map<std::string, int>& names() const = 0;

    virtual const std::map<int, std::string>& codes() const = 0;

    EnumItem get_by_name(const std::string& name) const;

    EnumItem get_by_code(int code) const;

    EnumItem get(const std::string& name) const;

    EnumItem get(int code) const;

    bool exist(const std::string& name) const;

    bool exist(int code) const;

protected:
    explicit IEnum(const std::string& name) : _name(name) {}

protected:
    std::string _name;
};

typedef IEnum::EnumItem EnumItem;
}
