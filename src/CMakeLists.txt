add_executable(
  rb3-midi-adapter
  Descriptors.c
  HIDReport.c
  MIDI.c
  PadConfig.c
  rb3-midi-adapter.c
)
target_sources(
  rb3-midi-adapter
  PRIVATE
    FILE_SET HEADERS
    FILES Descriptors.h HIDReport.h MIDI.h PadConfig.h 
)
target_link_libraries(
  rb3-midi-adapter
  PRIVATE lufa_usb lufa_usbclass
)
