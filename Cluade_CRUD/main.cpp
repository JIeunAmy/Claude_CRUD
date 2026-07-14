#include <iostream>
#include <limits>
#include <optional>
#include <string>

#include "JsonRepository.h"
#include "Json.h"

namespace
{
    const char* kDataFilePath = "data.json";

    std::string ReadLine(const std::string& prompt)
    {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);
        return line;
    }

    std::optional<int> TryParseInt(const std::string& text)
    {
        try
        {
            size_t pos = 0;
            int value = std::stoi(text, &pos);
            if (pos != text.size())
            {
                return std::nullopt;
            }
            return value;
        }
        catch (const std::exception&)
        {
            return std::nullopt;
        }
    }

    int ReadId(const std::string& prompt)
    {
        while (true)
        {
            std::string text = ReadLine(prompt);
            if (auto id = TryParseInt(text))
            {
                return *id;
            }
            std::cout << "숫자로 된 ID를 입력해주세요.\n";
        }
    }

    bool ReadYesNo(const std::string& prompt)
    {
        std::string answer = ReadLine(prompt);
        return !answer.empty() && (answer[0] == 'y' || answer[0] == 'Y');
    }

    void PrintRecord(const Record& record)
    {
        std::cout << "  id=" << record.id << ", name=" << record.name << ", value=" << record.value << "\n";
    }

    void PrintAll(const JsonRepository& repo)
    {
        const std::vector<Record>& records = repo.GetAll();
        if (records.empty())
        {
            std::cout << "데이터가 없습니다.\n";
            return;
        }
        for (const Record& record : records)
        {
            PrintRecord(record);
        }
    }

    void HandleCreate(JsonRepository& repo)
    {
        std::string name = ReadLine("name: ");
        while (name.empty())
        {
            std::cout << "name은 비어 있을 수 없습니다.\n";
            name = ReadLine("name: ");
        }

        std::string value = ReadLine("value: ");
        while (value.empty())
        {
            std::cout << "value는 비어 있을 수 없습니다.\n";
            value = ReadLine("value: ");
        }

        const Record& created = repo.Add(name, value);
        if (!repo.Save())
        {
            std::cout << "파일 저장에 실패했습니다. (메모리에는 반영됨)\n";
            return;
        }
        std::cout << "생성 완료:\n";
        PrintRecord(created);
    }

    void HandleReadAll(const JsonRepository& repo)
    {
        PrintAll(repo);
    }

    void HandleReadById(const JsonRepository& repo)
    {
        int id = ReadId("조회할 ID: ");
        const Record* record = repo.FindById(id);
        if (!record)
        {
            std::cout << "해당 ID의 데이터가 없습니다.\n";
            return;
        }
        PrintRecord(*record);
    }

    void HandleUpdate(JsonRepository& repo)
    {
        int id = ReadId("수정할 ID: ");
        const Record* existing = repo.FindById(id);
        if (!existing)
        {
            std::cout << "해당 ID의 데이터가 없습니다.\n";
            return;
        }

        std::cout << "현재 값:\n";
        PrintRecord(*existing);

        std::string name = ReadLine("새 name (변경 없으면 엔터): ");
        std::string value = ReadLine("새 value (변경 없으면 엔터): ");

        repo.UpdateById(id,
            name.empty() ? std::nullopt : std::optional<std::string>(name),
            value.empty() ? std::nullopt : std::optional<std::string>(value));

        if (!repo.Save())
        {
            std::cout << "파일 저장에 실패했습니다. (메모리에는 반영됨)\n";
            return;
        }

        std::cout << "수정 완료:\n";
        PrintRecord(*repo.FindById(id));
    }

    void HandleDelete(JsonRepository& repo)
    {
        int id = ReadId("삭제할 ID: ");
        const Record* existing = repo.FindById(id);
        if (!existing)
        {
            std::cout << "해당 ID의 데이터가 없습니다.\n";
            return;
        }

        std::cout << "삭제할 레코드:\n";
        PrintRecord(*existing);

        if (!ReadYesNo("정말 삭제하시겠습니까? (y/n): "))
        {
            std::cout << "삭제를 취소했습니다.\n";
            return;
        }

        repo.RemoveById(id);

        if (!repo.Save())
        {
            std::cout << "파일 저장에 실패했습니다. 메모리에서는 이미 삭제되었으나, "
                         "다음 저장 성공 시까지 파일 내용과 다를 수 있습니다.\n";
            return;
        }

        std::cout << "삭제가 완료되었습니다.\n";
    }

    void PrintMenu()
    {
        std::cout << "\n1) Create  2) Read(전체)  3) Read(ID 검색)  4) Update  5) Delete  6) 종료\n";
    }
}

int main()
{
    JsonRepository repo(kDataFilePath);

    try
    {
        repo.Load();
    }
    catch (const ClaudeJson::JsonParseError& e)
    {
        std::cout << "데이터 파일이 손상되었습니다: " << e.what() << "\n";
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cout << "데이터 파일을 불러오는 중 오류가 발생했습니다: " << e.what() << "\n";
        return 1;
    }

    while (true)
    {
        PrintMenu();
        std::string choice = ReadLine("선택: ");

        try
        {
            if (choice == "1")
            {
                HandleCreate(repo);
            }
            else if (choice == "2")
            {
                HandleReadAll(repo);
            }
            else if (choice == "3")
            {
                HandleReadById(repo);
            }
            else if (choice == "4")
            {
                HandleUpdate(repo);
            }
            else if (choice == "5")
            {
                HandleDelete(repo);
            }
            else if (choice == "6")
            {
                break;
            }
            else
            {
                std::cout << "1~6 사이의 번호를 입력해주세요.\n";
            }
        }
        catch (const ClaudeJson::JsonTypeError& e)
        {
            std::cout << "데이터 형식 오류: " << e.what() << "\n";
        }
        catch (const std::exception& e)
        {
            std::cout << "오류가 발생했습니다: " << e.what() << "\n";
        }
    }

    return 0;
}
