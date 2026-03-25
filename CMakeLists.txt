cmake_minimum_required(VERSION 3.20)
project(SteamGameFinder LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(SteamGameFinder
        main.cpp
        database/GameDB.cpp
        database/game_query.cpp
        resources/sqlite/sqlite3.c
        UI/Window.cpp
        UI/Window.h
        UI/Window.ui
)

target_include_directories(SteamGameFinder PRIVATE
        database
        resources/sqlite
)

target_link_libraries(SteamGameFinder PRIVATE
        Qt6::Widgets
)