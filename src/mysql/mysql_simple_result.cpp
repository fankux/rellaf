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

#include "mysql/mysql_simple_result.h"

namespace rellaf {

MyResult::~MyResult() {
    reset();
}

void MyResult::reset() {
    _index = 0;
    _fields.clear();
    _row = nullptr;
    if (_res != nullptr) {
        mysql_free_result(_res);
        _res = nullptr;
    }
}

bool MyResult::init(void* context) {
    _res = static_cast<MYSQL_RES*>(context);
    if (_res == nullptr) {
        return false;
    }

    MYSQL_FIELD* fields_ptr = mysql_fetch_fields(_res);
    if (fields_ptr == nullptr) {
        return false;
    }

    uint32_t num = mysql_num_fields(_res);
    for (uint32_t i = 0; i < num; ++i) {
        std::string name(fields_ptr[i].name, fields_ptr[i].name_length);
        _fields.emplace_back(name);
    }
    return true;
}

void MyResult::operator()(MYSQL_RES* res) {
    reset();
    _res = res;
}

size_t MyResult::row_count() const {
    if (_res != nullptr) {
        return mysql_num_rows(_res);
    }
    return 0;
}

size_t MyResult::field_count() const {
    return _fields.size();
}

std::string MyResult::field_name(size_t index) const {
    assert(index < _fields.size());
    return _fields[index];
}

bool MyResult::next() {
    if (_index >= field_count()) {
        return false;
    }
    _index = (_index == 0 ? 0 : _index + 1);
    _row = mysql_fetch_row(_res);
    if (_row == nullptr) {
        reset();
        return false;
    }
    return true;
}

std::string MyResult::fetch(size_t index) const {
    assert(index < _fields.size());
    return _row[index];
}

bool MyResult::to_model(Model* model) const {
    if (field_count() == 0) {
        return true;
    }

    if (model->rellaf_type() == ModelTypeEnum::e().OBJECT) {

        for (size_t i = 0; i < field_count(); ++i) {
            const std::string& key = field_name(i);
            if (!((Object*)model)->set_plain(key, fetch(i))) {
                RELLAF_DEBUG("select impl set result key %s failed", key.c_str());
                return -1;
            }
        }

    } else if (is_plain(model)) {
        ((Model*)model)->set_parse(fetch(0));
    }

    return true;
}


}