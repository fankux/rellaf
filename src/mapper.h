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
// mybatis like sql mapper framework
// TODO..

#pragma once

#include <map>
#include <deque>
#include "common.h"
#include "sql_pattern.h"
#include "sql_model_tool.h"

namespace rellaf {

class Select {

public:

};

class Mapper {

public:

    template<class T>
    void build_arg(std::map<std::string, std::map<std::string, std::string>>& vals, T& arg) {
        std::map<std::string, std::string> val;
//        str_sql_vals(&arg, val);
        vals.emplace(arg.name(), val);
    }

    template<class Ret, class ...Args>
    int select(const std::string& key, Ret& ret, Args& ...args) {
        auto entry = _pices.find(key);
        if (entry == _pices.end()) {
            return -1;
        }

        std::map<std::string, std::map<std::string, std::string>> vals;
        int arg_count = sizeof...(args);
        bool arr[] = {(build_arg(vals, args), true)...}; // for arguments expansion

        auto val = vals.begin()->second;

        std::string sql;
        for (const SqlPattern::Stub& stub : entry->second) {
            if (stub.type() == SqlPattern::LITERAL) {
                sql += stub.value();
            } else if (stub.type() == SqlPattern::FIELD) {
                auto val_e = val.find(stub.value());
                if (val_e == val.end()) {
                    RELLAF_DEBUG("no field : %s", stub.value().c_str());
                    return -1;
                }
                sql += val_e->second;

            } else if (stub.type() == SqlPattern::LIST) {


            }
            // TODO... array
        }

        RELLAF_DEBUG("mapper select sql : %s", sql.c_str());
    }

    bool reg_insert(const std::string& key) {
        std::string pattern = "SELECT a, b, c FROM capturer WHERE id=#{version} AND type in ()";

        std::deque<SqlPattern::Stub> pices;
        SqlPattern::PatternErr err;
        if (!SqlPattern::explode(pattern, pices, err)) {
            RELLAF_DEBUG("reg sql failed, explode pattern failed : %s", pattern.c_str());
            return false;
        }

        _patterns.insert({key, pattern});
        _pices.insert({key, pices});
        return true;
    }

private:
    std::map<std::string, std::string> _patterns;
    std::map<std::string, std::deque<SqlPattern::Stub>> _pices;
};

}