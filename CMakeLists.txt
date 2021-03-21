cmake_minimum_required(VERSION 3.15)
project(cannon-bootstrap CXX)

set(CANNON_VERSION 0.1)

set(LLVM_ABI_BREAKING_CHECKS FORCE_OFF CACHE STRING "By default, disable ABI breaking checks")
set(LLVM_ENABLE_PROJECTS "" CACHE STRING "Subprojects to be built with llvm")
option(LLVM_ENABLE_BINDINGS "By default, llvm bindings are not built with cannon" OFF)

add_subdirectory(llvm-project/llvm EXCLUDE_FROM_ALL)

if(NOT DEFINED LCCC_DETECTED_HOST_TARGET)
add_subdirectory(target)
endif()

set(CANNON_HOST_TRIPLE ${LCCC_DETECTED_HOST_TARGET})

set(CANNON_TARGET_TRIPLES ${CANNON_HOST_TRIPLE} CACHE STRING "The list of Cannon targets to compile the standard library for")

list(FIND CANNON_TARGET_TRIPLES ${CANNON_HOST_TRIPLE} _CANNON_TARGET_HOST_POS)

if(_CANNON_TARGET_HOST_POS GREATER -1)
    set(CANNON_DEFAULT_TARGET_TRIPLE_INIT ${CANNON_HOST_TRIPLE})
    # If the target list contains the host, set the default target triple to the host triple
else()
    list(GET CANNON_TARGET_TRIPLES 0 CANNON_DEFAULT_TARGET_TRIPLE_INIT)
    # Otherwise, use the first triple by default
endif()
set(CANNON_DEFAULT_TARGET_TRIPLE ${CANNON_DEFAULT_TARGET_TRIPLE_INIT} CACHE STRING "The default target to build when invoking cannon without --target")
mark_as_advanced(CANNON_DEFAULT_TARGET_TRIPLE)
set_property(CACHE CANNON_DEFAULT_TARGET_TRIPLE PROPERTY STRINGS ${CANNON_TARGET_TRIPLES})

set(CANNON_DEFAULT_LINKER "" CACHE STRING "The default linker to use with cannon")

list(JOIN CANNON_TARGET_TRIPLES ", " _cannon_target_commas)

message(STATUS "Building Cannon on host: ${CANNON_HOST_TRIPLE}")
message(STATUS "Building Cannon targets for: ${_cannon_target_commas}")
message(STATUS "Compiling for ${CANNON_DEFAULT_TARGET_TRIPLE} by default")

configure_file(Config.hpp.in Config.hpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

include(CheckCXXCompilerFlag)

if(NOT DEFINED CANNON_HAS_WERROR)
    check_cxx_compiler_flag("-Werror" CANNON_HAS_WERROR)
endif()

if(CANNON_HAS_WERROR)
    add_compile_options("-Werror")
endif()

if(NOT DEFINED CANNON_HAS_PEDANTIC_ERRORS)
    check_cxx_compiler_flag("-pedantic-errors" CANNON_HAS_PEDANTIC_ERRORS)
endif()

if(CANNON_HAS_PEDANTIC_ERRORS)
    add_compile_options("-pedantic-errors")
endif()

if(NOT DEFINED CANNON_HAS_WALL)
    check_cxx_compiler_flag("-Wall" CANNON_HAS_WALL)
endif()

if(CANNON_HAS_WALL)
    add_compile_options("-Wall")
endif()

add_executable(cannon-bootstrap
        src/main.cpp
        src/lex.cpp src/lex.hpp
        src/token.cpp src/token.hpp
        src/ast.cpp src/ast.hpp
        src/parser.cpp src/parser.hpp
        src/semantic.cpp src/semantic.hpp
        src/program.cpp src/program.hpp
        src/codegen.cpp src/codegen.hpp

        ${CMAKE_CURRENT_BINARY_DIR}/Config.hpp
        src/mode.hpp)

target_include_directories(cannon-bootstrap PRIVATE llvm-project/llvm/include
        ${CMAKE_CURRENT_BINARY_DIR}/llvm-project/llvm/include)

set(LLVM_NATIVE_ARCH X86)

llvm_map_components_to_libnames(llvm_libs codegen core native nativecodegen object support target)

message(STATUS "LLVM libraries: ${llvm_libs}")
message(STATUS "LLVM native target: ${LLVM_NATIVE_ARCH}")

target_link_libraries(cannon-bootstrap ${llvm_libs})
