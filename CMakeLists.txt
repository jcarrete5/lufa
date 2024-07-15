cmake_minimum_required(VERSION 3.25)

project(rb3-midi-adpater)

include_directories(lufa Config)
add_compile_definitions(F_CPU=${F_CPU} F_USB=${F_CPU})

add_subdirectory(lufa)
add_subdirectory(src)
