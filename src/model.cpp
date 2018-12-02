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

#include "model.h"

namespace rellaf {

ModelList::~ModelList() {
    clear();
}

std::string ModelList::debug_str() const {
    std::string buf = "[";
    for (auto _item : _items) {
        buf += _item->debug_str();
        buf += ", ";
    }
    if (buf.back() == ' ') {
        buf.pop_back();
        buf.back() = ']';
    } else {
        buf += ']';
    }
    return buf;
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
    if (model == nullptr) {
        _items.push_front(nullptr);
    } else {
        _items.push_front(model->clone());
    }
}

void ModelList::push_back(Model* model) {
    if (model == nullptr) {
        _items.push_back(nullptr);
    } else {
        _items.push_back(model->clone());
    }
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

void ModelList::set(size_t idx, Model* model) {
    if (idx >= _items.size()) {
        return;
    }
    delete _items[idx];
    _items[idx] = model->clone();
}

const Model* ModelList::operator[](size_t idx) const {
    return at(idx);
}

std::deque<Model*>::const_iterator ModelList::begin() const {
    return _items.begin();
}

std::deque<Model*>::const_iterator ModelList::end() const {
    return _items.end();
}

Object::~Object() {
    for (auto& entry : _objects) {
        delete entry.second;
    }
}

std::string Object::debug_str() const {
    std::string buf = "{";
    for (auto& key : get_plains()) {
        buf += key.first + ": " + key.second->str() + ", ";
    }
    for (auto& entry : get_lists()) {
        buf += entry.first + ": " + entry.second.debug_str() + ", ";
    }
    for (auto& entry : get_objects()) {
        buf += entry.first + ": " + entry.second->debug_str() + ", ";
    }
    if (buf.back() == ' ') {
        buf.pop_back();
        buf.back() = '}';
    } else {
        buf += '}';
    }
    return buf;
}

Model* Object::get_plain(const std::string& key) {
    if (!is_plain_member(key)) {
        return nullptr;
    }
    return _plains.find(key)->second;
}

const Model* Object::get_plain(const std::string& key) const {
    if (!is_plain_member(key)) {
        return nullptr;
    }
    return _plains.find(key)->second;
}

Object* Object::get_object(const std::string& name) {
    auto entry = _objects.find(name);
    return entry == _objects.end() ? nullptr : entry->second;
}

const Object* Object::get_object(const std::string& name) const {
    auto entry = _objects.find(name);
    return entry == _objects.end() ? nullptr : entry->second;
}

ModelList& Object::get_list(const std::string& name) {
    return _lists.at(name);
}

const ModelList& Object::get_list(const std::string& name) const {
    return _lists.at(name);
}

}