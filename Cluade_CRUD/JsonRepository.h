#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Record.h"

// JSON 파일(최상위 배열)에 레코드를 로드/저장하고, 인메모리 std::vector<Record>에 대한
// CRUD 연산을 제공한다. 저장소는 항상 인메모리 상태를 우선하며, Save()가 호출될 때만
// 파일에 즉시 반영된다.
class JsonRepository
{
public:
    explicit JsonRepository(std::string path);

    // 파일이 없으면 빈 배열로 새로 생성한다. 파일이 손상되어 있으면
    // ClaudeJson::JsonParseError가 그대로 전파된다.
    void Load();

    // 현재 인메모리 상태를 파일에 기록한다. 실패 시 false.
    bool Save() const;

    const std::vector<Record>& GetAll() const { return records_; }
    const Record* FindById(int id) const;

    // 기존 레코드들의 최대 ID + 1을 새 ID로 부여한다.
    Record Add(std::string name, std::string value);

    // name/value 중 값이 있는 항목만 갱신한다(비어 있으면 기존 값 유지). ID를 찾지 못하면 false.
    bool UpdateById(int id, const std::optional<std::string>& name, const std::optional<std::string>& value);

    // ID를 찾지 못하면 false, 아무것도 지우지 않는다.
    bool RemoveById(int id);

private:
    std::string path_;
    std::vector<Record> records_;
};
