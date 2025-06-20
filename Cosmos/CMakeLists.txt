cmake_minimum_required(VERSION 3.10)
project(GameProject LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Автоматический поиск файлов
file(GLOB CORE_SOURCES core/*.cpp)
file(GLOB CORE_HEADERS core/*.h)
file(GLOB UI_SOURCES ui/*.cpp)
file(GLOB UI_HEADERS ui/*.h)

set(SOURCES ${CORE_SOURCES} ${UI_SOURCES} main.cpp)
set(HEADERS ${CORE_HEADERS} ${UI_HEADERS})

add_executable(GameProject ${SOURCES} ${HEADERS})

# Копирование data только если папка существует
if(EXISTS "${CMAKE_SOURCE_DIR}/data")
    add_custom_command(TARGET GameProject POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:GameProject>/data"
    )
endif()

# Группировка файлов для Visual Studio
source_group("core" REGULAR_EXPRESSION "core/.*")
source_group("ui" REGULAR_EXPRESSION "ui/.*")

# (Опционально) установка
# install(TARGETS GameProject DESTINATION bin)
# install(DIRECTORY data/ DESTINATION bin/data)
