cmake_minimum_required(VERSION 3.15)
project(chext_test)

set(CMAKE_CXX_STANDARD 20)

find_package(SystemCLanguage REQUIRED)
find_package(fmt REQUIRED)
find_package(hdlstuff_hal)

# set(VERILATOR_ARGS --pins-bv 2 --trace -Wno-SYMRSVDWORD)
set(VERILATOR_ARGS --pins-bv 2 -Wno-SYMRSVDWORD)

add_library(
    chext_test
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util/hal.cpp
)

target_include_directories(
    chext_test
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(
    chext_test
    PUBLIC
    SystemC::systemc
    fmt::fmt
    hdlstuff::hdlstuff_hal
)

add_executable(test1 test/test1.cpp)
target_link_libraries(test1 PUBLIC chext_test)
add_compile_definitions(test1 PUBLIC SC_DISABLE_API_VERSION_CHECK)

add_executable(test2 test/test2.cpp)
target_link_libraries(test2 PUBLIC SystemC::systemc fmt::fmt)
target_include_directories(test2 PUBLIC include/)

add_executable(test3 test/test3.cpp)
target_link_libraries(test3 PUBLIC chext_test)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/chext_test-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/chext_test-config.cmake @ONLY
)

install(
    TARGETS chext_test
    EXPORT chext_test-targets
    DESTINATION lib
)

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include"
    DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

install(
    EXPORT chext_test-targets
    NAMESPACE chext::
    DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/chext_test"
    COMPONENT dev
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/chext_test-config.cmake
    DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/chext_test"
)

export(PACKAGE chext_test)
