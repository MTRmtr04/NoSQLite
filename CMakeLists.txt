cmake_minimum_required(VERSION 3.22)

project(nosqlite)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
            main.cpp
            src/auxiliary.hpp
            src/auxiliary.cpp
            src/database.hpp
            src/database.cpp
            src/collection.hpp
            src/collection.cpp)


target_include_directories(nosqlite PRIVATE include)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)
