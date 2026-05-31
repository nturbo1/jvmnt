add_library(class_file
    attributes.cpp
    class_file.cpp
    const_pool.cpp
)

target_include_directories(class_file
    PUBLIC
        ${CMAKE_SOURCE_DIR}/base
)

target_link_libraries(class_file
    PUBLIC
        base
)
