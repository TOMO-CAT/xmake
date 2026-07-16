cmake_minimum_required(VERSION 3.10)
project(cmake_flags_repro LANGUAGES CXX)

add_library(cmake_flags_repro STATIC repro.cpp)
target_include_directories(cmake_flags_repro PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS cmake_flags_repro ARCHIVE DESTINATION lib)
install(FILES repro.hpp DESTINATION include)
