#pragma once

#include <string>

#include "Json.h"

struct Record
{
    int id = 0;
    std::string name;
    std::string value;
};

inline ClaudeJson::JsonValue ToJson(const Record& record)
{
    ClaudeJson::JsonValue json(ClaudeJson::JsonValue::Object{});
    json["id"] = ClaudeJson::JsonValue(static_cast<double>(record.id));
    json["name"] = ClaudeJson::JsonValue(record.name);
    json["value"] = ClaudeJson::JsonValue(record.value);
    return json;
}

inline Record FromJson(const ClaudeJson::JsonValue& json)
{
    Record record;
    record.id = static_cast<int>(json["id"].AsNumber());
    record.name = json["name"].AsString();
    record.value = json["value"].AsString();
    return record;
}
