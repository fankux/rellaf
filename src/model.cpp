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

Object::~Object() {
    for (auto& entry : _objects) {
        delete entry.second;
    }
}

std::string Object::debug_str() const {
    std::stringstream ss;
    for (auto& key : get_plains()) {
        ss << key.first << ":" << key.second->str() << ",";
    }
    for (auto& entry : get_lists()) {
        ss << entry.first << ":[";
        const List& list = get_list(entry.first);
        for (auto& item : list) {
            ss << "{" << item->debug_str() << "}";
        }
        ss << "],";
    }
    for (auto& entry : get_objects()) {
        ss << entry.first << ":{" << entry.second->debug_str() << "},";
    }
    std::string str = ss.str();
    return str.empty() ? "" : (str.pop_back(), str);
}

Model* Object::get_plain(const std::string& key) {
    if (!is_plain_member(key)) {
        return nullptr;
    }
    return _plains[key];
}

const Model* Object::get_plain(const std::string& key) const {
    if (!is_plain_member(key)) {
        return nullptr;
    }
    return _plains[key];
}

Object* Object::get_object(const std::string& name) {
    auto entry = _objects.find(name);
    return entry == _objects.end() ? nullptr : entry->second;
}

const Object* Object::get_object(const std::string& name) const {
    auto entry = _objects.find(name);
    return entry == _objects.end() ? nullptr : entry->second;
}

List& Object::get_list(const std::string& name) {
    return _lists.find(name)->second;
}

const List& Object::get_list(const std::string& name) const {
    return _lists.find(name)->second;
}

List::~List() {
    clear();
}

size_t List::size() const {
    return _items.size();
}

bool List::empty() const {
    return _items.empty();
}

void List::clear() {
    for (auto& item : _items) {
        delete item;
    }
    _items.clear();
}

void List::push_front(Model* model) {
    _items.push_front(model == nullptr ? nullptr : model->clone());
}

void List::push_back(Model* model) {
    _items.push_back(model == nullptr ? nullptr : model->clone());
}

void List::pop_front() {
    if (!_items.empty()) {
        delete _items.front();
    }
    _items.pop_front();
}

void List::pop_back() {
    if (!_items.empty()) {
        delete _items.back();
    }
    _items.pop_back();
}

Model* List::front() {
    return _items.front();
}

const Model* List::front() const {
    return _items.front();
}

Model* List::back() {
    return _items.back();
}

const Model* List::back() const {
    return _items.back();
}

void List::set(size_t idx, Model* model) {
    if (idx >= _items.size()) {
        return;
    }
    delete _items[idx];
    _items[idx] = model->clone();
}

Model* List::get(size_t idx) {
    if (idx >= _items.size()) {
        return nullptr;
    }
    return _items[idx];
}

const Model* List::get(size_t idx) const {
    if (idx >= _items.size()) {
        return nullptr;
    }
    return _items[idx];
}

const Model* List::operator[](size_t idx) const {
    return get(idx);
}

std::deque<Model*>::const_iterator List::begin() const {
    return _items.begin();
}

std::deque<Model*>::const_iterator List::end() const {
    return _items.end();
}


}