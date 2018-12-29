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

Model* Void::create() const {
    return new(std::nothrow) Void;
}

Model* Void::clone() const {
    return create();
}

List::~List() {
    clear();
}

List::List() : Model() {
    _type = ModelTypeEnum::e().LIST;
}

List::List(const List& o) : Model() {
    _type = ModelTypeEnum::e().LIST;
    assign(&o);
}

List::List(List&& o) noexcept  : Model() {
    _type = ModelTypeEnum::e().LIST;
    assign(&o);
    o.clear();
}

List& List::operator=(const List& o) {
    _type = ModelTypeEnum::e().LIST;
    assign(&o);
    return *this;
}

List& List::operator=(List&& o) noexcept {
    _type = ModelTypeEnum::e().LIST;
    assign(&o);
    o.clear();
    return *this;
}

std::string List::debug_str() const {
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
    if (model == nullptr) {
        _items.push_front(nullptr);
    } else {
        _items.push_front(model->clone());
    }
}

void List::push_back(Model* model) {
    if (model == nullptr) {
        _items.push_back(nullptr);
    } else {
        _items.push_back(model->clone());
    }
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

void List::set(size_t idx, Model* model) {
    if (idx >= _items.size()) {
        return;
    }
    delete _items[idx];
    _items[idx] = model == nullptr ? nullptr : model->clone();
}

const Model* List::operator[](size_t idx) const {
    return at(idx);
}

std::deque<Model*>::const_iterator List::begin() const {
    return _items.begin();
}

std::deque<Model*>::const_iterator List::end() const {
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

List& Object::get_list(const std::string& name) {
    return _lists.at(name);
}

const List& Object::get_list(const std::string& name) const {
    return _lists.at(name);
}

bool is_plain(const Model* model) {
    return (model->rellaf_type() != ModelTypeEnum::e().no &&
            model->rellaf_type() != ModelTypeEnum::e().OBJECT &&
            model->rellaf_type() != ModelTypeEnum::e().LIST);
}

bool is_list(const Model* model) {
    return (model->rellaf_type() == ModelTypeEnum::e().LIST);
}

bool is_object(const Model* model) {
    return (model->rellaf_type() == ModelTypeEnum::e().OBJECT);
}

}