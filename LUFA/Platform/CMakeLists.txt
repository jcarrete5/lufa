if(ARCH STREQUAL "UC3")
  target_sources(
    lufa_platform
    PRIVATE
      UC3/Exception.S
      UC3/InterruptManagement.c
  )
endif()
