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
#include <type_traits>
#include "cast.hpp"
#include "common.h"
#include "model_type.h"

namespace rellaf {

/////////////////////// type declaration without complex model //////////////////////
#define RELLAF_MODEL_DCL_PLAIN(_clazz_)                                                 \
public:                                                                                 \
_clazz_() : Object() {}                                                                 \
virtual ~_clazz_() = default;                                                           \
_clazz_(const _clazz_& o) : Object() {                                                  \
    assign(&o);                                                                         \
}                                                                                       \
_clazz_(_clazz_&& o) noexcept : Object() {                                              \
    assign(&o);                                                                         \
    o.clear();                                                                          \
}                                                                                       \
_clazz_& operator=(const _clazz_& o) {                                                  \
    _type = o._type;                                                                    \
    assign(&o);                                                                         \
    return *this;                                                                       \
}                                                                                       \
_clazz_& operator=(_clazz_&& o) noexcept {                                              \
    _type = o._type;                                                                    \
    assign(&o);                                                                         \
    clear();                                                                            \
    return *this;                                                                       \
}                                                                                       \
inline std::string rellaf_name() const override { return #_clazz_; }                    \
inline Model* create() const override { return (Model*)new(std::nothrow)_clazz_; }      \
inline Model* clone() const override {                                                  \
    Model* new_model = new(std::nothrow) _clazz_;                                       \
    *((_clazz_*)new_model) = *this;                                                     \
    return (Model*)new_model;                                                           \
}                                                                                       \
inline void assign(const Model* val) override {                                         \
    if (rellaf_type() != val->rellaf_type()) {                                          \
        return;                                                                         \
    }                                                                                   \
    _clazz_* ptr = (_clazz_*)val;                                                       \
    for (auto& entry : _plains) {                                                       \
        entry.second->assign(ptr->_plains[entry.first]);                                \
    }                                                                                   \
    for (auto& entry : _objects) {                                                      \
        entry.second->assign(ptr->_objects[entry.first]);                               \
    }                                                                                   \
    for (auto& entry : _lists) {                                                        \
        entry.second.assign(&(ptr->_lists[entry.first]));                               \
    }                                                                                   \
}                                                                                       \
inline void clear() override {                                                          \
    for (auto& entry : _plains) {                                                       \
        entry.second->clear();                                                          \
    }                                                                                   \
    for (auto& entry : _objects) {                                                      \
        entry.second->clear();                                                          \
    }                                                                                   \
    _objects.clear();                                                                   \
    for (auto& entry : _lists) {                                                        \
        entry.second.clear();                                                           \
    }                                                                                   \
    _lists.clear();                                                                     \
}                                                                                       \
static const std::map<std::string, Model*>& plain_names() {                             \
    return _s_plain_names;                                                              \
}                                                                                       \
static bool plain_concern(const std::string& key) {                                     \
    return _s_plain_names.count(key) == 1;                                              \
}                                                                                       \
static bool is_plain_default(const std::string& key, const std::string& val) {          \
    auto entry = _s_plain_names.find(key);                                              \
    if (entry != _s_plain_names.end()) {                                                \
        return entry->second->equal_parse(val);                                         \
    }                                                                                   \
    return false;                                                                       \
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
Reg(const std::string& name, _clazz_* inst, Model* val, Model* dft) {                   \
    _clazz_::_s_plain_names.emplace(name, dft);                                         \
    inst->_plains.emplace(name, val);                                                   \
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
    explicit RegObject(const std::string& name, _clazz_* inst) {                        \
        _clazz_::_s_object_names.emplace(name);                                         \
        inst->_objects.emplace(name, nullptr);                                          \
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

/////////////////////// basic ////////////////////
// plain default
class Model {
public:
    Model() = default;

    virtual ~Model() = default;

    virtual inline const ModelType& rellaf_type() const {
        return _type;
    }

    virtual std::string rellaf_name() const {
        return "";
    }

    /**
     * markup current model with a tag which is Enum
     */
    template<class T=Model>
    T& tag(const std::string& tag_str) {
        _tag = tag_str;
        return *((T*)this);
    }

    const std::string& rellaf_tag() const {
        return _tag;
    }

    virtual Model* create() const = 0;

    virtual Model* clone() const = 0;

    virtual void assign(const Model* val) = 0;

    virtual void clear() = 0;

    /**
     * @brief pass a string that parsing to value
     */
    virtual bool set_parse(const std::string& val_str) = 0;

    /**
     * @brief if this equal to value which parsed from string `val_str`
     */
    virtual bool equal_parse(const std::string& val_str) = 0;

    virtual std::string str() const = 0;

    virtual std::string debug_str() const = 0;

protected:
    ModelType _type;
    std::string _tag;
};

class Void : public Model {
public:
    Void() {
        _type = ModelTypeEnum::e().no;
    }

    Model* create() const override;

    Model* clone() const override;

private:
    void assign(const Model* val) override {}

    void clear() override {}

    bool set_parse(const std::string& val_str) override {
        return true;
    }

    bool equal_parse(const std::string& val_str) override {
        return true;
    }

    std::string str() const override {
        return "";
    }

    std::string debug_str() const override {
        return "";
    }
};

template<class T>
class Plain : public Model {
public:
    Plain() {
        TypeDetect td(this, T());
    }

    Plain(const T& val) : _val(val) {
        TypeDetect td(this, T());
    }

    Plain(const Plain& o) {
        TypeDetect td(this, T());
        assign(&o);
    }

    Plain(Plain&& o) noexcept {
        TypeDetect td(this, T());
        assign(&o);
        o.clear();
    }

    Plain& operator=(const Plain& o) {
        TypeDetect td(this, T());
        assign(&o);
        return *this;
    }

    Plain& operator=(Plain&& o) noexcept {
        TypeDetect td(this, T());
        assign(&o);
        o.clear();
        return *this;
    }

    Plain(std::function<T(const std::string&)> parse_func,
            std::function<std::string(const T&)> str_func) :
            _parse_func(parse_func),
            _str_func(str_func) {
        _type = ModelTypeEnum::e().no;
    }

    inline Model* create() const override {
        return new(std::nothrow) Plain<T>();
    }

    inline Model* clone() const override {
        Model* inst = create();
        if (inst != nullptr) {
            *((Plain <T>*)inst) = *((Plain < T > *)
            this);
        }
        return inst;
    }

    inline void assign(const Model* val) override {
        if (rellaf_type() != val->rellaf_type()) {
            return;
        }
        Plain* ptr = (Plain*)val;
        _val = ptr->_val;
        _parse_func = ptr->_parse_func;
        _str_func = ptr->_str_func;
    }

    inline void clear() override {}

    inline void set_parse_func(const std::function<T(const std::string&)>& parse_func) {
        _parse_func = parse_func;
    }

    inline void set_str_func(const std::function<std::string(const T&)>& str_func) {
        _str_func = str_func;
    }

    inline T value() const {
        return _val;
    }

    inline void set(const T& val) {
        _val = val;
    }

    /**
     * @brief pass a string that parsing to value
     */
    inline bool set_parse(const std::string& val_str) override {
        if (_parse_func) {
            _val = _parse_func(val_str);
            return true;
        } else {
            try {
                _val = cast<T>(val_str);
                return true;
            } catch (...) {
                // do nothing
            }
        }
        return false;
    }

    /**
     * @brief if this equal to value which parsed from string `val_str`
     */
    inline bool equal_parse(const std::string& val_str) override {
        if (_parse_func) {
            return _val == _parse_func(val_str);
        } else {
            try {
                return _val == cast<T>(val_str);
            } catch (...) {
                // do nothing
            }
        }
        return false;
    }

    inline std::string str() const override {
        if (_str_func) {
            return _str_func(_val);
        }
        return "";
    }

    inline std::string debug_str() const override {
        return std::forward<std::string>(str());
    }

private:
    class TypeDetect {
    public:
        TypeDetect(Plain* inst, char val) {
            inst->_type = ModelTypeEnum::e().CHAR;
            inst->_str_func = [](const char& val) {
                std::string str = std::string();
                str += val;
                return str;
            };
        }

        TypeDetect(Plain* inst, int16_t val) {
            inst->_type = ModelTypeEnum::e().INT16;
            inst->_str_func = [](const int16_t& val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, int val) {
            inst->_type = ModelTypeEnum::e().INT;
            inst->_str_func = [](const int& val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, int64_t val) {
            inst->_type = ModelTypeEnum::e().INT64;
            inst->_str_func = [](const int64_t& val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, uint16_t val) {
            inst->_type = ModelTypeEnum::e().UINT16;
            inst->_str_func = [](const uint16_t& val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, uint32_t val) {
            inst->_type = ModelTypeEnum::e().UINT32;
            inst->_str_func = [](const uint32_t& val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, uint64_t val) {
            inst->_type = ModelTypeEnum::e().UINT64;
            inst->_str_func = [](const uint64_t& val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, bool val) {
            inst->_type = ModelTypeEnum::e().BOOL;
            inst->_str_func = [](const bool& val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, float val) {
            inst->_type = ModelTypeEnum::e().FLOAT;
            inst->_str_func = [](const float& val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, double val) {
            inst->_type = ModelTypeEnum::e().DOUBLE;
            inst->_str_func = [](const double& val) {
                return std::to_string(val);
            };
        }

        TypeDetect(Plain* inst, const std::string& val) {
            inst->_type = ModelTypeEnum::e().STR;
            inst->_str_func = [](const std::string& val) {
                return val;
            };
        }
    };

protected:
    T _val;
    // parse val from string
    std::function<T(const std::string&)> _parse_func;

    // value to string
    std::function<std::string(const T&)> _str_func;
};

/////////////////////// model list ////////////////////
class List : public Model {
public:
    ~List() override;

    List();

    List(const List& o);

    List(List&& o) noexcept;

    List& operator=(const List& o);

    List& operator=(List&& o) noexcept;

    inline Model* create() const override {
        return new(std::nothrow) List();
    }

    inline Model* clone() const override {
        List* inst = (List*)create();
        *inst = *this;
        return inst;
    }

    inline void assign(const Model* val) override {
        if (rellaf_type() != val->rellaf_type()) {
            return;
        }

        List* ptr = (List*)val;
        clear();
        for (auto& entry : ptr->_items) {
            Model* m = entry->clone();
            _items.push_back(m);
        }
    }

    std::string debug_str() const override;

    size_t size() const;

    bool empty() const;

    void clear() override;

    void push_front(Model* model);

    template<class T>
    void push_front(const T& model) {
        static_assert(std::is_base_of<Model, T>::value, "type not Model");
        _items.push_front(model.clone());
    }

    // FIXME.. mac compile over write
    void push_back(Model* model);

    template<class T>
    void push_back(const T& model) {
        static_assert(std::is_base_of<Model, T>::value, "type not Model");
        _items.push_back(model.clone());
    }

    void pop_front();

    void pop_back();

    template<class T=Model>
    T* front() {
        static_assert(std::is_base_of<Model, T>::value, "class not model");
        return (T*)(_items.front());
    }

    template<class T=Model>
    const T* front() const {
        static_assert(std::is_base_of<Model, T>::value, "class not model");
        return (T*)(_items.front());
    }

    template<class T=Model>
    T* back() {
        static_assert(std::is_base_of<Model, T>::value, "class not model");
        return (T*)_items.back();
    }

    template<class T=Model>
    const T* back() const {
        static_assert(std::is_base_of<Model, T>::value, "class not model");
        return (T*)_items.back();
    }

    void set(size_t idx, Model* model);

    template<class T=Model>
    T* at(size_t idx) {
        static_assert(std::is_base_of<Model, T>::value, "class not model");
        if (idx >= _items.size()) {
            return nullptr;
        }
        return (T*)(_items.at(idx));
    }

    template<class T=Model>
    const T* at(size_t idx) const {
        static_assert(std::is_base_of<Model, T>::value, "class not model");
        if (idx >= _items.size()) {
            return nullptr;
        }
        return (T*)(_items.at(idx));
    }

    const Model* operator[](size_t idx) const;

    std::deque<Model*>::const_iterator begin() const;

    std::deque<Model*>::const_iterator end() const;

private:
    std::deque<Model*> _items;

private:
    // hide method
    std::string str() const override {
        return "";
    }

    bool set_parse(const std::string& val_str) override {
        RELLAF_UNUSED(val_str);
        return false;
    }

    bool equal_parse(const std::string& val_str) override {
        RELLAF_UNUSED(val_str);
        return false;
    }
};

/////////////////////// base model class ////////////////////
class Object : public Model {
public:
    virtual ~Object();

    Object() {
        _type = ModelTypeEnum::e().OBJECT;
    }

    virtual std::string rellaf_name() const override = 0;

    virtual Model* create() const override = 0;

    virtual Model* clone() const override = 0;

    virtual void assign(const Model* val) override = 0;

    virtual void clear() override = 0;

    virtual std::string debug_str() const override;

    virtual bool is_plain_member(const std::string& key) const = 0;

    virtual bool set_plain(const std::string& key, const std::string& val_str) = 0;

    template<class T>
    Plain<T>* get_plain(const std::string& key) {
        if (!is_plain_member(key)) {
            return nullptr;
        }
        return (Plain < T > *)(_plains.find(key)->second);
    }

    template<class T>
    const Plain<T>* get_plain(const std::string& key) const {
        if (!is_plain_member(key)) {
            return nullptr;
        }
        return (Plain < T > *)(_plains.find(key)->second);
    }

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

    bool set_parse(const std::string& val_str) override {
        RELLAF_UNUSED(val_str);
        return false;
    }

    bool equal_parse(const std::string& val_str) override {
        RELLAF_UNUSED(val_str);
        return false;
    }
};

/////////////////////// definition method ////////////////////
#define RELLAF_MODEL_DEF_type(_type_, _sign_, _name_, _dft_)                            \
public:                                                                                 \
    _type_ _name_() const {                                                             \
        return _plain_##_name_.value();                                                 \
    }                                                                                   \
    void set_##_name_(const _type_& val) {                                              \
        return _plain_##_name_.set(val);                                                \
    }                                                                                   \
    _type_ _name_##_default() const {                                                   \
        return _sign_##_value(_s_plain_names[#_name_]);                                 \
    }                                                                                   \
    static _type_ _sign_##_value(Model* ptr) {                                          \
        return ((Plain<_type_>*)ptr)->value();                                          \
    }                                                                                   \
private:                                                                                \
    Plain<_type_> _plain_##_name_{_dft_};                                               \
    Plain<_type_> _plain_##_name_##_dft{_dft_};                                         \
    Reg _reg_##_name_{#_name_, this, (Model*)(&_plain_##_name_), (Model*)(&_plain_##_name_##_dft)}

#define rellaf_model_def_char(_name_, _dft_) RELLAF_MODEL_DEF_type(char, char, _name_, _dft_)
#define rellaf_model_def_int16(_name_, _dft_) RELLAF_MODEL_DEF_type(int16_t, int16, _name_, _dft_)
#define rellaf_model_def_int(_name_, _dft_) RELLAF_MODEL_DEF_type(int, int, _name_, _dft_)
#define rellaf_model_def_int64(_name_, _dft_) RELLAF_MODEL_DEF_type(int64_t, int64, _name_, _dft_)
#define rellaf_model_def_uint16(_name_, _dft_) RELLAF_MODEL_DEF_type(uint16_t, uint16, _name_, _dft_)
#define rellaf_model_def_uint32(_name_, _dft_) RELLAF_MODEL_DEF_type(uint32_t, uint32, _name_, _dft_)
#define rellaf_model_def_uint64(_name_, _dft_) RELLAF_MODEL_DEF_type(uint64_t, uint64, _name_, _dft_)
#define rellaf_model_def_bool(_name_, _dft_) RELLAF_MODEL_DEF_type(bool, bool, _name_, _dft_)
#define rellaf_model_def_float(_name_, _dft_) RELLAF_MODEL_DEF_type(float, float, _name_, _dft_)
#define rellaf_model_def_double(_name_, _dft_) RELLAF_MODEL_DEF_type(double, double, _name_, _dft_)
#define rellaf_model_def_str(_name_, _dft_) RELLAF_MODEL_DEF_type(std::string, str, _name_, _dft_)

#define rellaf_model_def_object(_name_, _type_)                         \
public:                                                                 \
    inline _type_* _name_() {                                           \
        return (_type_*)get_object(#_name_);                            \
    }                                                                   \
    inline _type_* _name_() const {                                     \
        return (_type_*)get_object(#_name_);                            \
    }                                                                   \
    inline void set_##_name_(_type_* val) {                             \
        auto entry = _objects.find(#_name_);                            \
        if (entry != _objects.end()) {                                  \
            delete entry->second;                                       \
            entry->second = nullptr;                                    \
            if (val != nullptr) {                                       \
                entry->second = (Object*)val->clone();                  \
            }                                                           \
        } else {                                                        \
            if (val != nullptr) {                                       \
                _objects.emplace(#_name_, (Object*)val->clone());       \
            } else {                                                    \
                _objects.emplace(#_name_, nullptr);                     \
            }                                                           \
        }                                                               \
    }                                                                   \
private:                                                                \
    RegObject _reg_##_name_##_object{#_name_, this}


#define rellaf_model_def_list(_name_, _type_)                           \
public:                                                                 \
    inline List& _name_() {                                             \
        return get_list(#_name_);                                       \
    }                                                                   \
    inline const ModelType& _name_##_list_type() const {                \
        return _name_##_type.rellaf_type();                             \
    }                                                                   \
private:                                                                \
    RegList _reg_##_name_##_list{#_name_, this};                        \
    _type_ _name_##_type

bool is_plain(const Model* model);

bool is_list(const Model* model);

bool is_object(const Model* model);

} //namespace
