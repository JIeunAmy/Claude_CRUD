#include <windows.h>

#include <iostream>
#include <limits>
#include <optional>
#include <string>

#include "Json.h"
#include "JsonRepository.h"

namespace
{
    const std::string kDataFilePath = "data.json";

    // 입력 스트림이 끝났을 때(EOF/오류) 무한 재입력 루프에 빠지지 않도록 던지는 예외.
    struct EndOfInput
    {
    };

    void PrintRecord(const Record& record)
    {
        std::cout << "  id=" << record.id << ", name=" << record.name << ", value=" << record.value << "\n";
    }

    std::string ReadLine(const std::string& prompt)
    {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line))
        {
            throw EndOfInput{};
        }
        return line;
    }

    // 파싱에 성공할 때까지 재입력을 요구한다.
    int ReadInt(const std::string& prompt)
    {
        while (true)
        {
            std::string line = ReadLine(prompt);
            try
            {
                size_t consumed = 0;
                int value = std::stoi(line, &consumed);
                if (consumed == line.size())
                {
                    return value;
                }
            }
            catch (const std::exception&)
            {
                // 아래에서 재입력 안내
            }
            std::cout << "숫자를 정확히 입력해주세요.\n";
        }
    }

    void HandleCreate(JsonRepository& repo)
    {
        std::string name;
        while (name.empty())
        {
            name = ReadLine("name: ");
            if (name.empty())
            {
                std::cout << "name은 비워둘 수 없습니다.\n";
            }
        }
        std::string value = ReadLine("value: ");

        Record created = repo.Add(name, value);
        if (!repo.Save())
        {
            std::cout << "파일 저장에 실패했습니다. 메모리 상의 변경은 유지됩니다.\n";
        }

        std::cout << "생성되었습니다.\n";
        PrintRecord(created);
    }

    void HandleReadAll(const JsonRepository& repo)
    {
        const auto& all = repo.GetAll();
        if (all.empty())
        {
            std::cout << "데이터가 없습니다.\n";
            return;
        }
        for (const auto& record : all)
        {
            PrintRecord(record);
        }
    }

    void HandleReadById(const JsonRepository& repo)
    {
        int id = ReadInt("조회할 id: ");
        const Record* found = repo.FindById(id);
        if (!found)
        {
            std::cout << "해당 ID의 데이터가 없습니다.\n";
            return;
        }
        PrintRecord(*found);
    }

    void HandleUpdate(JsonRepository& repo)
    {
        int id = ReadInt("수정할 id: ");
        const Record* found = repo.FindById(id);
        if (!found)
        {
            std::cout << "해당 ID의 데이터가 없습니다.\n";
            return;
        }

        std::cout << "현재 값:\n";
        PrintRecord(*found);

        std::string choiceLine = ReadLine("수정할 필드를 선택하세요 (1) name  2) value): ");

        std::optional<std::string> newName;
        std::optional<std::string> newValue;

        if (choiceLine == "1")
        {
            std::string input = ReadLine("새 name (빈 입력 시 기존 값 유지): ");
            if (!input.empty())
            {
                newName = input;
            }
        }
        else if (choiceLine == "2")
        {
            std::string input = ReadLine("새 value (빈 입력 시 기존 값 유지): ");
            if (!input.empty())
            {
                newValue = input;
            }
        }
        else
        {
            std::cout << "잘못된 선택입니다. 취소합니다.\n";
            return;
        }

        repo.UpdateById(id, newName, newValue);

        if (!repo.Save())
        {
            std::cout << "파일 저장에 실패했습니다. 다음 저장 시도까지 메모리 상의 변경 사항이 유지됩니다.\n";
        }

        std::cout << "수정되었습니다.\n";
        PrintRecord(*repo.FindById(id));
    }

    void HandleDelete(JsonRepository& repo)
    {
        int id = ReadInt("삭제할 id: ");
        const Record* found = repo.FindById(id);
        if (!found)
        {
            std::cout << "해당 ID의 데이터가 없습니다.\n";
            return;
        }

        std::cout << "삭제할 레코드:\n";
        PrintRecord(*found);

        std::string confirm = ReadLine("정말 삭제하시겠습니까? (y/n): ");
        if (confirm.empty() || (confirm != "y" && confirm != "Y"))
        {
            std::cout << "삭제를 취소했습니다.\n";
            return;
        }

        repo.RemoveById(id);

        if (!repo.Save())
        {
            std::cout << "파일 저장에 실패했습니다. 파일과 메모리 상태가 일치하지 않을 수 있습니다.\n";
        }

        std::cout << "삭제되었습니다.\n";
    }

    void PrintMenu()
    {
        std::cout << "\n1) Create  2) Read(전체)  3) Read(검색)  4) Update  5) Delete  6) 종료\n선택: ";
    }
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    JsonRepository repo(kDataFilePath);

    try
    {
        repo.Load();
    }
    catch (const ClaudeJson::JsonParseError& e)
    {
        std::cout << "데이터 파일이 손상되어 있습니다: " << e.what() << "\n";
        std::cout << "프로그램을 종료합니다.\n";
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
        std::string choice;

        try
        {
            choice = ReadLine("");
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
        catch (const EndOfInput&)
        {
            break;
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
