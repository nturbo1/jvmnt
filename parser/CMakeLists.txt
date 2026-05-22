add_library(class_parser
    parser.cpp
    class_reader.cpp
    const_pool.cpp
)

target_include_directories(class_parser
    PUBLIC
        ${CMAKE_SOURCE_DIR}/base
)

target_link_libraries(class_parser
    PUBLIC
        base
)
