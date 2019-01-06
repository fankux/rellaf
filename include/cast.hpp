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

template<class T>
class Caster {
public:
    char cast(const std::string& val, const char& ret) {
        return (char)(val.empty() ? '\0' : val[0]);
    }

    int16_t cast(const std::string& val, const int16_t& ret) {
        return (int16_t)strtol(val.c_str(), nullptr, 10);
    }

    int cast(const std::string& val, const int& ret) {
        return (int)strtol(val.c_str(), nullptr, 10);
    }

    int64_t cast(const std::string& val, const int64_t& ret) {
        return strtoll(val.c_str(), nullptr, 10);
    }

    uint16_t cast(const std::string& val, const uint16_t& ret) {
        return (uint16_t)strtoul(val.c_str(), nullptr, 10);
    }

    uint32_t cast(const std::string& val, const uint32_t& ret) {
        return (uint32_t)strtoul(val.c_str(), nullptr, 10);
    }

    int64_t cast(const std::string& val, const uint64_t& ret) {
        return (int64_t)(strtoull(val.c_str(), nullptr, 10));
    }

    bool cast(const std::string& val, const bool& ret) {
        for (char c : val) {
            if (c != '0') {
                return true;
            }
        }
        return false;
    }

    float cast(const std::string& val, const float& ret) {
        return strtof(val.c_str(), nullptr);
    }

    double cast(const std::string& val, const double& ret) {
        return strtod(val.c_str(), nullptr);
    }

    std::string cast(const std::string& val, const std::string& ret) {
        return val;
    }
};

template<class T>
T cast(const std::string& val) {
    return Caster<T>().cast(val, T());
}
