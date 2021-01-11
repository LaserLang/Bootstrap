cmake_minimum_required(VERSION 3.15)
project(laserc-bootstrap CXX)

set(LASERC_VERSION 0.1)

set(LLVM_ABI_BREAKING_CHECKS FORCE_OFF CACHE STRING "By default, disable ABI breaking checks")
set(LLVM_ENABLE_PROJECTS "" CACHE STRING "Subprojects to be built with llvm")
option(LLVM_ENABLE_BINDINGS "By default, llvm bindings are not built with laserc" OFF)

add_subdirectory(llvm-project/llvm EXCLUDE_FROM_ALL)

if(NOT DEFINED LCCC_DETECTED_HOST_TARGET)
add_subdirectory(target)
endif()

set(LASERC_HOST_TRIPLE ${LCCC_DETECTED_HOST_TARGET})

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

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

include(CheckCXXCompilerFlag)

if(NOT DEFINED LASERC_HAS_WERROR)
    check_cxx_compiler_flag("-Werror" LASERC_HAS_WERROR)
endif()

if(LASERC_HAS_WERROR)
    add_compile_options("-Werror")
endif()

if(NOT DEFINED LASERC_HAS_PEDANTIC_ERRORS)
    check_cxx_compiler_flag("-pedantic-errors" LASERC_HAS_PEDANTIC_ERRORS)
endif()

if(LASERC_HAS_PEDANTIC_ERRORS)
    add_compile_options("-pedantic-errors")
endif()

if(NOT DEFINED LASERC_HAS_WALL)
    check_cxx_compiler_flag("-Wall" LASERC_HAS_WALL)
endif()

if(LASERC_HAS_WALL)
    add_compile_options("-Wall")
endif()
add_executable(laserc-bootstrap src/main.cpp src/lex.cpp src/lex.hpp src/token.cpp src/token.hpp src/ast.cpp src/ast.hpp src/parser.cpp src/parser.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Config.hpp
        src/mode.hpp)

target_include_directories(laserc-bootstrap PRIVATE llvm-project/llvm/include
        ${CMAKE_CURRENT_BINARY_DIR}/llvm-project/llvm/include)

target_link_libraries(laserc-bootstrap LLVMCore LLVMTarget)

