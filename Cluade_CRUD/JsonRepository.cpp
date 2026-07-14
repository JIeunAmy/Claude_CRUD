#include "JsonRepository.h"

#include <algorithm>
#include <filesystem>

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

bool JsonRepository::Save()
{
    JsonValue root(JsonValue::Array{});
    for (const Record& record : records_)
    {
        root.PushBack(ToJson(record));
    }
    return root.Save(path_, 2);
}

const std::vector<Record>& JsonRepository::GetAll() const
{
    return records_;
}

const Record* JsonRepository::FindById(int id) const
{
    auto it = std::find_if(records_.begin(), records_.end(),
        [id](const Record& record) { return record.id == id; });
    return it != records_.end() ? &(*it) : nullptr;
}

Record& JsonRepository::Add(std::string name, std::string value)
{
    int nextId = 1;
    for (const Record& record : records_)
    {
        nextId = std::max(nextId, record.id + 1);
    }

    Record record;
    record.id = nextId;
    record.name = std::move(name);
    record.value = std::move(value);
    records_.push_back(std::move(record));
    return records_.back();
}

bool JsonRepository::UpdateById(int id, std::optional<std::string> name, std::optional<std::string> value)
{
    auto it = std::find_if(records_.begin(), records_.end(),
        [id](const Record& record) { return record.id == id; });
    if (it == records_.end())
    {
        return false;
    }

    if (name.has_value())
    {
        it->name = std::move(*name);
    }
    if (value.has_value())
    {
        it->value = std::move(*value);
    }
    return true;
}

bool JsonRepository::RemoveById(int id)
{
    auto it = std::find_if(records_.begin(), records_.end(),
        [id](const Record& record) { return record.id == id; });
    if (it == records_.end())
    {
        return false;
    }

    records_.erase(it);
    return true;
}
