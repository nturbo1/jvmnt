add_library(class_parser
    parser.cpp
)

target_include_directories(class_parser
    PUBLIC
        ${CMAKE_SOURCE_DIR}/base
)
