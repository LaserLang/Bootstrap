cmake_minimum_required(VERSION 3.15)
project(laserc-bootstrap CXX)

option(LLVM_ENABLE_BINDINGS "By default, llvm bindings are not built with laserc" OFF)

add_subdirectory(llvm-project/llvm)



