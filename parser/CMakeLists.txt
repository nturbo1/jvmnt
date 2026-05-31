add_library(class_parser
    parser.cpp
    class_reader.cpp
    print.cpp
)

target_include_directories(class_parser
    PUBLIC
        ${CMAKE_SOURCE_DIR}/base
        ${CMAKE_SOURCE_DIR}/class_file
)

target_link_libraries(class_parser
    PUBLIC
        base
        class_file
)
