add_library(classParser
    parser.cpp
    classReader.cpp
    print.cpp
)

target_include_directories(classParser
    PUBLIC
        ${CMAKE_SOURCE_DIR}/base
        ${CMAKE_SOURCE_DIR}/classFile
)

target_link_libraries(classParser
    PUBLIC
        base
        classFile
)
