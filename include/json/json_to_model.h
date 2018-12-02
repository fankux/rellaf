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

/**
 * @brief convert model object to json string
 * @param model
 * @param json_str output
 * @param is_format
 */
bool model_to_json(const Model* model, std::string& json_str, bool is_format = false);

/**
 * @brief convert json string to model object
 * @param json_str
 * @param model
 */
bool json_to_model(const std::string& json_str, Model* model);

}