#include <iostream>
#include <limits>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "Json.h"
#include "JsonRepository.h"

namespace
{
    const std::string kDataFilePath = "data.json";

    std::string PromptNonEmpty(const std::string& label)
    {
        std::string input;
        for (;;)
        {
            std::cout << label;
            std::getline(std::cin, input);
            if (!input.empty())
            {
                return input;
            }
            std::cout << "값을 입력해야 합니다. 다시 입력해 주세요.\n";
        }
    }

    void SaveWithRetry(JsonRepository& repository)
    {
        while (!repository.Save())
        {
            std::cout << "파일 저장에 실패했습니다. 다시 시도하시겠습니까? (y/n): ";
            std::string answer;
            std::getline(std::cin, answer);
            if (answer != "y" && answer != "Y")
            {
                std::cout << "저장 없이 계속합니다. 변경 내용은 메모리에만 남아 있습니다.\n";
                return;
            }
        }
    }

    void HandleCreate(JsonRepository& repository)
    {
        std::string name = PromptNonEmpty("name: ");
        std::string value = PromptNonEmpty("value: ");

        const Record& created = repository.Add(name, value);
        SaveWithRetry(repository);

        std::cout << "레코드가 생성되었습니다. (id=" << created.id << ")\n";
    }

    void PrintMenu()
    {
        std::cout << "\n1) Create\n2) 종료\n선택: ";
    }
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    JsonRepository repository(kDataFilePath);

    try
    {
        repository.Load();
    }
    catch (const ClaudeJson::JsonParseError& e)
    {
        std::cout << "데이터 파일이 손상되었습니다: " << e.what() << "\n";
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cout << "데이터 파일을 읽는 중 오류가 발생했습니다: " << e.what() << "\n";
        return 1;
    }

    for (;;)
    {
        PrintMenu();

        std::string choice;
        std::getline(std::cin, choice);

        try
        {
            if (choice == "1")
            {
                HandleCreate(repository);
            }
            else if (choice == "2")
            {
                break;
            }
            else
            {
                std::cout << "올바른 메뉴 번호를 입력해 주세요.\n";
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
