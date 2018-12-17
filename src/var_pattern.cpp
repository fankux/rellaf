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

#include "var_pattern.h"
#include "common.h"

namespace rellaf {

/**
 * hard code directly
 * #(35), {(123), }(125), [(91), ](93)
 */
const char SqlPattern::token_pairs[128] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 31
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 63
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ']', 0, 0, 0, 0, // 95
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '}', 0, 0, 0, 0, // 127
};
const uint8_t SqlPattern::tokens[128] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 31
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 63
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, // 95
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, // 127
};
const uint8_t SqlPattern::end_tokens[128] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 31
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 63
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, // 95
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, // 127
};

bool SqlPattern::explode(const std::string& pattern, std::deque<Stub>& pieces, PatternErr& err) {
    pieces.clear();

    std::string section;

    Stub stub;
    stub.set_type(LITERAL);

    char token = 0;

    int state = STATE_INIT;
    for (char c : pattern) {
        if (state == STATE_INIT) {
            if (tokens[(int)c]) {
                if (c == '#') {
                    if (!section.empty()) {
                        stub.set_value(section);
                        pieces.emplace_back(stub);
                        section.clear();
                    }

                    state = STATE_MATCH_TOKEN;
                } else {
                    err = ILL_TOKEN;
                    RELLAF_DEBUG("unexpected token : %c", c);
                    return false;
                }
            } else {
                section += c;
            }
            continue;
        }

        if (state == STATE_MATCH_TOKEN) {
            if (!token_pairs[(int)c]) {
                err = ILL_BEGIN;
                RELLAF_DEBUG("unexpected begin token : %c", c);
                return false;
            }
            token = c;

            state = STATE_FETCH;
            stub.set_type(token == '[' ? LIST : FIELD);
            continue;
        }

        if (state == STATE_FETCH) {
            if (tokens[(int)c]) {
                if (end_tokens[(int)c]) {
                    if (c != token_pairs[(int)token]) {
                        err = UNMATCH;
                        RELLAF_DEBUG("unmatch end token %c of %c", c, token);
                        return false;
                    }

                    if (section.empty()) {
                        err = NONE_FIELD;
                        RELLAF_DEBUG("field empty");
                        return false;
                    }

                    stub.set_value(section);
                    pieces.emplace_back(stub);
                    section.clear();

                    state = STATE_INIT;
                    stub.set_type(LITERAL);
                } else {
                    err = ILL_END;
                    RELLAF_DEBUG("unexped end token %c of %c", c, token);
                    return false;
                }

            } else {
                section += c;
            }
            continue;
        }
    }

    bool ret;
    switch (state) {
        case STATE_INIT:
            if (!section.empty()) {
                stub.set_value(section);
                pieces.emplace_back(stub);
            }
            err = OK;
            ret = true;
            break;
        case STATE_MATCH_TOKEN:
            err = ILL_BEGIN;
            ret = false;
            break;
        case STATE_FETCH:
            err = ILL_END;
            ret = false;
            break;
        default:
            err = UNKNOWN;
            RELLAF_DEBUG("unknow state : %d", state);
            ret = false;
            break;
    }
    return ret;
}

bool UrlPattern::explode_path_vars(const std::string& path, std::map<uint32_t, std::string>& vars,
        PatternErr& err) {
    vars.clear();

    bool start_flag = false;

    uint32_t idx = 0;
    std::string section;
    PatternState state = STATE_INIT;
    for (size_t i = 0; i < path.size();) {
        char c = path[i];

        if (state == STATE_INIT) {
            if (c == '/' || c == '{' || c == '}') {
                if (c == '/') {
                    if (start_flag) {
                        ++idx;
                    }
                    state = STATE_MATCH_TOKEN;
                } else {
                    err = ILL_TOKEN;
                    RELLAF_DEBUG("unexpected token : %c", c);
                    vars.clear();
                    return false;
                }
            }

            start_flag = true;
            ++i;
            continue;
        }

        if (state == STATE_MATCH_TOKEN) {
            if (c == '/' || c == '{' || c == '}') {
                if (c == '/') {
                    ++i;
                    continue;
                }
                if (c != '{') {
                    err = ILL_BEGIN;
                    RELLAF_DEBUG("unexpected start token : %c", c);
                    vars.clear();
                    return false;
                }

                state = STATE_FETCH;
            } else {
                state = STATE_NORMAL;
            }

            ++i;
            continue;
        }

        if (state == STATE_NORMAL) {
            if (c == '{' || c == '}') {
                err = ILL_TOKEN;
                RELLAF_DEBUG("unexpected token : %c", c);
                vars.clear();
                return false;
            }
            if (c == '/') {
                state = STATE_INIT;
                continue;
            }

            ++i;
            continue;
        }

        if (state == STATE_FETCH) {
            if (c == '/' || c == '{' || c == '}') {
                if (c == '}') {
                    if (section.empty()) {
                        err = NONE_FIELD;
                        RELLAF_DEBUG("field empty");
                        vars.clear();
                        return false;
                    }
                    vars.emplace(idx, section);
                    section.clear();

                    state = STATE_END_FETCH;
                } else {
                    err = ILL_END;
                    RELLAF_DEBUG("unexpected end token %c", c);
                    vars.clear();
                    return false;
                }

            } else {
                section += c;
            }

            ++i;
            continue;
        }

        if (state == STATE_END_FETCH) {
            if (c != '/') {
                err = ILL_TOKEN;
                RELLAF_DEBUG("unexpected token : %c after end token", c);
                vars.clear();
                return false;
            }
            state = STATE_INIT;
            continue;
        }
    }

    bool ret;
    switch (state) {
        case STATE_INIT:
        case STATE_NORMAL:
        case STATE_END_FETCH:
            if (!section.empty()) {
                vars.emplace(idx, section);
            }
            err = OK;
            ret = true;
            break;
        case STATE_MATCH_TOKEN:
            err = OK;
            ret = true;
            break;
        case STATE_FETCH:
            err = ILL_END;
            vars.clear();
            ret = false;
            break;
        default:
            err = UNKNOWN;
            RELLAF_DEBUG("unknow state : %d", state);
            vars.clear();
            ret = false;
            break;
    }
    return ret;
}

bool UrlPattern::fetch_path_vars(const std::string& path,
        std::map<uint32_t, std::string>& vars,
        std::map<std::string, std::string>& vals) {
    vals.clear();

    bool start_flag = false;

    uint32_t idx = 0;
    std::string section;
    PatternState state = STATE_INIT;
    for (size_t i = 0; i < path.size();) {
        char c = path[i];

        if (state == STATE_INIT) {
            if (c == '/') {
                if (!section.empty()) {
                    if (vars.count(idx) == 1) {
                        vals.emplace(vars[idx], section);
                    }
                    section.clear();
                    if (start_flag) {
                        ++idx;
                    }
                }
                ++i;

            } else {
                state = STATE_MATCH_TOKEN;
            }

            start_flag = true;
            continue;
        }

        if (state == STATE_MATCH_TOKEN) {
            if (c == '/') {
                state = STATE_INIT;
                continue;
            }
        }

        section += c;
        ++i;
    }

    switch (state) {
        case STATE_INIT:
        case STATE_MATCH_TOKEN:
            if (vars.count(idx) == 1 && !section.empty()) {
                vals.emplace(vars[idx], section);
            }
            break;
        default:
            break;
    }
    return true;
}

}