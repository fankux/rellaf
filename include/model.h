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
// reflection model
// TODO.. class memeber desc comment

#pragma once

#include <stdint.h>
#include <set>
#include <map>
#include <deque>
#include <functional>
#include "cast.hpp"
#include "common.h"

namespace rellaf {

/////////////////////// type declaration type stub //////////////////////

#define RELLAF_MODEL_TYPE_NAMES(_type_, _sign_)                                         \
static const std::map<std::string, _type_>& _sign_##_names() {                          \
    return _s_##_sign_##s;                                                              \
}

#define RELLAF_MODEL_TYPE_CONCERN(_sign_)                                               \
static bool _sign_##_concern(const std::string& key) {                                  \
    return _s_##_sign_##s.count(key) != 0;                                              \
}

#define RELLAF_MODEL_IS_TYPE_DEFAULT(_type_, _sign_)                                    \
static bool is_##_sign_##_default(const std::string& key, _type_ val) {                 \
    auto entry = _s_##_sign_##s.find(key);                                              \
    if (entry != _s_##_sign_##s.end()) {                                                \
        return entry->second == val;                                                    \
    }                                                                                   \
    return false;                                                                       \
}

#define RELLAF_MODEL_IS_TYPE_DEFAULT_REF(_type_, _sign_)                                \
static bool is_##_sign_##_default(const std::string& key, const _type_& val) {          \
    auto entry = _s_##_sign_##s.find(key);                                              \
    if (entry != _s_##_sign_##s.end()) {                                                \
        return entry->second == val;                                                    \
    }                                                                                   \
    return false;                                                                       \
}

#define RELLAF_MODEL_DCL_TYPE(_type_, _sign_)                                           \
public:                                                                                 \
RELLAF_MODEL_TYPE_NAMES(_type_, _sign_)                                                 \
RELLAF_MODEL_IS_TYPE_DEFAULT(_type_, _sign_)                                            \
RELLAF_MODEL_TYPE_CONCERN(_sign_)                                                       \
void set_##_sign_(const std::string& key, _type_ val) override {                        \
    auto entry = _s_##_sign_##s.find(key);                                              \
    if (entry != _s_##_sign_##s.end()) {                                                \
        _##_sign_##s[key] = val;                                                        \
    }                                                                                   \
}                                                                                       \
_type_ get_##_sign_(const std::string& key) const override {                            \
    auto entry = _##_sign_##s.find(key);                                                \
    return entry == _##_sign_##s.end() ? _s_##_sign_##s[key] : entry->second;           \
}                                                                                       \
const std::map<std::string, _type_>& get_##_sign_##_names() const override {            \
    return _sign_##_names();                                                            \
}                                                                                       \
bool is_##_sign_##_member(const std::string& key) const override {                      \
    return _sign_##_concern(key);                                                       \
}                                                                                       \
const std::map<std::string, _type_>& _sign_##s() override {                             \
    return _##_sign_##s;                                                                \
}                                                                                       \
private:                                                                                \
static std::map<std::string, _type_> _s_##_sign_##s

#define RELLAF_MODEL_DCL_TYPE_REF(_type_, _sign_)                                       \
public:                                                                                 \
RELLAF_MODEL_TYPE_NAMES(_type_, _sign_)                                                 \
RELLAF_MODEL_IS_TYPE_DEFAULT_REF(_type_, _sign_)                                        \
RELLAF_MODEL_TYPE_CONCERN(_sign_)                                                       \
void set_##_sign_(const std::string& key, const _type_& val) override {                 \
    auto entry = _s_##_sign_##s.find(key);                                              \
    if (entry != _s_##_sign_##s.end()) {                                                \
        _##_sign_##s[key] = val;                                                        \
    }                                                                                   \
}                                                                                       \
const _type_& get_##_sign_(const std::string& key) const override {                     \
    auto entry = _##_sign_##s.find(key);                                                \
    return entry == _##_sign_##s.end() ? _s_##_sign_##s[key] : entry->second;           \
}                                                                                       \
const std::map<std::string, _type_>& get_##_sign_##_names() const override {            \
    return _sign_##_names();                                                            \
}                                                                                       \
bool is_##_sign_##_member(const std::string& key) const override {                      \
    return _sign_##_concern(key);                                                       \
}                                                                                       \
const std::map<std::string, _type_>& _sign_##s() override {                             \
    return _##_sign_##s;                                                                \
}                                                                                       \
private:                                                                                \
static std::map<std::string, _type_> _s_##_sign_##s

