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

#include "enum.h"

namespace rellaf {

class HttpArgTypeEnum : public IEnum {
rellaf_enum_dcl(HttpArgTypeEnum);

rellaf_enum_item_code_def(1, REQ_BODY);
rellaf_enum_item_code_def(2, REQ_PARAM);
rellaf_enum_item_code_def(3, PATH_VAR);

};

} // namespace rellaf
