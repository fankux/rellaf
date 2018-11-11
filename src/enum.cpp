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

#include "enum.h"

namespace rellaf {

const EnumItem* IEnum::get_by_name(const std::string& name) {
    auto it = names().find(name);
    if (it == names().end()) {
        return nullptr;
    }
    return it->second;
}

const EnumItem* IEnum::get_by_code(int code) {
    auto it = codes().find(code);
    if (it == codes().end()) {
        return nullptr;
    }
    return it->second;
}

bool IEnum::exist(const std::string& name) {
    return names().find(name) != names().end();
}

bool IEnum::exist(int code) {
    return codes().find(code) != codes().end();
}

}