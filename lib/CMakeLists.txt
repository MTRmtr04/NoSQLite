add_library(json_parse simdjson.cpp simdjson.h)

target_include_directories(json_parse
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})
