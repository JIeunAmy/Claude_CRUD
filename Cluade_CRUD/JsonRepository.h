#pragma once

#include <string>
#include <vector>

#include "Record.h"

class JsonRepository
{
public:
    explicit JsonRepository(std::string path);

    // Loads records from the JSON file. If the file does not exist yet, starts empty.
    void Load();

    // Writes all in-memory records back to the JSON file. Returns false on write failure.
    bool Save() const;

    // Adds a new record with an auto-assigned ID (max existing ID + 1, or 1 if empty).
    const Record& Add(std::string name, std::string value);

private:
    std::string path_;
    std::vector<Record> records_;
};
