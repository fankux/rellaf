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

#include "json_model_tool.h"

namespace rellaf {

bool model_to_json(Model* model, Json::Value& json) {
    json.clear();
    for (auto& entry : model->get_int_names()) {
        json[entry.first] = (Json::Int)model->get_int(entry.first);
    }
    for (auto& entry : model->get_int64_names()) {
        json[entry.first] = (Json::Int)model->get_int64(entry.first);
    }
    for (auto& entry : model->get_uint16_names()) {
        json[entry.first] = (Json::UInt)model->get_uint16(entry.first);
    }
    for (auto& entry : model->get_uint32_names()) {
        json[entry.first] = (Json::UInt)model->get_uint32(entry.first);
    }
    for (auto& entry : model->get_uint64_names()) {
        json[entry.first] = (Json::UInt64)model->get_uint64(entry.first);
    }
    for (auto& entry : model->get_bool_names()) {
        json[entry.first] = model->get_bool(entry.first);
    }
    for (auto& entry : model->get_float_names()) {
        json[entry.first] = model->get_float(entry.first);
    }
    for (auto& entry : model->get_double_names()) {
        json[entry.first] = model->get_double(entry.first);
    }
    for (auto& entry : model->get_str_names()) {
        json[entry.first] = model->get_str(entry.first);
    }
    return true;
}

std::string model_to_json_str(Model* model, bool is_compact) {
    Json::Value node;
    if (model_to_json(model, node)) {
        Json::StreamWriterBuilder builder;
        if (is_compact) {
            builder.settings_["indentation"] = "";
        }
        return Json::writeString(builder, node);
    }
    return "";
}

bool json_to_model(const Json::Value& json, Model* model) {
    if (!json.isObject()) {
        RELLAF_DEBUG("not object, could not parse to MODEL");
        return false;
    }
    for (const std::string& key : json.getMemberNames()) {
        const Json::Value& val = json[key];
        if (model->is_int_member(key) && val.isInt()) {
            model->set_int(key, val.asInt());
        } else if (model->is_uint16_member(key) && val.isUInt()) {
            model->set_uint16(key, (uint16_t)(val.asUInt()));
        } else if (model->is_uint32_member(key) && val.isUInt()) {
            model->set_uint32(key, val.asUInt());
        } else if (model->is_uint64_member(key) && val.isUInt64()) {
            model->set_uint64(key, val.asUInt64());
        } else if (model->is_bool_member(key) && val.isBool()) {
            model->set_bool(key, val.asBool());
        } else if (model->is_float_member(key)) {
            model->set_float(key, val.asFloat());
        } else if (model->is_double_member(key)) {
            model->set_double(key, val.asDouble());
        } else if (model->is_str_member(key) && val.isString()) {
            model->set_str(key, val.asString());
        }
    }
    return true;
}

bool json_str_to_model(const std::string& json_str, Model* model) {
    if (json_str.empty()) {
        return true;
    }

    std::string err;
    Json::Value json;
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(json_str.data(), json_str.data() + json_str.size(), &json, &err)) {
        RELLAF_DEBUG("not json, could not parse to MODEL");
        return false;
    }
    return json_to_model(json, model);
}

}