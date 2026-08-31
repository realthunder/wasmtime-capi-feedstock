# Consumes the package the way an embedder does: find the library, include
# wasmtime.h, link, run.
cmake_minimum_required(VERSION 3.20)
project(wasmtime_capi_consumer C)

find_path(WASMTIME_INCLUDE_DIR NAMES wasmtime.h REQUIRED)
find_library(WASMTIME_LIBRARY NAMES wasmtime REQUIRED)
message(STATUS "wasmtime headers: ${WASMTIME_INCLUDE_DIR}")
message(STATUS "wasmtime library: ${WASMTIME_LIBRARY}")

add_executable(consumer consumer.c)
target_include_directories(consumer PRIVATE ${WASMTIME_INCLUDE_DIR})
target_link_libraries(consumer PRIVATE ${WASMTIME_LIBRARY})

enable_testing()
add_test(NAME consumer COMMAND consumer)
