cmake_minimum_required(VERSION 3.20)

project(jvmnt)

include(cmake/configs.cmake)

add_executable(jvmnt main.cpp)

target_link_libraries(jvmnt
    PRIVATE
        base
        classFile
)

add_subdirectory(base)
add_subdirectory(classFile)
