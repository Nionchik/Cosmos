cmake_minimum_required(VERSION 3.10)
project(Cosmos)  # Название проекта

# Явно указываем C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Обязательное использование C++14

# Включаем все исходники из core/, ui/ и main.cpp
file(GLOB_RECURSE SOURCES
    "core/*.cpp"
    "ui/*.cpp"
    "main.cpp"
)

# Подключаем заголовки (core/, ui/)
include_directories(
    core/
    ui/
)

# Создаём исполняемый файл
add_executable(Cosmos ${SOURCES})

# Копируем папку data/ в билд-директорию (чтобы программа видела ресурсы)
file(GLOB DATA_FILES "data/*.*")  # Все файлы из data/
add_custom_command(
    TARGET Cosmos POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/data
    COMMAND ${CMAKE_COMMAND} -E copy ${DATA_FILES} ${CMAKE_BINARY_DIR}/data
    COMMENT "Копирование ресурсов в build/data/"
)

# Опционально: настройка для Visual Studio (если нужно)
if(MSVC)
    target_compile_options(Cosmos PRIVATE /W4 /WX)  # Включить все предупреждения и трактовать их как ошибки
endif()