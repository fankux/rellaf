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

#include <sstream>
#include "model.h"

namespace rellaf {

RELLAF_MODEL_DEF_TYPE(Plain, int, int);
RELLAF_MODEL_DEF_TYPE(Plain, int64_t, int64);
RELLAF_MODEL_DEF_TYPE(Plain, uint16_t, uint16);
RELLAF_MODEL_DEF_TYPE(Plain, uint32_t, uint32);
RELLAF_MODEL_DEF_TYPE(Plain, uint64_t, uint64);
RELLAF_MODEL_DEF_TYPE(Plain, bool, bool);
RELLAF_MODEL_DEF_TYPE(Plain, float, float);
RELLAF_MODEL_DEF_TYPE(Plain, double, double);
RELLAF_MODEL_DEF_TYPE(Plain, std::string, str);

Model::~Model() {
    for (auto& entry : _objects) {
        delete entry.second;
    }
}

std::string Model::str() const {
    std::stringstream ss;
    for (auto& key : get_int_names()) {
        ss << key.first << ":" << get_int(key.first) << ",";
    }
    for (auto& key : get_uint32_names()) {
        ss << key.first << ":" << get_int(key.first) << ",";
    }
    for (auto& key : get_bool_names()) {
        ss << key.first << ":" << get_int(key.first) << ",";
    }
    for (auto& key : get_double_names()) {
        ss << key.first << ":" << get_int(key.first) << ",";
    }
    for (auto& key : get_str_names()) {
        ss << key.first << ":" << get_str(key.first) << ",";
    }
    for (auto& entry : get_lists()) {
        ss << entry.first << ":[";
        const ModelList& list = get_list(entry.first);
        for (auto& item : list) {
            ss << "{" << item->str() << "}";
        }
        ss << "],";
    }
    for (auto& entry : get_objects()) {
        ss << entry.first << ":{" << entry.second->str() << "},";
    }
    std::string str = ss.str();
    return str.empty() ? "" : (str.pop_back(), str);
}

Model* Model::get_object(const std::string& name) {
    auto entry = _objects.find(name);
    return entry == _objects.end() ? nullptr : entry->second;
}

const Model* Model::get_object(const std::string& name) const {
    auto entry = _objects.find(name);
    return entry == _objects.end() ? nullptr : entry->second;
}

ModelList& Model::get_list(const std::string& name) {
    return _lists.find(name)->second;
}

const ModelList& Model::get_list(const std::string& name) const {
    return _lists.find(name)->second;
}

ModelList::~ModelList() {
    clear();
}

size_t ModelList::size() const {
    return _items.size();
}

bool ModelList::empty() const {
    return _items.empty();
}

void ModelList::clear() {
    for (auto& item : _items) {
        delete item;
    }
    _items.clear();
}

void ModelList::push_front(Model* model) {
    _items.push_front(model == nullptr ? nullptr : model->clone());
}

void ModelList::push_back(Model* model) {
    _items.push_back(model == nullptr ? nullptr : model->clone());
}

void ModelList::pop_front() {
    if (!_items.empty()) {
        delete _items.front();
    }
    _items.pop_front();
}

void ModelList::pop_back() {
    if (!_items.empty()) {
        delete _items.back();
    }
    _items.pop_back();
}

Model* ModelList::front() {
    return _items.front();
}

const Model* ModelList::front() const {
    return _items.front();
}

Model* ModelList::back() {
    return _items.back();
}

const Model* ModelList::back() const {
    return _items.back();
}

void ModelList::set(size_t idx, Model* model) {
    if (idx >= _items.size()) {
        return;
    }
    delete _items[idx];
    _items[idx] = model;
}

const Model* ModelList::operator[](int idx) const {
    return _items[idx];
}

std::deque<Model*>::const_iterator ModelList::begin() const {
    return _items.begin();
}

std::deque<Model*>::const_iterator ModelList::end() const {
    return _items.end();
}


}