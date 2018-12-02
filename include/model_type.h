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

class ModelTypeEnum : public IEnum {
rellaf_enum_dcl(ModelTypeEnum);

rellaf_enum_item_def(0, no);
rellaf_enum_item_code_def(1, CHAR);
rellaf_enum_item_code_def(2, INT16);
rellaf_enum_item_code_def(3, INT);
rellaf_enum_item_code_def(4, INT64);
rellaf_enum_item_code_def(5, UINT16);
rellaf_enum_item_code_def(6, UINT32);
rellaf_enum_item_code_def(7, UINT64);
rellaf_enum_item_code_def(8, BOOL);
rellaf_enum_item_code_def(9, FLOAT);
rellaf_enum_item_code_def(10, DOUBLE);
rellaf_enum_item_code_def(11, STR);

rellaf_enum_item_code_def(100, OBJECT);
rellaf_enum_item_code_def(101, LIST);

};

typedef EnumItem ModelType;

}