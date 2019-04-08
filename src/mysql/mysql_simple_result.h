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

#include "mysql.h"

#include "common.h"
#include "cast.hpp"
#include "mysql/sql_executor.h"

namespace rellaf {

class MyResult : public SqlResult {
RELLAF_AVOID_COPY(MyResult)

public:
    MyResult() = default;

    ~MyResult() override;

    void reset();

    bool init(void* context);

    void operator()(MYSQL_RES* res);

    size_t row_count() const override;

    size_t field_count() const override;

    std::string field_name(size_t index) const override;

    bool next() override;

    std::string fetch(size_t index) const override;

    bool to_model(Model* model) const override;

private:
    uint32_t _index = 0;
    MYSQL_RES* _res = nullptr;
    MYSQL_ROW _row = nullptr;
    std::deque<std::string> _fields;
};

}