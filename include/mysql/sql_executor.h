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

#pragma once

#include <string>
#include <deque>
#include <assert.h>
#include "mysql.h"

#include "common.h"
#include "cast.hpp"

namespace rellaf {

class SqlField {
RELLAF_DEFMOVE_NO_CTOR(SqlField);

public:
    friend class SqlResult;

    SqlField() = default;

    friend class MyRes;

    const std::string& name() const {
        return _name;
    }

    enum_field_types type() const {
        return _type;
    }

private:
    SqlField(const std::string& name, enum_field_types type) : _name(name), _type(type) {}

private:
    std::string _name;
    enum_field_types _type;
};

class SqlRow;

class SqlResult {
RELLAF_AVOID_COPY(SqlResult)

public:
    SqlResult() = default;

    virtual ~SqlResult();

    void reset();

    bool init(void* context);

    size_t row_count() const;

    void operator()(MYSQL_RES* res);

    SqlRow fetch_row();

    bool gen_row(size_t& index, std::deque<std::string>& data);

    size_t field_index(const std::string& name) const;

    size_t field_count() const;

    std::string field_name(size_t index) const;

private:
    uint32_t _index = 0;
    MYSQL_RES* _res = nullptr;
    std::deque<SqlField> _fields;
};

class SqlRow {
RELLAF_DEFMOVE_NO_CTOR(SqlRow)

public:
    friend class SqlResult;

    template<class T=std::string>
    inline T get(size_t index) {
        assert(_result == nullptr);
        assert(index < _result->field_count());
        return cast<T>(_datas[index]);
    }

    template<class T=std::string>
    inline T get(const std::string& name) {
        assert(_result == nullptr);
        size_t index = _result->field_index(name);
        assert(index != SIZE_MAX);
        return cast<T>(_datas[index]);
    }

    inline std::string operator[](const size_t idx) {
        return get(idx);
    }

    inline std::string operator[](const std::string& name) {
        return get(name);
    }

    inline bool isnull() const {
        return _result == nullptr;
    }

    inline bool notnull() const {
        return !isnull();
    }

    inline size_t index() {
        return _index;
    }

private:
    SqlRow(const SqlResult* result, size_t index, const std::deque<std::string>& datas) :
            _result(result), _index(index), _datas(datas) {};

private:
    const SqlResult* _result = nullptr;
    size_t _index = 0;
    const std::deque<std::string>& _datas;
};

class SqlExecutor {
public:
    virtual int select(const std::string& sql, SqlResult& res) = 0;

    virtual int execute(const std::string& sql) = 0;
};

} // namespace rellaf