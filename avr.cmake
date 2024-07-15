cmake_minimum_required(VERSION 3.25)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

block(
  SCOPE_FOR VARIABLES
  PROPAGATE CMAKE_C_COMPILER CMAKE_CXX_COMPILER
)
  set(tools /Volumes/crosstool-NG/x-tools/avr/bin)
  set(prefix avr-)
  set(common_flags -pipe -mmcu=atmega32u4)
  set(c_flags)

  set(CMAKE_C_COMPILER "${tools}/${prefix}gcc" ${common_flags} ${c_flags})
  set(CMAKE_CXX_COMPILER "${tools}/${prefix}g++" ${common_flags})
endblock()
