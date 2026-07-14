#include "Record.h"

using ClaudeJson::JsonValue;

JsonValue ToJson(const Record& record)
{
    JsonValue json(JsonValue::Object{});
    json["id"] = JsonValue(record.id);
    json["name"] = JsonValue(record.name);
    json["value"] = JsonValue(record.value);
    return json;
}

Record FromJson(const JsonValue& json)
{
    Record record;
    record.id = static_cast<int>(json["id"].AsNumber());
    record.name = json["name"].AsString();
    record.value = json["value"].AsString();
    return record;
}
