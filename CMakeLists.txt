cmake_minimum_required(VERSION 3.22)

project(tordbot VERSION 0.1 DESCRIPTION "Truth or Dare discord bot")

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_executable(tordbot
	src/main.cxx
	src/bot.cxx
)
target_include_directories(tordbot PRIVATE include)

find_package(DPP REQUIRED)
target_link_libraries(tordbot PRIVATE ${DPP_LIBRARIES})
target_include_directories(tordbot PRIVATE ${DPP_INCLUDE_DIR})

find_package(SQLite3 REQUIRED)
target_link_libraries(tordbot PRIVATE SQLite::SQLite3)

include(FetchContent)
FetchContent_Declare(
	cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
	GIT_TAG dd967cb48ea6bcbad9f1da5ada0db8ac0d532c06
)
FetchContent_MakeAvailable(cpr)
target_link_libraries(tordbot PRIVATE cpr::cpr)

FetchContent_Declare(
	json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(json)
target_link_libraries(tordbot PRIVATE nlohmann_json::nlohmann_json)

add_subdirectory(third_party/cpp-dotenv)
target_link_libraries(tordbot PRIVATE cpp_dotenv)
