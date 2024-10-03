cmake_minimum_required(VERSION 3.28)
project(chext-test)

add_compile_definitions(SC_DISABLE_API_VERSION_CHECK)

find_package(SystemCLanguage REQUIRED)
find_package(verilator REQUIRED)
find_package(fmt REQUIRED)

add_executable(ElasticModule.tb)
target_sources(ElasticModule.tb PUBLIC src/ElasticModule.tb.cpp)
target_include_directories(ElasticModule.tb PUBLIC include/)
target_link_libraries(ElasticModule.tb PUBLIC SystemC::systemc fmt::fmt)

verilate(
    ElasticModule.tb
    SYSTEMC
    SOURCES ElasticModule.sv
    VERILATOR_ARGS --trace
)

add_executable(AxiModule.tb)
target_sources(AxiModule.tb PUBLIC src/AxiModule.tb.cpp)
target_include_directories(AxiModule.tb PUBLIC include/)
target_link_libraries(AxiModule.tb PUBLIC SystemC::systemc fmt::fmt)

verilate(
    AxiModule.tb
    SYSTEMC
    SOURCES AxiModule.sv
    VERILATOR_ARGS --trace --pins-bv 2
)
