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
RELLAF_ENUM_DCL(ModelTypeEnum);

RELLAF_ENUM_ITEM_DEF(0, no);
RELLAF_ENUM_ITEM_DEF(1, INT);
RELLAF_ENUM_ITEM_DEF(2, INT64);
RELLAF_ENUM_ITEM_DEF(3, UINT16);
RELLAF_ENUM_ITEM_DEF(4, UINT32);
RELLAF_ENUM_ITEM_DEF(5, UINT64);
RELLAF_ENUM_ITEM_DEF(6, BOOL);
RELLAF_ENUM_ITEM_DEF(7, FLOAT);
RELLAF_ENUM_ITEM_DEF(8, DOUBLE);
RELLAF_ENUM_ITEM_DEF(9, STR);

};

typedef EnumItem ModelType;

}