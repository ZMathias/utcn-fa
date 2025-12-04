cmake_minimum_required(VERSION 4.0)
project (lab09)

set(CMAKE_CXX_STANDARD 23)
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
        bfs.cpp
        bfs.h)

target_include_directories(${PROJECT_NAME} PRIVATE ${COMMON_DIR})