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
#include <map>

namespace rellaf {

typedef enum {
    OK = 0,
    ILL_TOKEN,
    ILL_BEGIN,
    ILL_END,
    UNMATCH,
    NONE_FIELD,
    UNKNOWN
} PatternErr;

typedef enum {
    STATE_INIT = 0,
    STATE_NORMAL,
    STATE_MATCH_TOKEN,
    STATE_FETCH,
    STATE_END_FETCH,
} PatternState;

class SqlPattern {
public:
    typedef enum {
        LITERAL = 0,
        FIELD,
        LIST
    } StubType;

    class Stub {
    public:
        int type() const {
            return _type;
        }

        const std::string& value() const {
            return _value;
        }

        void set_type(StubType type) {
            _type = type;
        }

        void set_value(const std::string& value) {
            _value = value;
        }

    private:
        StubType _type;
        std::string _value;
    };

public:
    static bool explode(const std::string& pattern, std::deque<Stub>& pieces, PatternErr& err);

private:

    static const char token_pairs[128];
    static const uint8_t tokens[128];
    static const uint8_t end_tokens[128];
};

class UrlPattern {
public:
    static bool explode_path_vars(const std::string& path, std::map<uint32_t, std::string>& vars,
            PatternErr& err);

    static bool fetch_path_vars(const std::string& path, std::map<uint32_t, std::string>& vars,
            std::map<std::string, std::string>& vals);
};

}
