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

#include "mysql/sql_executor.h"

namespace rellaf {

SqlResult::~SqlResult() {
    reset();
}

void SqlResult::reset() {
    _index = 0;
    if (_res != nullptr) {
        mysql_free_result(_res);
        _res = nullptr;
    }
}

bool SqlResult::init(void* context) {
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
        _fields.emplace_back(SqlField(name, fields_ptr[i].type));
    }
    return true;
}

size_t SqlResult::row_count() const {
    if (_res != nullptr) {
        return mysql_num_rows(_res);
    }
    return 0;
}

void SqlResult::operator()(MYSQL_RES* res) {
    reset();
    _res = res;
}

SqlRow SqlResult::fetch_row() {
    size_t index;
    std::deque<std::string> datas;
    if (gen_row(index, datas)) {
        return SqlRow{this, index, datas};
    }
    return SqlRow{nullptr, SIZE_MAX, {}};
}

bool SqlResult::gen_row(size_t& index, std::deque<std::string>& data) {
    if (_res == nullptr) {
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(_res);
    if (row == nullptr) {
        return false;
    }

    index = _index++;
    size_t field_count = mysql_num_fields(_res);
    for (size_t i = 0; i < field_count; ++i) {
        data.emplace_back(row[i]);
    }
    return true;
}

size_t SqlResult::field_index(const std::string& name) const {
    for (size_t i = 0; i < _fields.size(); ++i) {
        if (_fields[i].name() == name) {
            return i;
        }
    }
    return SIZE_MAX;
}

size_t SqlResult::field_count() const {
    return _fields.size();
}

std::string SqlResult::field_name(size_t index) const {
    assert(index < _fields.size());
    return _fields[index].name();
}


} // namespace rellaf
