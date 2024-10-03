cmake_minimum_required(VERSION 3.29)
project(chext-test)

add_compile_definitions(SC_DISABLE_API_VERSION_CHECK)

find_package(SystemCLanguage REQUIRED)
find_package(verilator REQUIRED)
find_package(fmt REQUIRED)

add_executable(exec01)
target_sources(exec01 PUBLIC src/main.cpp)
target_include_directories(exec01 PUBLIC include/)
target_link_libraries(exec01 PUBLIC SystemC::systemc fmt::fmt)

verilate(
    exec01
    SYSTEMC
    SOURCES ElasticModule.sv
    VERILATOR_ARGS --trace
)