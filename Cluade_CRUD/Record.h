#pragma once

#include <string>

#include "Json.h"

struct Record
{
    int id = 0;
    std::string name;
    std::string value;
};

ClaudeJson::JsonValue ToJson(const Record& record);
Record FromJson(const ClaudeJson::JsonValue& json);
