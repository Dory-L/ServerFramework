cmake_minimum_required(VERSION 2.8)
project(dory)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC
    dory/log.cpp
    dory/util.cpp
    )

add_library(dory SHARED ${LIB_SRC})

add_executable(test tests/test.cpp)
add_dependencies(test dory)
target_link_libraries(test dory)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
