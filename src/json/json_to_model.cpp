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
#include "json/json_to_model.h"

namespace rellaf {

static void model_to_json_inner(const Model* model, Json::Value& json) {
    if (model == nullptr) {
        json = Json::Value(Json::nullValue);
        return;
    }

    switch (model->rellaf_type().code) {
        case ModelTypeEnum::CHAR_code:
            json = (Json::Int)((Plain<char>*)model)->value();
            return;
        case ModelTypeEnum::INT16_code:
            json = (Json::Int)((Plain<int16_t>*)model)->value();
            return;
        case ModelTypeEnum::INT_code:
            json = ((Plain<int>*)model)->value();
            return;
        case ModelTypeEnum::INT64_code:
            json = (Json::Int64)((Plain<int64_t>*)model)->value();
            return;
        case ModelTypeEnum::UINT16_code:
            json = (Json::UInt)((Plain<uint16_t>*)model)->value();
            return;
        case ModelTypeEnum::UINT32_code:
            json = (Json::UInt)((Plain<int32_t>*)model)->value();
            return;
        case ModelTypeEnum::UINT64_code:
            json = (Json::UInt64)((Plain<uint64_t>*)model)->value();
            return;
        case ModelTypeEnum::BOOL_code:
            json = ((Plain<bool>*)model)->value();
            return;
        case ModelTypeEnum::FLOAT_code:
            json = ((Plain<float>*)model)->value();
            return;
        case ModelTypeEnum::DOUBLE_code:
            json = ((Plain<double>*)model)->value();
            return;
        case ModelTypeEnum::STR_code:
            json = ((Plain<std::string>*)model)->value();
            return;
        default:
            break;
    }

    if (model->rellaf_type() == ModelTypeEnum::e().LIST) {
        json = Json::Value(Json::arrayValue);
        for (auto& item : *((List*)model)) {
            Json::Value item_node;
            model_to_json_inner(item, item_node);
            json.append(item_node);
        }
        return;
    }

    if (model->rellaf_type() == ModelTypeEnum::e().OBJECT) {
        json = Json::Value(Json::objectValue);
        for (auto& item : ((Object*)model)->get_plains()) {
            Json::Value item_node;
            model_to_json_inner(item.second, item_node);
            json[item.first] = item_node;
        }
        for (auto& item : ((Object*)model)->get_lists()) {
            Json::Value item_node(Json::arrayValue);
            model_to_json_inner(&item.second, item_node);
            json[item.first] = item_node;
        }
        for (auto& item : ((Object*)model)->get_objects()) {
            Json::Value item_node;
            model_to_json_inner(item.second, item_node);
            json[item.first] = item_node;
        }
    }
}

bool model_to_json(const Model* model, std::string& json_str, bool is_format) {
    json_str.clear();
    Json::Value node;
    model_to_json_inner(model, node);
    Json::StreamWriterBuilder builder;
    if (!is_format) {
        builder.settings_["indentation"] = "";
    }
    json_str = Json::writeString(builder, node);
    return true;
}

static void json_to_model_inner(const Json::Value& json, Model* model) {
    if (model == nullptr || json.isNull()) {
        return;
    }

    switch (model->rellaf_type().code) {
        case ModelTypeEnum::CHAR_code:
            if (json.isNumeric()) {
                ((Plain<char>*)model)->set((char)json.asInt());
            }
            return;
        case ModelTypeEnum::INT16_code:
            if (json.isNumeric()) {
                ((Plain<int16_t>*)model)->set((int16_t)json.asInt());
            }
            return;
        case ModelTypeEnum::INT_code:
            if (json.isNumeric()) {
                ((Plain<int>*)model)->set(json.asInt());
            }
            return;
        case ModelTypeEnum::INT64_code:
            if (json.isNumeric()) {
                ((Plain<int64_t>*)model)->set(json.asInt64());
            }
            return;
        case ModelTypeEnum::UINT16_code:
            if (json.isNumeric()) {
                ((Plain<uint16_t>*)model)->set((uint16_t)json.asUInt());
            }
            return;
        case ModelTypeEnum::UINT32_code:
            if (json.isNumeric()) {
                ((Plain<uint32_t>*)model)->set(json.asUInt());
            }
            return;
        case ModelTypeEnum::UINT64_code:
            if (json.isNumeric()) {
                ((Plain<uint64_t>*)model)->set(json.asUInt64());
            }
            return;
        case ModelTypeEnum::BOOL_code:
            if (json.isBool()) {
                ((Plain<bool>*)model)->set(json.asBool());
            }
            return;
        case ModelTypeEnum::FLOAT_code:
            if (json.isDouble()) {
                ((Plain<float>*)model)->set(json.asFloat());
            }
            return;
        case ModelTypeEnum::DOUBLE_code:
            if (json.isDouble()) {
                ((Plain<float>*)model)->set(json.asFloat());
            }
            return;
        case ModelTypeEnum::STR_code:
            if (json.isString()) {
                ((Plain<std::string>*)model)->set(json.asString());
            }
            return;
        default:
            break;
    }

    if (json.isArray() && model->rellaf_type() == ModelTypeEnum::e().LIST) {
        List& list = *((List*)model);
        for (size_t i = 0; i < list.size(); ++i) {
            if (json[(Json::ArrayIndex)i].isNull()) {
                list.set(i, nullptr);
            }
            json_to_model_inner(json[(Json::ArrayIndex)i], list.at(i));
        }
        return;
    }

    if (json.isObject() && model->rellaf_type() == ModelTypeEnum::e().OBJECT) {
        Object* obj = (Object*)model;
        for (const std::string& json_key : json.getMemberNames()) {
            // TODO.. null value
            if (obj->is_plain_member(json_key)) {
                json_to_model_inner(json[json_key], obj->get_plain(json_key));
            } else if (obj->is_list_member(json_key)) {
                json_to_model_inner(json[json_key], &obj->get_list(json_key));
            } else if (obj->is_object_member(json_key)) {
                json_to_model_inner(json[json_key], obj->get_object(json_key));
            }
        }
        return;
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
    json_to_model_inner(json, model);
    return true;
}

}