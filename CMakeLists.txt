cmake_minimum_required(VERSION 3.22)

project(nosqlite CXX) # Specify CXX language for the project

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
            main.cpp
            src/auxiliary.hpp
            src/auxiliary.cpp
            src/database.hpp
            src/database.cpp
            src/collection.hpp
            src/collection.cpp
            src/hash_index.hpp
            src/hash_index.cpp
            src/nosqlite.hpp
            src/nosqlite.cpp)

# --- OpenMP Configuration ---
find_package(OpenMP REQUIRED) # Find OpenMP on the system

if (OpenMP_FOUND)
    message(STATUS "OpenMP found and enabled for ${PROJECT_NAME}.")
    # Link the OpenMP C++ library to your executable target
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
    # Add the necessary compile options for OpenMP
    target_compile_options(${PROJECT_NAME} PRIVATE ${OpenMP_CXX_FLAGS})
else()
    message(FATAL_ERROR "OpenMP not found! Please install an OpenMP-capable compiler or library.")
endif()
# --- End OpenMP Configuration ---

target_include_directories(${PROJECT_NAME} PRIVATE include)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)
