#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Record.h"

class JsonRepository
{
public:
    explicit JsonRepository(std::string path);

    // Loads records from the JSON file. If the file does not exist yet, starts
    // with an empty in-memory list (the file is created on the first Save()).
    void Load();

    // Writes the in-memory records back to the JSON file. Returns false if the
    // file could not be opened for writing.
    bool Save();

    const std::vector<Record>& GetAll() const;
    const Record* FindById(int id) const;

    Record& Add(std::string name, std::string value);
    bool UpdateById(int id, std::optional<std::string> name, std::optional<std::string> value);
    bool RemoveById(int id);

private:
    std::string path_;
    std::vector<Record> records_;
};
