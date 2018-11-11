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


#include "mysql_escape.h"
#include "common.h"
#include "sql_model_tool.h"

namespace rellaf {

static bool sql_field_escape(const std::string& field, std::string& out) {
    out.clear();

    char field_buf[1024];
    bool alloc_flag = false;
    char* p = field_buf;
    if (field.size() > 768) { // rest space(1024-768-1) for blackslash added after escaped
        p = (char*)malloc((size_t)(field.size() * 1.5));
        if (p == nullptr) {
            return false;
        }
        alloc_flag = true;
    }
    p[0] = '\0';
    SqlEscape::instance("utf8").escape_field(field, p);

    out = p;
    if (alloc_flag) {
        free(p);
    }
    return true;
}

void model_to_sql_vals(Model* model, std::map<std::string, std::string>& vals) {
    vals.clear();

    std::string escape_field;

    for (auto& entry : model->ints()) {
        vals[entry.first] = std::to_string(entry.second);
    }
    for (auto& entry : model->uint32s()) {
        vals[entry.first] = std::to_string(entry.second);
    }
    for (auto& entry : model->bools()) {
        vals[entry.first] = std::to_string(entry.second);
    }
    for (auto& entry : model->doubles()) {
        vals[entry.first] = std::to_string(entry.second);
    }
    for (auto& entry : model->strs()) {
        if (!sql_field_escape(entry.second, escape_field)) {
            RELLAF_DEBUG("escape sql val failed of field : %s", entry.second.c_str());
            continue;
        }

        escape_field.insert(0, "'");
        escape_field.push_back('\'');
        vals[entry.first] = escape_field;
    }
}

}