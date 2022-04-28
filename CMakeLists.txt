cmake_minimum_required(VERSION 2.8)
project(dory)

include (cmake/utils.cmake)

add_definitions(-Wno-builtin-macro-redefined)#允许宏重定义

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -pthread")

include_directories(/usr/local/include)

link_directories(/usr/local/lib)

#FIND_LIBRARY(YAML_CPP_LIBRARIES yaml-cpp)

set(LIB_SRC
    dory/log.cpp
    dory/util.cpp
    dory/config.cpp
    dory/Thread.cpp
    dory/fiber.cpp
    dory/scheduler.cpp
    )

add_library(dory SHARED ${LIB_SRC})
redefine_file_macro(dory)
add_executable(test tests/test.cpp)
redefine_file_macro(test)
add_dependencies(test dory)
target_link_libraries(test dory yaml-cpp)

add_executable(test_config tests/test_config.cpp)
redefine_file_macro(test_config)
add_dependencies(test_config dory)
target_link_libraries(test_config dory yaml-cpp)

add_executable(test_thread tests/test_thread.cpp)
redefine_file_macro(test_thread)
add_dependencies(test_thread dory)
target_link_libraries(test_thread dory yaml-cpp)

add_executable(test_util tests/test_util.cpp)
redefine_file_macro(test_util)
add_dependencies(test_util dory)
target_link_libraries(test_util dory yaml-cpp)

add_executable(test_fiber tests/test_fiber.cpp)
redefine_file_macro(test_fiber)
add_dependencies(test_fiber dory)
target_link_libraries(test_fiber dory yaml-cpp)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