#define RELLAF_MODEL_TYPE_REG(_clazz_, _type_, _sign_)                                  \
Reg(const std::string& name, _clazz_* inst, _type_ dft) {                               \
    _clazz_::_s_##_sign_##s.emplace(name, dft);                                         \
    inst->_##_sign_##s.emplace(name, dft);                                              \
}

#define RELLAF_MODEL_TYPE_REG_REF(_clazz_, _type_, _sign_)                              \
Reg(const std::string& name, _clazz_* inst, const _type_& dft) {                        \
    _clazz_::_s_##_sign_##s.emplace(name, dft);                                         \
    inst->_##_sign_##s.emplace(name, dft);                                              \
}

/////////////////////// type declaration without complex model //////////////////////
#define RELLAF_MODEL_DCL_PRIMITIVE(_clazz_)                                             \
RELLAF_DEFMOVE(_clazz_)                                                                 \
public:                                                                                 \
std::string name() const override { return #_clazz_; }                                  \
RELLAF_MODEL_DCL_TYPE(int, int);                                                        \
RELLAF_MODEL_DCL_TYPE(int64_t, int64);                                                  \
RELLAF_MODEL_DCL_TYPE(uint16_t, uint16);                                                \
RELLAF_MODEL_DCL_TYPE(uint32_t, uint32);                                                \
RELLAF_MODEL_DCL_TYPE(uint64_t, uint64);                                                \
RELLAF_MODEL_DCL_TYPE(bool, bool);                                                      \
RELLAF_MODEL_DCL_TYPE(float, float);                                                    \
RELLAF_MODEL_DCL_TYPE(double, double);                                                  \
RELLAF_MODEL_DCL_TYPE_REF(std::string, str);                                            \
public:                                                                                 \
static bool plain_concern(const std::string& key) {                                     \
    if (int_concern(key)) { return true; }                                              \
    if (int64_concern(key)) { return true; }                                            \
    if (uint16_concern(key)) { return true; }                                           \
    if (uint32_concern(key)) { return true; }                                           \
    if (uint64_concern(key)) { return true; }                                           \
    if (bool_concern(key)) { return true; }                                             \
    if (float_concern(key)) { return true; }                                            \
    if (double_concern(key)) { return true; }                                           \
    if (str_concern(key)) { return true; }                                              \
    return false;                                                                       \
}                                                                                       \
static bool is_default(const std::string& key, const std::string& val) {                \
    try {                                                                               \
        if (int_concern(key) && is_int_default(key, cast<int>(val))) {                  \
            return true;                                                                \
        }                                                                               \
        if (int64_concern(key) && is_int64_default(key, cast<int64_t>(val))) {          \
            return true;                                                                \
        }                                                                               \
        if (uint16_concern(key) && is_uint16_default(key, cast<uint16_t>(val))) {       \
            return true;                                                                \
        }                                                                               \
        if (uint32_concern(key) && is_uint32_default(key, cast<uint32_t>(val))) {       \
            return true;                                                                \
        }                                                                               \
        if (uint64_concern(key) && is_uint64_default(key, cast<uint64_t>(val))) {       \
            return true;                                                                \
        }                                                                               \
        if (bool_concern(key) && is_bool_default(key, cast<bool>(val))) {               \
            return true;                                                                \
        }                                                                               \
        if (float_concern(key) && is_float_default(key, cast<float>(val))) {            \
            return true;                                                                \
        }                                                                               \
        if (double_concern(key) && is_double_default(key, cast<double>(val))) {         \
            return true;                                                                \
        }                                                                               \
        if (str_concern(key) && is_str_default(key, val)) {                             \
            return true;                                                                \
        }                                                                               \
    } catch (std::exception& e) {                                                       \
        RELLAF_DEBUG("default object parse val failed : %s", e.what());                 \
        return false;                                                                   \
    }                                                                                   \
    return false;                                                                       \
}                                                                                       \
private:                                                                                \
class Reg {                                                                             \
public:                                                                                 \
RELLAF_MODEL_TYPE_REG(_clazz_, int, int);                                               \
RELLAF_MODEL_TYPE_REG(_clazz_, int64_t, int64)                                          \
RELLAF_MODEL_TYPE_REG(_clazz_, uint16_t, uint16)                                        \
RELLAF_MODEL_TYPE_REG(_clazz_, uint32_t, uint32)                                        \
RELLAF_MODEL_TYPE_REG(_clazz_, uint64_t, uint64)                                        \
RELLAF_MODEL_TYPE_REG(_clazz_, bool, bool)                                              \
RELLAF_MODEL_TYPE_REG(_clazz_, float, float)                                            \
RELLAF_MODEL_TYPE_REG(_clazz_, double, double)                                          \
RELLAF_MODEL_TYPE_REG_REF(_clazz_, std::string, str)                                    \
};

/////////////////////// type declaration //////////////////////

#define RELLAF_MODEL_DCL(_clazz_)                                                       \
RELLAF_MODEL_DCL_PRIMITIVE(_clazz_)                                                     \
public:                                                                                 \
Model* create() override { return new(std::nothrow)_clazz_; }                           \
Model* clone() override {                                                               \
    Model* new_model = new(std::nothrow)_clazz_;                                        \
    *((_clazz_*)new_model) = *((_clazz_*)this);                                         \
    return new_model;                                                                   \
}                                                                                       \
bool is_plain_member(const std::string& key) const override {                           \
    return _clazz_::plain_concern(key);                                                 \
}                                                                                       \
private:                                                                                \
class RegList {                                                                         \
public:                                                                                 \
    RegList(const std::string& name, _clazz_* inst) {                                   \
        _clazz_::_s_list_names.emplace(name);                                           \
        inst->_lists.emplace(name, ModelList());                                        \
    }                                                                                   \
};                                                                                      \
class RegObject {                                                                       \
public:                                                                                 \
    explicit RegObject(const std::string& name) {                                       \
        _clazz_::_s_object_names.emplace(name);                                         \
    }                                                                                   \
};                                                                                      \
static std::set<std::string> _s_list_names;                                             \
static std::set<std::string> _s_object_names;

/////////////////////// type definition type stub ////////////////////

#define RELLAF_MODEL_DEF_TYPE(_clazz_, _type_, _sign_)                                  \
std::map<std::string, _type_> _clazz_::_s_##_sign_##s

/////////////////////// type definition without complex model ////////////////////
#define RELLAF_MODEL_DEF_PRIMITIVE(_clazz_)                                             \
RELLAF_MODEL_DEF_TYPE(_clazz_, int, int);                                               \
RELLAF_MODEL_DEF_TYPE(_clazz_, int64_t, int64);                                         \
RELLAF_MODEL_DEF_TYPE(_clazz_, uint16_t, uint16);                                       \
RELLAF_MODEL_DEF_TYPE(_clazz_, uint32_t, uint32);                                       \
RELLAF_MODEL_DEF_TYPE(_clazz_, uint64_t, uint64);                                       \
RELLAF_MODEL_DEF_TYPE(_clazz_, bool, bool);                                             \
RELLAF_MODEL_DEF_TYPE(_clazz_, float, float);                                           \
RELLAF_MODEL_DEF_TYPE(_clazz_, double, double);                                         \
RELLAF_MODEL_DEF_TYPE(_clazz_, std::string, str);                                       \
static volatile _clazz_ _s_c_##_clazz_; // force init static context

/////////////////////// type definition ////////////////////
#define RELLAF_MODEL_DEF(_clazz_)                                                       \
std::set<std::string> _clazz_::_s_list_names;                                           \
std::set<std::string> _clazz_::_s_object_names;                                         \
RELLAF_MODEL_DEF_PRIMITIVE(_clazz_)

/////////////////////// abstract interface class method sign ////////////////////
#define RELLAF_MODEL_DCL_ENTRY_SIGN(_type_, _sign_)                                     \
public:                                                                                 \
virtual void set_##_sign_(const std::string& key, _type_ val) = 0;                      \
virtual _type_ get_##_sign_(const std::string& key) const = 0;                          \
virtual const std::map<std::string, _type_>& get_##_sign_##_names() const = 0;          \
virtual bool is_##_sign_##_member(const std::string& key) const = 0;                    \
virtual const std::map<std::string, _type_>& _sign_##s() = 0;                           \
protected:                                                                              \
std::map<std::string, _type_> _##_sign_##s;

#define RELLAF_MODEL_DCL_ENTRY_SIGN_REF(_type_, _sign_)                                 \
public:                                                                                 \
virtual void set_##_sign_(const std::string& key, const _type_& val) = 0;               \
virtual const _type_& get_##_sign_(const std::string& key) const = 0;                   \
virtual const std::map<std::string, _type_>& get_##_sign_##_names() const = 0;          \
virtual bool is_##_sign_##_member(const std::string& key) const = 0;                    \
virtual const std::map<std::string, _type_>& _sign_##s() = 0;                           \
protected:                                                                              \
std::map<std::string, _type_> _##_sign_##s;

/////////////////////// base model class ////////////////////
class ModelList;

class Model {
RELLAF_DEFMOVE(Model)

RELLAF_MODEL_DCL_ENTRY_SIGN(int, int);
RELLAF_MODEL_DCL_ENTRY_SIGN(int64_t, int64);
RELLAF_MODEL_DCL_ENTRY_SIGN(uint16_t, uint16);
RELLAF_MODEL_DCL_ENTRY_SIGN(uint32_t, uint32);
RELLAF_MODEL_DCL_ENTRY_SIGN(uint64_t, uint64);
RELLAF_MODEL_DCL_ENTRY_SIGN(bool, bool);
RELLAF_MODEL_DCL_ENTRY_SIGN(float, float);
RELLAF_MODEL_DCL_ENTRY_SIGN(double, double);
RELLAF_MODEL_DCL_ENTRY_SIGN_REF(std::string, str);

public:
    virtual ~Model();

    virtual std::string name() const = 0;

    virtual Model* create() = 0;

    virtual Model* clone() = 0;

    virtual std::string debug_str() const;

    virtual bool is_plain() const {
        return false;
    }

    virtual bool is_plain_member(const std::string& key) const = 0;

    const std::map<std::string, Model*>& get_objects() const {
        return _objects;
    }

    bool is_object_member(const std::string& name) const {
        return _objects.count(name) != 0;
    }

    Model* get_object(const std::string& name);

    const Model* get_object(const std::string& name) const;

    const std::map<std::string, ModelList>& get_lists() const {
        return _lists;
    }

    bool is_list_member(const std::string& name) const {
        return _lists.count(name) != 0;
    }

    ModelList& get_list(const std::string& name);

    const ModelList& get_list(const std::string& name) const;

protected:
    std::map<std::string, ModelList> _lists;
    std::map<std::string, Model*> _objects;
};

/////////////////////// model list ////////////////////
class PlainWrap : public Model {
    // TODO make primitive types private
RELLAF_DEFMOVE(PlainWrap)

public:
//    virtual std::string name() const = 0;

//    virtual Model* create() = 0;

//    virtual Model* clone() = 0;

//    virtual std::string debug_str() { return ""; };

    bool is_plain() const override {
        return true;
    }

    std::string debug_str() const override {
        return str();
    }

    virtual std::string str() const = 0;

private:
    bool is_plain_member(const std::string& key) const override {
        return false;
    }

RELLAF_MODEL_DCL_TYPE(int, int);
RELLAF_MODEL_DCL_TYPE(int64_t, int64);
RELLAF_MODEL_DCL_TYPE(uint16_t, uint16);
RELLAF_MODEL_DCL_TYPE(uint32_t, uint32);
RELLAF_MODEL_DCL_TYPE(uint64_t, uint64);
RELLAF_MODEL_DCL_TYPE(bool, bool);
RELLAF_MODEL_DCL_TYPE(float, float);
RELLAF_MODEL_DCL_TYPE(double, double);
RELLAF_MODEL_DCL_TYPE_REF(std::string, str);
};

#define RELLAF_PLAIN_DCL(_clazz_, _type_, _dft_)                        \
RELLAF_DEFMOVE(_clazz_)                                                 \
public:                                                                 \
std::string name() const override { return #_clazz_; }                  \
Model* create() override { return new(std::nothrow)_clazz_; }           \
Model* clone() override {                                               \
    Model* new_model = new(std::nothrow)_clazz_;                        \
    *((_clazz_*)new_model) = *((_clazz_*)this);                         \
    return new_model;                                                   \
}                                                                       \
_clazz_(const _type_& val) : _val(val) {}                               \
_type_ value() const { return _val; }                                   \
void set_value(const _type_& val) { _val = val; }                       \
private:                                                                \
_type_ _val = _dft_;

class ModelList {
RELLAF_DEFMOVE(ModelList)

public:
    virtual ~ModelList();

    size_t size() const;

    bool empty() const;

    void clear();

    void push_front(Model* model);

    void push_back(Model* model);

    void pop_front();

    void pop_back();

    Model* front();

    const Model* front() const;

    Model* back();

    const Model* back() const;

    void set(size_t idx, Model* model);

    const Model* get(size_t idx) const;

    const Model* operator[](size_t idx) const;

    std::deque<Model*>::const_iterator begin() const;

    std::deque<Model*>::const_iterator end() const;

private:
    std::deque<Model*> _items;
};

/////////////////////// definition method ////////////////////
#define RELLAF_MODEL_DEF_type(_type_, _sign_, _name_, _dft_)                            \
public:                                                                                 \
    _type_ _name_() const {                                                             \
        return get_##_sign_(#_name_);                                                   \
    }                                                                                   \
    void set_##_name_(const _type_& val) {                                              \
        return set_##_sign_(#_name_, val);                                              \
    }                                                                                   \
    _type_ _name_##_default() const {                                                   \
        return _s_##_sign_##s[#_name_];                                                 \
    }                                                                                   \
