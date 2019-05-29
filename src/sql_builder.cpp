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
// sql builder framework
//

#include <assert.h>
#include "sql_builder.h"

namespace rellaf {

rellaf_enum_def(SqlBuilder::Charset);

SqlExecutor* SqlBuilder::_executor = nullptr;

void SqlBuilder::set_executor(SqlExecutor* executor) {
    _executor = executor;
}

void SqlBuilder::split_section(const std::string& section_str, std::deque<std::string>& sections) {
    sections.clear();
    if (section_str.empty()) {
        return;
    }

    char* begin = const_cast<char*>(section_str.c_str());
    do {
        const char* pos = strchr(begin, '.');
        if (pos != nullptr) {
            if (pos == begin) {
                begin = const_cast<char*>(pos + 1);
                continue;
            }
            sections.emplace_back(std::string(begin, pos - begin));
            begin = const_cast<char*>(pos + 1);
        } else {
            sections.emplace_back(std::string(section_str, begin - section_str.c_str()));
            break;
        }
    } while (*begin != '\0');
}

bool SqlBuilder::get_plain_val_str(const Model* model, std::string& val,
        bool& need_quote, bool& need_escape) {
    switch (model->rellaf_type().code) {
        case ModelTypeEnum::INT16_code:
        case ModelTypeEnum::INT_code:
        case ModelTypeEnum::INT64_code:
        case ModelTypeEnum::UINT16_code:
        case ModelTypeEnum::UINT32_code:
        case ModelTypeEnum::UINT64_code:
        case ModelTypeEnum::BOOL_code:
        case ModelTypeEnum::FLOAT_code:
        case ModelTypeEnum::DOUBLE_code:
            need_quote = false;
            need_escape = false;
            break;
        case ModelTypeEnum::CHAR_code:
        case ModelTypeEnum::STR_code:
            need_quote = true;
            need_escape = true;
            break;
        default:
            return false;
    }
    val = model->str();
    return true;
}

bool SqlBuilder::get_plain_val(const Model* model, const std::deque<std::string>& sections,
        std::string& val, bool& need_quote, bool& need_escape) {
    val.clear();
    if (model == nullptr) {
        return false;
    }

    if (is_plain(model)) {
        return get_plain_val_str(model, val, need_quote, need_escape);
    }

    if (sections.empty() || !is_object(model)) {
        return false;
    }

    Model* travel = const_cast<Model*>(model); // for travel only, so we need non const
    for (auto& section : sections) {

        if (is_plain(travel)) { // plain type just return
            return get_plain_val_str(travel, val, need_quote, need_escape);

        } else if (is_object(travel)) {
            Object* obj = (Object*) travel;
            if (obj->is_plain_member(section)) {
                travel = obj->get_plain(section);
            } else if (obj->is_list_member(section)) {
                travel = &(obj->get_list(section));
            } else if (obj->is_object_member(section)) {
                travel = obj->get_object(section);
            } else {
                RELLAF_DEBUG("invalid key %s", section.c_str());
                return false;
            }

        } else if (is_list(travel)) {

            if (section.front() != '<' || section.back() != '>') {
                RELLAF_DEBUG("key %s is not list", section.c_str());
                return false;
            }

            size_t idx = strtoul(section.c_str() + 1, nullptr, 10);
            travel = ((List*) travel)->at(idx);
        }
    }

    if (is_plain(travel)) {
        return get_plain_val_str(travel, val, need_quote, need_escape);
    }

    // last section MUST be plain, and should be returned in for loop
    RELLAF_DEBUG("section %s invalid, last not value", sections.back().c_str());
    return false;
}

bool SqlBuilder::get_list_val(const Model* model, const std::deque<std::string>& sections,
        std::deque<std::string>& vals) {
    vals.clear();
    if (sections.empty() || model == nullptr) {
        return false;
    }

    Model* travel = const_cast<Model*>(model); // never modify memory here,just force convert
    for (auto& section : sections) {

        if (sections.size() == 1 && is_plain(model)) {
            RELLAF_DEBUG("key %s should not be plain", section.c_str());
            return false;
        }

        if (is_object(model)) {
            Object* obj = (Object*) travel;
            if (obj->is_plain_member(section)) {
                travel = obj->get_plain(section);
            } else if (obj->is_list_member(section)) {
                travel = &(obj->get_list(section));
            } else if (obj->is_object_member(section)) {
                travel = obj->get_object(section);
            } else {
                assert(false);
            }

        } else if (is_list(model)) {
            if (section.front() != '<' || section.back() != '>') {
                RELLAF_DEBUG("key %s is not list", section.c_str());
                return false;
            }

            size_t idx = strtoul(section.c_str() + 1, nullptr, 10);
            travel = ((List*) travel)->at(idx);
        }
    }

    if (is_list(travel)) { // convert to array list
        for (const Model* m : *((List*) travel)) {
            if (!is_plain(m)) {
                continue;
            }
            vals.emplace_back(m->str());
        }
        return true;
    }

    // last section MUST be list, and should be returned in the scope of for loop
    RELLAF_DEBUG("section %s invalid, last not list", sections.back().c_str());
    return false;
}

bool SqlBuilder::append_sql(std::string& sql, const std::string& val, bool need_quote,
        bool need_escape) {
    if (need_quote) {
        sql += '\'';
    }
    if (need_escape) {
        std::string escape_val;
        if (!SqlEscape::instance(_charset.name).escape_field(val, escape_val)) {
            RELLAF_DEBUG("escape val failed");
            return false;
        }
        sql += escape_val;
    } else {
        sql += val;
    }
    if (need_quote) {
        sql += '\'';
    }
    return true;
}

}