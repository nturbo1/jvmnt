add_library(class_check
    class_format.cpp
)

target_include_directories(class_check
    PUBLIC
        ${CMAKE_SOURCE_DIR}/base
        ${CMAKE_SOURCE_DIR}/class_file
)

target_link_libraries(class_check
    PUBLIC
        base
        class_file
)
