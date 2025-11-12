cmake_minimum_required(VERSION 3.5)
project (lab05 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(COMMON_DIR "../common")
file(GLOB SOURCES
        *.cpp
)
file(GLOB HEADERS
        *.h
        ${COMMON_DIR}/*.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS}
        hash_table.h
        hash_table.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${COMMON_DIR})