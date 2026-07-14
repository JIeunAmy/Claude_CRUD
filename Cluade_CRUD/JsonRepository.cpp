#include "JsonRepository.h"

#include <filesystem>

#include "Json.h"

using ClaudeJson::JsonValue;

JsonRepository::JsonRepository(std::string path) : path_(std::move(path))
{
}

void JsonRepository::Load()
{
    records_.clear();

    if (!std::filesystem::exists(path_))
    {
        return;
    }

    JsonValue root = JsonValue::Load(path_);
    for (const JsonValue& item : root.AsArray())
    {
        records_.push_back(FromJson(item));
    }
}

bool JsonRepository::Save() const
{
    JsonValue::Array array;
    array.reserve(records_.size());
    for (const Record& record : records_)
    {
        array.push_back(ToJson(record));
    }

    return JsonValue(std::move(array)).Save(path_, 2);
}

const Record& JsonRepository::Add(std::string name, std::string value)
{
    int nextId = 1;
    for (const Record& record : records_)
    {
        if (record.id >= nextId)
        {
            nextId = record.id + 1;
        }
    }

    Record record;
    record.id = nextId;
    record.name = std::move(name);
    record.value = std::move(value);

    records_.push_back(std::move(record));
    return records_.back();
}
