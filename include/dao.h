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
// java mybatis like sql dao framework
//

#pragma once

#include <string.h>
#include <map>
#include <deque>
#include <memory>
#include "common.h"
#include "sql_pattern.h"
#include "model.h"
#include "model_type.h"
#include "mysql_escape.h"

namespace rellaf {

class DaoModel {
public:
    explicit DaoModel(const Model& model) : _model(&model) {}

    DaoModel(const std::string& name, const Model& model) : _name(name), _model(&model) {}

    const std::string& name() const {
        return _name;
    }

    const Model* model() const {
        return _model;
    }

private:
    std::string _name;
    const Model* _model;
};

class Dao {

public:
    class Charset : public IEnum {
    RELLAF_ENUM_DCL(Charset);

    RELLAF_ENUM_ITEM_DEF(1, UTF8);
    RELLAF_ENUM_ITEM_DEF(2, GBK);
    };

    typedef EnumItem CharsetType;

public:
    void set_charset(const CharsetType& charset) {
        _charset = charset;
    }

protected:
    class Reg {
    public:
        Reg(Dao* inst, const std::string& method, const std::string& pattern) {

            std::deque<SqlPattern::Stub> pices;
            SqlPattern::PatternErr err;
            if (!SqlPattern::explode(pattern, pices, err)) {
                RELLAF_DEBUG("reg sql failed, explode pattern failed : %s", pattern.c_str());
                exit(-1);
            }

            inst->_patterns.emplace(method, pattern);
            inst->_pices.emplace(method, pices);
        }
    };

    template<class T>
    void build_dao_models(std::map<std::string, const DaoModel*>& dao_models, const T& arg) {
        dao_models.emplace(arg.name(), &arg);
    }

    template<class ...Args>
    bool prepare_statement(const std::string& method, std::string& sql, const Args& ...args) {
        sql.clear();

        auto entry = _pices.find(method);
        if (entry == _pices.end()) {
            return false;
        }

        int arg_count = sizeof...(args);
        bool is_multi_arg = arg_count > 1;

        std::map<std::string, const DaoModel*> dao_models;
        bool arr[] = {(build_dao_models(dao_models, args), true)...}; // for arguments expansion
        (void)(arr);// suppress warning

        for (const SqlPattern::Stub& stub : entry->second) {
            std::deque<std::string> sections;
            if (stub.type() == SqlPattern::LITERAL) {
                sql += stub.value();
                continue;
            }

            split_section(stub.value(), sections);
            if (sections.empty()) {
                RELLAF_DEBUG("no section failed : %s", stub.value().c_str());
                return false;
            }

            std::deque<const Model*> model_box;
            if (!is_multi_arg) {
                model_box.push_front(dao_models.begin()->second->model());
            } else {
                auto model_entry = dao_models.find(sections.front());
                if (model_entry == dao_models.end()) {
                    RELLAF_DEBUG("no dao model name : %s", sections.front().c_str());
                    return false;
                }
                model_box.push_front(model_entry->second->model());
                sections.pop_front();
            }

            if (stub.type() == SqlPattern::FIELD) {
                std::string val;
                bool need_quote = false;
                bool need_escape = false;
                if (!get_plain_val(model_box.front(), sections, val, need_quote, need_escape)) {
                    return false;
                }
                if (!append_sql(sql, val, need_quote, need_escape)) {
                    return false;
                }
            } else if (stub.type() == SqlPattern::LIST) {
                std::deque<std::string> vals;
                if (!get_list_val(model_box.front(), sections, vals)) {
                    return false;
                }
                sql += '(';
                for (auto& val : vals) {
                    if (!append_sql(sql, val, true, true)) {
                        return false;
                    }
                    sql += ',';
                }
                if (sql.back() == ',') {
                    sql.back() = ')';
                } else {
                    sql += ')';
                }
            }
        }
        RELLAF_DEBUG("mapper select sql : %s", sql.c_str());
        return true;
    }

    template<class Ret, class ...Args>
    int select_impl(const std::string& method, Ret& ret, const Args& ...args) {
        std::string sql;
        if (!prepare_statement(method, sql, args...)) {
            return -1;
        }
        return 0;
    }

    template<class Ret, class ...Args>
    int select_list_impl(const std::string& method, std::deque<Ret>& ret_list,
            const Args& ...args) {
        std::string sql;
        if (!prepare_statement(method, sql, args...)) {
            return -1;
        }
        return 0;
    }

protected:
    void split_section(const std::string& section_str, std::deque<std::string>& sections);

    bool get_plain_val_str(const Model* model, const std::string& key, std::string& val,
            bool& need_quote, bool& need_escape);

    bool get_plain_val(const Model* model, const std::deque<std::string>& sections,
            std::string& val, bool& need_quote, bool& need_escape);

    bool get_list_val(const Model* model, const std::deque<std::string>& sections,
            std::deque<std::string>& vals);

    bool append_sql(std::string& sql, const std::string& val, bool need_quote, bool need_escape);

private:
    CharsetType _charset = Charset::e().UTF8;
    std::map<std::string, std::string> _patterns;
    std::map<std::string, std::deque<SqlPattern::Stub>> _pices;
};

#define RELLAF_DAO_SELECT(_method_, _pattern_, _Ret_)                               \
public:                                                                             \
template<class ...Args> int _method_(_Ret_& ret, const Args& ...args) {             \
    return select_impl(#_method_, ret, args...);                                    \
}                                                                                   \
private:                                                                            \
Reg _reg_##_method_{this, #_method_, _pattern_}

#define RELLAF_DAO_SELECT_list(_method_, _pattern_, _Ret_)                          \
public:                                                                             \
template<class ...Args> int _method_(std::deque<_Ret_>& ret, const Args& ...args) { \
    return select_list_impl(#_method_, ret, args...);                               \
}                                                                                   \
private:                                                                            \
Reg _reg_##_method_{this, #_method_, _pattern_}

}