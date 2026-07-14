# PLAN.md

CRUD 콘솔 애플리케이션 구현 계획. 데이터는 JSON 파일로 관리하며, 요구사항은 `CLAUDE.md`(원본: `docs/[CRA_AI] Day2_1_Agentic Engineering.pdf` p.32)를 따른다.

## 0. 사용할 JSON 라이브러리

자체 라이브러리인 `ClaudeJson`(`C:\Users\User\source\repos\ClaudeJson`)을 사용한다.

- 헤더: `C:\Users\User\source\repos\ClaudeJson\ClaudeJson\Json.h` — 네임스페이스 `ClaudeJson`, 핵심 타입 `JsonValue`.
- 정적 라이브러리(빌드 결과물): `C:\Users\User\source\repos\ClaudeJson\x64\Debug\ClaudeJson.lib`.
- 주요 API: `JsonValue::Parse(text)` / `JsonValue::Load(path)` / `value.Save(path, indent)` / `value.Dump(indent)`, 객체는 `value["key"]`, 배열은 `value[idx]` / `PushBack` / `Size()`, 값 추출은 `AsString()/AsNumber()/AsBool()/AsArray()/AsObject()`.
- `Object`는 `std::vector<std::pair<std::string, JsonValue>>`라 키 순서가 보존되고 조회는 선형 탐색 — 데이터 규모가 작은 이 과제에서는 문제 없음. `Number`는 항상 `double`이므로 ID 등 정수 필드는 `AsNumber()` 후 `static_cast<int>`로 변환.

### 프로젝트 연동 방법

이미 빌드된 `.lib`만 참조하는 방식으로 진행(소스를 복제하지 않음):

- `Cluade_CRUD.vcxproj`의 `AdditionalIncludeDirectories`에 `C:\Users\User\source\repos\ClaudeJson\ClaudeJson` 추가 → `#include "Json.h"` 가능하게 함.
- `AdditionalLibraryDirectories`(Linker)에 `C:\Users\User\source\repos\ClaudeJson\x64\Debug` 추가, `AdditionalDependencies`에 `ClaudeJson.lib` 추가.
- 주의: 참조하는 `.lib`은 `x64\Debug` 산출물이므로, 우선 `Debug|x64` 구성 기준으로 작업. Release/Win32 구성까지 맞추려면 ClaudeJson 쪽도 해당 구성으로 다시 빌드해 경로를 늘려야 함(추후 필요 시 처리).

## 1. 데이터 모델 정의 (`Record.h`)

- CRUD 대상 레코드 struct 정의: 예) `int id; std::string name; std::string value;` (실제 필드는 요구사항에 맞게 확정 필요 — 우선 범용 키-값 형태로 시작).
- `JsonValue ToJson(const Record&)` / `Record FromJson(const JsonValue&)` 형태의 변환 함수 작성 (ClaudeJson은 nlohmann 스타일의 ADL 직렬화 훅이 없으므로 직접 변환 함수를 둔다).

## 2. 저장소 계층 (`JsonRepository.h/.cpp`)

- JSON 파일 경로(예: `data.json`) 로드/저장 담당. 내부적으로 최상위 `JsonValue`는 `Array`.
- 앱 시작 시 파일이 없으면 `JsonValue(JsonValue::Array{})`로 새로 생성 후 `Save()`.
- 인메모리에 `std::vector<Record>` 유지, 아래 연산 제공:
  - `load()`: `JsonValue::Load(path)` → 배열 순회하며 `FromJson`으로 벡터 구성
  - `save()`: 벡터를 `JsonValue::Array`로 변환(`ToJson`) 후 `Save(path, 2)` (매 변경 후 즉시 저장하여 안전성 확보)
  - `getAll()`, `findById(int id)`
  - `add(Record)`: 신규 ID 자동 부여(기존 최대 ID + 1) 후 추가
  - `updateById(int id, ...)`: 필드 수정
  - `removeById(int id)`: 삭제 (삭제 전 존재 여부 확인 → 안전한 삭제)
- 파일 파싱 실패 시 `ClaudeJson::JsonParseError`를 잡아 사용자에게 안내(예: 파일 손상) 후 빈 목록으로 시작할지 종료할지 결정.

## 3. CRUD 기능 구현

- **Create**: 콘솔에서 필드 입력 받아 `Repository::add()` 호출 후 저장.
- **Read**:
  - 전체 목록 출력 메뉴
  - ID/키로 검색하는 메뉴 (`findById` 결과 없을 시 안내 메시지)
- **Update**: ID 입력 → 존재 확인 → 수정할 필드 선택 → 새 값 입력 → 저장.
- **Delete**: ID 입력 → 존재 확인 → 삭제 확인(y/n) → 삭제 후 저장.

## 4. 콘솔 메뉴 / 진입점 (`main.cpp`)

- 반복 메뉴 루프: `1) Create 2) Read(전체) 3) Read(검색) 4) Update 5) Delete 6) 종료`
- 입력 검증(숫자 파싱 실패, 존재하지 않는 ID 등) 처리.
- `ClaudeJson::JsonParseError` / `JsonTypeError` / `std::exception`을 최상위에서 catch하여 비정상 종료 방지.

## 5. 빌드 확인

- Debug|x64 구성으로 빌드하여 컴파일/링크 오류(특히 include/lib 경로) 없는지 확인.
- 실행 후 Create → Read(전체) → Update → Read(검색) → Delete 시나리오 수동 테스트로 JSON 파일 내용 검증.

## 순서 요약

1. `Cluade_CRUD.vcxproj`에 ClaudeJson include/lib 경로 및 `ClaudeJson.lib` 링크 설정 추가
2. Record 모델 + ToJson/FromJson 정의
3. JsonRepository 구현 (load/save/CRUD 메서드)
4. main.cpp에 메뉴 루프 및 각 CRUD 핸들러 연결
5. 빌드 및 수동 시나리오 테스트
