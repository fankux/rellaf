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

#include "model.h"

namespace rellaf {

class Info : public Object {
rellaf_model_dcl(Info);

rellaf_model_def_uint64(id, 0);
rellaf_model_def_str(content, "");
rellaf_model_def_uint32(create_time, 0);
rellaf_model_def_uint32(update_time, 0);

};

}
