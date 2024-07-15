set(F_USB ${F_CPU})

add_library(lufa_usb_common STATIC)

add_library(lufa_usb_host STATIC)
target_link_libraries(
  lufa_usb_host
  PUBLIC lufa_usb_common
)

add_library(lufa_usb_device STATIC)
target_link_libraries(
  lufa_usb_device
  PUBLIC lufa_usb_common
)

add_library(lufa_usbclass_device STATIC)

add_library(lufa_usbclass_host STATIC)

add_library(lufa_usbclass INTERFACE)
target_link_libraries(
  lufa_usbclass
  INTERFACE
    lufa_usbclass_device
    lufa_usbclass_host
)

add_library(lufa_usb INTERFACE)
target_link_libraries(
  lufa_usb
  INTERFACE
    lufa_usb_common
    lufa_usb_host
    lufa_usb_device
)

add_library(lufa_temperature STATIC)

add_library(lufa_serial STATIC)

add_library(lufa_twi STATIC)

if(ARCH STREQUAL "UC3")
  add_library(lufa_platform STATIC)
endif()

add_compile_definitions(
  ARCH=ARCH_${ARCH}
  BOARD=BOARD_${BOARD}
  F_USB=${F_USB}UL
)

add_subdirectory(Drivers)
add_subdirectory(Platform)
