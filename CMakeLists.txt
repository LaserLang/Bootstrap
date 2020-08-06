cmake_minimum_required(VERSION 3.15)
project(laserc-bootstrap CXX)

set(LASERC_VERSION 0.1)

set(LLVM_ENABLE_PROJECTS "" CACHE STRING "Subprojects to be built with llvm")
option(LLVM_ENABLE_BINDINGS "By default, llvm bindings are not built with laserc" OFF)

add_subdirectory(llvm-project/llvm EXCLUDE_FROM_ALL)

set(LASERC_HOST_TRIPLE ${LLVM_HOST_TRIPLE})

set(LASERC_TARGET_TRIPLES ${LASERC_HOST_TRIPLE} CACHE STRING "The list of laser targets to compile the standard library for")

list(FIND LASERC_TARGET_TRIPLES ${LASERC_HOST_TRIPLE} _LASERC_TARGET_HOST_POS)

if(_LASERC_TARGET_HOST_POS GREATER -1)
    set(LASERC_DEFAULT_TARGET_TRIPLE_INIT ${LASERC_HOST_TRIPLE})
    # If the target list contains the host, set the default target triple to the host triple
else()
    list(GET LASERC_TARGET_TRIPLES 0 LASERC_DEFAULT_TARGET_TRIPLE_INIT)
    # Otherwise, use the first triple by default
endif()
set(LASERC_DEFAULT_TARGET_TRIPLE ${LASERC_DEFAULT_TARGET_TRIPLE_INIT} CACHE STRING "The default target to build when invoking laserc without --target")
mark_as_advanced(LASERC_DEFAULT_TARGET_TRIPLE)
set_property(CACHE LASERC_DEFAULT_TARGET_TRIPLE PROPERTY STRINGS ${LASERC_TARGET_TRIPLES})

set(LASERC_DEFAULT_LINKER "" CACHE STRING "The default linker to use with laserc")

list(JOIN LASERC_TARGET_TRIPLES ", " _laser_target_commas)

message(STATUS "Building Laserc on host: ${LASERC_HOST_TRIPLE}")
message(STATUS "Building laser targets for: ${_laser_target_commas}")
message(STATUS "Compiling for ${LASERC_DEFAULT_TARGET_TRIPLE} by default")

configure_file(Config.hpp.in Config.hpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(laserc-bootstrap src/main.cpp src/token.cpp src/token.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Config.hpp
        src/mode.hpp)

target_link_libraries(laserc-bootstrap LLVMCore)

