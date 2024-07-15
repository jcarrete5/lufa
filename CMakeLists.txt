cmake_minimum_required(VERSION 3.24)

project(lufa)

set(
  MCU atmega32u4
  CACHE STRING "Name of the Atmel processor model"
)

set(
  ARCH AVR8 
  CACHE STRING "Target device architecture (AVR8, UC3, XMEGA)"
)
set_property(
  CACHE ARCH
  PROPERTY STRINGS AVR8 UC3 XMEGA
)

set(
  F_CPU 16000000
  CACHE STRING "Processor core clock frequency, in Hz. This is used by some architectures for functions such as software spin-loop delays"
)

set(
  BOARD MICRO
  CACHE STRING "Board name. Used to tailor APIs to specific board configurations"
)

add_subdirectory(LUFA)
