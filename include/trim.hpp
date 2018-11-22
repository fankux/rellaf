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

namespace rellaf {

inline void trim_left(std::string& s, const std::string& any_of = "") {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&](int ch) {
        if (any_of.empty()) {
            return !std::isspace(ch);
        }
        for (char c : any_of) {
            if (ch == c) {
                return false;
            }
        }
        return true;
    }));
}

inline void trim_right(std::string& s, const std::string& any_of = "") {
    s.erase(std::find_if(s.rbegin(), s.rend(), [&](int ch) {
        if (any_of.empty()) {
            return !std::isspace(ch);
        }
        for (char c : any_of) {
            if (ch == c) {
                return false;
            }
        }
        return true;
    }).base(), s.end());
}

inline void trim(std::string& s, const std::string& any_of = "") {
    trim_left(s);
    trim_right(s);
}

}
