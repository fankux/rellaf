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
//

#pragma once

#include "mysql_simple_pool.h"
#include "dao.h"

namespace rellaf {

int select_func(const std::string& sql, DaoResultList& rets) {

    SqlRes res(MysqlSimplePool::instance().query(sql));
    if (!res.good()) {
        return -1;
    }

    std::deque<SqlField> fields;
    int row_count = static_cast<int>(res.row_count());
    if (res.row_count() > 0) {
        res.fetch_fields(fields);
    } else {
        return 0;
    }
    if (res.field_count() != fields.size()) {
        RELLAF_DEBUG("IMPOSIBLE!!! result set field count not match, sql : %s", sql.c_str());
        return -1;
    }

    SqlRow row;
    while ((row = res.fetch_row()).isnotnull()) {
        DaoResultRow dao_row;
        for (size_t i = 0; i < fields.size(); ++i) {
            dao_row.set(fields[i].name(), row.get(i));
        }
        rets.push(dao_row);
    }

    return row_count;
}

}