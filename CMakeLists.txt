cmake_minimum_required(VERSION 3.23)

project(
    system-threads
    VERSION 0.0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COMPILER_WARNING_AS_ERROR ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
)
