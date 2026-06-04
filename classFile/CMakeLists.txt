add_library(classFile
    attributes.cpp
    classFile.cpp
    constPool.cpp
    classFileParser.cpp
    classReader.cpp
    print.cpp
)

target_include_directories(classFile
    PUBLIC
        ${CMAKE_SOURCE_DIR}/base
)

target_link_libraries(classFile
    PUBLIC
        base
)
