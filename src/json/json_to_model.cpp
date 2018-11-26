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

#include <memory>
#include "json/json.h"
#include "json_to_model.h"

namespace rellaf {

static void model_to_json(const Model* model, Json::Value& json) {
    if (model == nullptr) {
        json = Json::Value(Json::nullValue);
        return;
    }

    if (model->rellaf_type() == ModelTypeEnum::e().LIST) {
        json = Json::Value(Json::arrayValue);
        if (!model->get_lists().empty()) {
            const ModelList& list = model->get_lists().begin()->second;
            for (auto& entry : list) {
                Json::Value item;
                model_to_json(entry, item);
                json.append(item);
            }
        }
        return;
    }

    if (model->rellaf_type() == ModelTypeEnum::e().OBJECT) {
        json = Json::Value(Json::objectValue);
        for (auto& entry : model->get_objects()) {
            Json::Value item;
            model_to_json(entry.second, item);
            json[entry.first] = item;
        }

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
    }
}

bool model_to_json(const Model* model, std::string& json_str, bool is_format) {
    json_str.clear();
    Json::Value node;
    model_to_json(model, node);
    Json::StreamWriterBuilder builder;
    if (!is_format) {
        builder.settings_["indentation"] = "";
    }
    json_str = Json::writeString(builder, node);
    return true;
}

static void json_to_model(const Json::Value& json, Model* model, const std::string& key) {
    if (model == nullptr || model->is_plain()) { // plain type not concern
        return;
    }

    if (json.isObject() && model->rellaf_type() == ModelTypeEnum::e().OBJECT) {
        for (const std::string& json_key : json.getMemberNames()) {
            json_to_model(json[json_key], model, json_key);
        }
        return;
    }

    if (json.isArray() && model->rellaf_type() == ModelTypeEnum::e().LIST &&
            !model->get_lists().empty()) {

        ModelList& list = model->get_lists().begin()->second;
        for (Json::ArrayIndex i = 0; i < json.size(); ++i) {
            if (i >= list.size()) {
                break;
            }
            json_to_model(json[i], list.get(i), "");
        }
        return;
    }

    if (json.isInt()) {
        model->set_int(key, json.asInt());
    } else if (json.isUInt()) {
        model->set_uint16(key, (uint16_t)(json.asUInt()));
    } else if (json.isUInt()) {
        model->set_uint32(key, json.asUInt());
    } else if (json.isUInt64()) {
        model->set_uint64(key, json.asUInt64());
    } else if (json.isBool()) {
        model->set_bool(key, json.asBool());
    } else if (json.isDouble() && model->is_float_member(key)) {
        model->set_float(key, json.asFloat());
    } else if (json.isDouble() && model->is_double_member(key)) {
        model->set_double(key, json.asDouble());
    } else if (json.isString()) {
        model->set_str(key, json.asString());
    }
}

bool json_to_model(const std::string& json_str, Model* model) {
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
    json_to_model(json, model, "");
    return true;
}

}