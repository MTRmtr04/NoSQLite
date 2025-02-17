cmake_minimum_required(VERSION 3.22)

project(nosqlite)

add_executable(${PROJECT_NAME}
            main.cpp
            src/auxiliary.hpp
            src/auxiliary.cpp
            src/database.hpp
            src/database.cpp
            src/collection.hpp
            src/collection.cpp)


add_subdirectory(lib)

target_link_libraries(${PROJECT_NAME} json_parse)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)
