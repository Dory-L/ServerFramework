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
    dory/iomanager.cpp
    dory/timer.cpp
    dory/fd_manager.cpp
    dory/hook.cpp
    dory/address.cpp
    dory/socket.cpp
    dory/bytearray.cpp
    dory/tcp_server.cpp
    dory/stream.cpp
    dory/socket_stream.cpp
    dory/uri.rl.cpp
    dory/http/http.cpp
    dory/http/http11_parser.rl.cpp
    dory/http/httpclient_parser.rl.cpp
    dory/http/http_parser.cpp
    dory/http/http_session.cpp
    dory/http/http_server.cpp
    dory/http/servlet.cpp
    dory/http/http_connection.cpp
    )

ragelmaker(dory/http/http11_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/dory/http)
ragelmaker(dory/http/httpclient_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/dory/http)
ragelmaker(dory/uri.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/dory)

set(LIBS 
        dory
        dl
        pthread
        yaml-cpp)


add_library(dory SHARED ${LIB_SRC})
redefine_file_macro(dory)
add_executable(test tests/test.cpp)
redefine_file_macro(test)
add_dependencies(test dory)
target_link_libraries(test ${LIBS})

add_executable(test_config tests/test_config.cpp)
redefine_file_macro(test_config)
add_dependencies(test_config dory)
target_link_libraries(test_config ${LIBS})

add_executable(test_thread tests/test_thread.cpp)
redefine_file_macro(test_thread)
add_dependencies(test_thread dory)
target_link_libraries(test_thread ${LIBS})

add_executable(test_util tests/test_util.cpp)
redefine_file_macro(test_util)
add_dependencies(test_util dory)
target_link_libraries(test_util ${LIBS})

add_executable(test_fiber tests/test_fiber.cpp)
redefine_file_macro(test_fiber)
add_dependencies(test_fiber dory)
target_link_libraries(test_fiber ${LIBS})

add_executable(test_scheduler tests/test_scheduler.cpp)
redefine_file_macro(test_scheduler)
add_dependencies(test_scheduler dory)
target_link_libraries(test_scheduler ${LIBS})

add_executable(test_iomanager tests/test_iomanager.cpp)
redefine_file_macro(test_iomanager)
add_dependencies(test_iomanager dory)
target_link_libraries(test_iomanager ${LIBS})

add_executable(test_hook tests/test_hook.cpp)
redefine_file_macro(test_hook)
add_dependencies(test_hook dory)
target_link_libraries(test_hook ${LIBS})

add_executable(test_address tests/test_address.cpp)
redefine_file_macro(test_address)
add_dependencies(test_address dory)
target_link_libraries(test_address ${LIBS})

add_executable(test_socket tests/test_socket.cpp)
redefine_file_macro(test_socket)
add_dependencies(test_socket dory)
target_link_libraries(test_socket ${LIBS})

add_executable(test_bytearray tests/test_bytearray.cpp)
redefine_file_macro(test_bytearray)
add_dependencies(test_bytearray dory)
target_link_libraries(test_bytearray ${LIBS})

add_executable(test_http tests/test_http.cpp)
redefine_file_macro(test_http)
add_dependencies(test_http dory)
target_link_libraries(test_http ${LIBS})

add_executable(test_http_parser tests/test_http_parser.cpp)
redefine_file_macro(test_http_parser)
add_dependencies(test_http_parser dory)
target_link_libraries(test_http_parser ${LIBS})

add_executable(test_tcp_server tests/test_tcp_server.cpp)
redefine_file_macro(test_tcp_server)
add_dependencies(test_tcp_server dory)
target_link_libraries(test_tcp_server ${LIBS})

add_executable(test_http_server tests/test_http_server.cpp)
redefine_file_macro(test_http_server)
add_dependencies(test_http_server dory)
target_link_libraries(test_http_server ${LIBS})

add_executable(test_http_connection tests/test_http_connection.cpp)
redefine_file_macro(test_http_connection)
add_dependencies(test_http_connection dory)
target_link_libraries(test_http_connection ${LIBS})

add_executable(test_uri tests/test_uri.cpp)
redefine_file_macro(test_uri)
add_dependencies(test_uri dory)
target_link_libraries(test_uri ${LIBS})

add_executable(echo_server examples/echo_server.cpp)
redefine_file_macro(echo_server)
add_dependencies(echo_server dory)
target_link_libraries(echo_server ${LIBS})


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