private:                                                                                \
    Reg _reg_##_name_{#_name_, this, (_type_)_dft_}

#define RELLAF_MODEL_DEF_int(_name_, _dft_) RELLAF_MODEL_DEF_type(int, int, _name_, _dft_)
#define RELLAF_MODEL_DEF_int64(_name_, _dft_) RELLAF_MODEL_DEF_type(int64_t, int64, _name_, _dft_)
#define RELLAF_MODEL_DEF_uint16(_name_, _dft_) RELLAF_MODEL_DEF_type(uint16_t, uint16, _name_, _dft_)
#define RELLAF_MODEL_DEF_uint32(_name_, _dft_) RELLAF_MODEL_DEF_type(uint32_t, uint32, _name_, _dft_)
#define RELLAF_MODEL_DEF_uint64(_name_, _dft_) RELLAF_MODEL_DEF_type(uint64_t, uint64, _name_, _dft_)
#define RELLAF_MODEL_DEF_bool(_name_, _dft_) RELLAF_MODEL_DEF_type(bool, bool, _name_, _dft_)
#define RELLAF_MODEL_DEF_float(_name_, _dft_) RELLAF_MODEL_DEF_type(float, float, _name_, _dft_)
#define RELLAF_MODEL_DEF_double(_name_, _dft_) RELLAF_MODEL_DEF_type(double, double, _name_, _dft_)
#define RELLAF_MODEL_DEF_str(_name_, _dft_) RELLAF_MODEL_DEF_type(std::string, str, _name_, _dft_)

#define RELLAF_MODEL_DEF_object(_name_, _type_)             \
public:                                                     \
    _type_* _name_() {                                      \
        return (_type_*)get_object(#_name_);                \
    }                                                       \
    _type_* _name_() const {                                \
        return (_type_*)get_object(#_name_);                \
    }                                                       \
    void set_##_name_(_type_* val) {                        \
        auto entry = _objects.find(#_name_);                \
        if (entry != _objects.end()) {                      \
            delete entry->second;                           \
            entry->second = nullptr;                        \
            if (val != nullptr) {                           \
                entry->second = val->clone();               \
            }                                               \
        } else {                                            \
            if (val != nullptr) {                           \
                _objects.emplace(#_name_, val->clone());    \
            } else {                                        \
                _objects.emplace(#_name_, nullptr);         \
            }                                               \
        }                                                   \
    }                                                       \
private:                                                    \
    RegObject _reg_##_name_##_object{#_name_}


#define RELLAF_MODEL_DEF_list(_name_, _type_)               \
public:                                                     \
    ModelList& _name_() {                                   \
        return get_list(#_name_);                           \
    }                                                       \
private:                                                    \
    RegList _reg_##_name_##_list{#_name_, this}

}