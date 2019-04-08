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
#include "model.h"

namespace rellaf {

class SqlResult {
public:
    virtual ~SqlResult() = default;

    virtual size_t row_count() const = 0;

    virtual size_t field_count() const = 0;

    /**
     * @brief fetch filed name through specific index
     */
    virtual std::string field_name(size_t index) const = 0;

    /**
     * @brief determine if there is next row exist, move index to next if exist.
     */
    virtual bool next() = 0;

    /**
     * @brief fetch field value through `index` to the `value` at current index row
     */
    virtual std::string fetch(size_t index) const = 0;

    /**
     * @brief convert current index row to model
     * @return if success
     */
    virtual bool to_model(Model* model) const = 0;
};

class SqlExecutor {
public:
    /**
     * @brief execute select action to mysql
     * @return result set, memory resource MUST be clean after using by caller
     */
    virtual SqlResult* select(const std::string& sql) = 0;

    /**
     * @brief   execute sql
     * @return  affected rows count
     */
    virtual int execute(const std::string& sql) = 0;
};

} // namespace rellaf