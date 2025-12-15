cmake_minimum_required(VERSION 4.1)
project (lab10)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(COMMON_DIR "../common")
file(GLOB SOURCES
        *.cpp
)
file(GLOB HEADERS
        *.h
        ${COMMON_DIR}/*.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS}
        dfs.cpp
        dfs.h)

target_include_directories(${PROJECT_NAME} PRIVATE ${COMMON_DIR})