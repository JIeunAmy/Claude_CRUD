#include "JsonRepository.h"

#include <algorithm>
#include <filesystem>

JsonRepository::JsonRepository(std::string path) : path_(std::move(path))
{
}

void JsonRepository::Load()
{
    records_.clear();

    if (!std::filesystem::exists(path_))
    {
        ClaudeJson::JsonValue(ClaudeJson::JsonValue::Array{}).Save(path_, 2);
        return;
    }

    ClaudeJson::JsonValue root = ClaudeJson::JsonValue::Load(path_);
    for (const auto& item : root.AsArray())
    {
        records_.push_back(FromJson(item));
    }
}

bool JsonRepository::Save() const
{
    ClaudeJson::JsonValue root(ClaudeJson::JsonValue::Array{});
    for (const auto& record : records_)
    {
        root.PushBack(ToJson(record));
    }
    return root.Save(path_, 2);
}

const Record* JsonRepository::FindById(int id) const
{
    for (const auto& record : records_)
    {
        if (record.id == id)
        {
            return &record;
        }
    }
    return nullptr;
}

Record JsonRepository::Add(std::string name, std::string value)
{
    int maxId = 0;
    for (const auto& record : records_)
    {
        maxId = std::max(maxId, record.id);
    }

    Record record{ maxId + 1, std::move(name), std::move(value) };
    records_.push_back(record);
    return record;
}

bool JsonRepository::UpdateById(int id, const std::optional<std::string>& name, const std::optional<std::string>& value)
{
    for (auto& record : records_)
    {
        if (record.id == id)
        {
            if (name)
            {
                record.name = *name;
            }
            if (value)
            {
                record.value = *value;
            }
            return true;
        }
    }
    return false;
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
