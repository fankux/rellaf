#include <utility>

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
#include "model_type.h"

namespace rellaf {

/////////////////////// type declaration without complex model //////////////////////
#define RELLAF_MODEL_DCL_PLAIN(_clazz_)                                                 \
RELLAF_DEFMOVE_NO_CTOR(_clazz_)                                                         \
public:                                                                                 \
_clazz_() : Object() {}                                                                 \
virtual ~_clazz_() {                                                                    \
    for (auto& entry : _s_plain_names) {                                                \
        delete entry.second;                                                            \
    }                                                                                   \
}                                                                                       \
inline std::string rellaf_name() const override { return #_clazz_; }                    \
inline Model* create() const override { return (Model*)new(std::nothrow)_clazz_; }      \
inline Model* clone() const override {                                                  \
    Model* new_model = new(std::nothrow)_clazz_;                                        \
    *((_clazz_*)new_model) = *((_clazz_*)this);                                         \
    return (Model*)new_model;                                                           \
}                                                                                       \
static const std::map<std::string, Model*>& plain_names() {                             \
    return _s_plain_names;                                                              \
}                                                                                       \
static bool plain_concern(const std::string& key) {                                     \
    return _s_plain_names.count(key) == 1;                                              \
}                                                                                       \
static bool is_plain_default(const std::string& key, const std::string& val) {          \
    auto entry = _s_plain_names.find(key);                                              \
    return entry != _s_plain_names.end() ? entry->second->equal_parse(val) : false;     \
}                                                                                       \
inline bool is_plain_member(const std::string& key) const override {                    \
    return plain_concern(key);                                                          \
}                                                                                       \
inline bool set_plain(const std::string& key, const std::string& val_str) override {    \
    auto entry = _s_plain_names.find(key);                                              \
    if (entry == _s_plain_names.end()) { return false; }                                \
    auto val_entry = _plains.find(key);                                                 \
    if (val_entry != _plains.end()) {                                                   \
        val_entry->second->set_parse(val_str);                                          \
        return true;                                                                    \
    }                                                                                   \
    Model* plain = entry->second->create();                                             \
    plain->set_parse(val_str);                                                          \
    _plains.emplace(key, plain);                                                        \
    return true;                                                                        \
}                                                                                       \
protected:                                                                              \
static std::map<std::string, Model*> _s_plain_names;                                    \
private:                                                                                \
class Reg {                                                                             \
public:                                                                                 \
Reg(const std::string& name, _clazz_* inst, Model* dft) {                               \
    _clazz_::_s_plain_names.emplace(name, dft->clone());                                \
    inst->_plains.emplace(name, dft);                                                   \
}                                                                                       \
}

/////////////////////// type declaration //////////////////////

#define rellaf_model_dcl(_clazz_)                                                       \
RELLAF_MODEL_DCL_PLAIN(_clazz_);                                                        \
private:                                                                                \
class RegList {                                                                         \
public:                                                                                 \
    RegList(const std::string& name, _clazz_* inst) {                                   \
        _clazz_::_s_list_names.emplace(name);                                           \
        inst->_lists.emplace(name, List());                                             \
    }                                                                                   \
};                                                                                      \
class RegObject {                                                                       \
public:                                                                                 \
    explicit RegObject(const std::string& name) {                                       \
        _clazz_::_s_object_names.emplace(name);                                         \
    }                                                                                   \
};                                                                                      \
static std::set<std::string> _s_list_names;                                             \
static std::set<std::string> _s_object_names

/////////////////////// type definition without complex model ////////////////////
#define RELLAF_MODEL_DEF_PLAIN(_clazz_)                                                 \
std::map<std::string, Model*> _clazz_::_s_plain_names;                                  \
static volatile _clazz_ _s_force_init_##_clazz_

/////////////////////// type definition ////////////////////
#define rellaf_model_def(_clazz_)                                                       \
std::set<std::string> _clazz_::_s_list_names;                                           \
std::set<std::string> _clazz_::_s_object_names;                                         \
RELLAF_MODEL_DEF_PLAIN(_clazz_)

/////////////////////// plain ////////////////////
class Model {
public:
    virtual ~Model() = default;

    virtual inline const ModelType& rellaf_type() const {
        return _type;
    }

    virtual std::string rellaf_name() const {
        return "";
    }

    virtual Model* create() const = 0;

    virtual Model* clone() const = 0;

    virtual void set_parse(const std::string& val_str) = 0;

    virtual bool equal_parse(const std::string& val_str) const = 0;

    virtual std::string str() const = 0;

    virtual std::string debug_str() const {
        return "";
    }

protected:
    ModelType _type;
};

template<class T>
class Plain : public Model {
RELLAF_DEFMOVE_NO_CTOR_NOEXCEPT(Plain);

private:
    class TypeDetect {
    public:
        TypeDetect(Plain* inst, int16_t val) {
            inst->_type = ModelTypeEnum::e().INT;
            inst->_str_func = [](int val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, int val) {
            inst->_type = ModelTypeEnum::e().INT;
            inst->_str_func = [](int val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, int64_t val) {
            inst->_type = ModelTypeEnum::e().INT64;
            inst->_str_func = [](int64_t val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, uint16_t val) {
            inst->_type = ModelTypeEnum::e().UINT16;
            inst->_str_func = [](uint16_t val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, uint32_t val) {
            inst->_type = ModelTypeEnum::e().UINT32;
            inst->_str_func = [](uint32_t val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, uint64_t val) {
            inst->_type = ModelTypeEnum::e().UINT64;
            inst->_str_func = [](uint64_t val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, bool val) {
            inst->_type = ModelTypeEnum::e().FLOAT;
            inst->_str_func = [](float val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, float val) {
            inst->_type = ModelTypeEnum::e().FLOAT;
            inst->_str_func = [](float val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, double val) {
            inst->_type = ModelTypeEnum::e().DOUBLE;
            inst->_str_func = [](double val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, const std::string& val) {
            inst->_type = ModelTypeEnum::e().STR;
            inst->_str_func_ref = [](const std::string& val) {
                return val;
            };
        }
    };

public:
    explicit Plain() {
        TypeDetect td(this, T());
    }

    explicit Plain(const T& val) : _val(val) {
        TypeDetect td(this, T());
    }

    explicit Plain(std::function<T(const std::string&)> parse_func,
            std::function<std::string(T)> str_func) :
            _parse_func(parse_func),
            _str_func(str_func) {
        _type = ModelTypeEnum::e().no;
    }

    explicit Plain(std::function<T(const std::string&)> parse_func,
            std::function<std::string(const T&)> str_func_ref) :
            _parse_func(parse_func),
            _str_func_ref(str_func_ref) {
        _type = ModelTypeEnum::e().no;
    }

    inline Model* create() const override { return new(std::nothrow) Plain<T>(); }

    inline Model* clone() const override {
        Model* inst = create();
        if (inst != nullptr) {
            *inst = *this;
        }
        return inst;
    }

    inline virtual void set_parse_func(const std::function<T(const std::string&)>& parse_func) {
        _parse_func = parse_func;
    }

    inline virtual void set_str_func(const std::function<std::string(T)>& str_func) {
        _str_func = str_func;
    }

    inline virtual void set_str_func_ref(const std::function<std::string(const T&)>& str_func_ref) {
        _str_func_ref = str_func_ref;
    }

    inline virtual T value() const {
        return _val;
    }

    inline virtual void set(const T& val) {
        _val = val;
    }

    inline void set_parse(const std::string& val_str) override {
        if (_parse_func) {
            _val = _parse_func(val_str);
        } else {
            try {
                _val = cast<T>(val_str);
            } catch (...) {
                // do nothing
            }
        }
    }

    inline bool equal_parse(const std::string& val_str) const override {
        if (_parse_func) {
            return _val == _parse_func(val_str);
        } else {
            try {
                return _val = cast<T>(val_str);
            } catch (...) {
                // do nothing
            }
        }
        return false;
    }

    inline std::string str() const override {
        if (_str_func_ref) {
            return _str_func_ref(_val);
        }
        if (_str_func) {
            return _str_func(_val);
        }
        return "";
    }

    inline std::string debug_str() const override {
        return std::forward(str());
    }

protected:
    T _val;
    // parse val from string
    std::function<T(const std::string&)> _parse_func;

    // value to string
    std::function<std::string(T)> _str_func;
    std::function<std::string(const T&)> _str_func_ref;
};

/////////////////////// model list ////////////////////
class List : public Model {
RELLAF_DEFMOVE_NO_CTOR(List)

public:
    List() {
        _type = ModelTypeEnum::e().LIST;
    }

    virtual ~List();

    inline std::string debug_str() const override {
        return "";
    }

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

    Model* get(size_t idx);

    const Model* get(size_t idx) const;

    const Model* operator[](size_t idx) const;

    std::deque<Model*>::const_iterator begin() const;

    std::deque<Model*>::const_iterator end() const;

private:
    std::deque<Model*> _items;

private:
    // hide method
    Model* create() const override {
        return nullptr;
    }

    Model* clone() const override {
        return nullptr;
    }

    std::string str() const override {
        return "";
    }

    void set_parse(const std::string& val_str) override {
        RELLAF_UNUSED(val_str);
    }

    bool equal_parse(const std::string& val_str) const override {
        RELLAF_UNUSED(val_str);
        return false;
    }
};

/////////////////////// base model class ////////////////////
class Object : public Model {
RELLAF_DEFMOVE_NO_CTOR(Object)

public:
    Object() {
        _type = ModelTypeEnum::e().OBJECT;
    }

    virtual ~Object();

    virtual std::string rellaf_name() const override = 0;

    virtual Model* create() const override = 0;

    virtual Model* clone() const override = 0;

    virtual std::string debug_str() const;

    virtual bool is_plain_member(const std::string& key) const = 0;

    virtual bool set_plain(const std::string& key, const std::string& val_str) = 0;

    Model* get_plain(const std::string& key);

    const Model* get_plain(const std::string& key) const;

    inline const std::map<std::string, Model*>& get_plains() const {
        return _plains;
    }

    inline bool is_object_member(const std::string& name) const {
        return _objects.count(name) != 0;
    }

    inline const std::map<std::string, Object*>& get_objects() const {
        return _objects;
    }

    Object* get_object(const std::string& name);

    const Object* get_object(const std::string& name) const;

    inline bool is_list_member(const std::string& name) const {
        return _lists.count(name) != 0;
    }

    inline std::map<std::string, List>& get_lists() {
        return _lists;
    }

    inline const std::map<std::string, List>& get_lists() const {
        return _lists;
    }

    List& get_list(const std::string& name);

    const List& get_list(const std::string& name) const;

protected:
    std::map<std::string, Model*> _plains;
    std::map<std::string, List> _lists;
    std::map<std::string, Object*> _objects;

private:
    // hide method
    std::string str() const override {
        return "";
    }

    void set_parse(const std::string& val_str) override {
        RELLAF_UNUSED(val_str);
    }

    bool equal_parse(const std::string& val_str) const override {
        RELLAF_UNUSED(val_str);
        return false;
    }
};

#define a       \


/////////////////////// definition method ////////////////////
#define RELLAF_MODEL_DEF_type(_type_, _name_, _dft_)                                    \
public:                                                                                 \
    _type_ _name_() const {                                                             \
        return _plain_##_name_.value();                                                 \
    }                                                                                   \
    void set_##_name_(const _type_& val) {                                              \
        return _plain_##_name_.set(val);                                                \
    }                                                                                   \
private:                                                                                \
    Plain<_type_> _plain_##_name_{_dft_};                                               \
    Reg _reg_##_name_{#_name_, this, (Model*)(&_plain_##_name_)}

#define rellaf_model_def_int16(_name_, _dft_) RELLAF_MODEL_DEF_type(int16, _name_, _dft_)
#define rellaf_model_def_int(_name_, _dft_) RELLAF_MODEL_DEF_type(int, _name_, _dft_)
#define rellaf_model_def_int64(_name_, _dft_) RELLAF_MODEL_DEF_type(int64_t, _name_, _dft_)
#define rellaf_model_def_uint16(_name_, _dft_) RELLAF_MODEL_DEF_type(uint16_t, _name_, _dft_)
#define rellaf_model_def_uint32(_name_, _dft_) RELLAF_MODEL_DEF_type(uint32_t, _name_, _dft_)
#define rellaf_model_def_uint64(_name_, _dft_) RELLAF_MODEL_DEF_type(uint64_t, _name_, _dft_)
#define rellaf_model_def_bool(_name_, _dft_) RELLAF_MODEL_DEF_type(bool, _name_, _dft_)
#define rellaf_model_def_float(_name_, _dft_) RELLAF_MODEL_DEF_type(float, _name_, _dft_)
#define rellaf_model_def_double(_name_, _dft_) RELLAF_MODEL_DEF_type(double, _name_, _dft_)
#define rellaf_model_def_str(_name_, _dft_) RELLAF_MODEL_DEF_type(std::string, _name_, _dft_)

#define rellaf_model_def_object(_name_, _type_)             \
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
                entry->second = (Object*)val->clone();               \
            }                                               \
        } else {                                            \
            if (val != nullptr) {                           \
                _objects.emplace(#_name_, (Object*)val->clone());    \
            } else {                                        \
                _objects.emplace(#_name_, nullptr);         \
            }                                               \
        }                                                   \
    }                                                       \
private:                                                    \
    RegObject _reg_##_name_##_object{#_name_}


#define rellaf_model_def_list(_name_, _type_)               \
public:                                                     \
    List& _name_() {                                        \
        return get_list(#_name_);                           \
    }                                                       \
private:                                                    \
    RegList _reg_##_name_##_list{#_name_, this}

}