target_sources(
  lufa_usb_common
  PRIVATE
    "USB/Core/${ARCH}/USBController_${ARCH}.c"
    "USB/Core/${ARCH}/USBInterrupt_${ARCH}.c"
    "USB/Core/ConfigDescriptors.c"
    "USB/Core/Events.c"
    "USB/Core/USBTask.c"
    "USB/Class/Common/HIDParser.c"
)

target_sources(
  lufa_usb_host
  PRIVATE
    "USB/Core/${ARCH}/Host_${ARCH}.c"
    "USB/Core/${ARCH}/Pipe_${ARCH}.c"
    "USB/Core/${ARCH}/PipeStream_${ARCH}.c"
    "USB/Core/HostStandardReq.c"
)

target_sources(
  lufa_usb_device
  PRIVATE
    "USB/Core/${ARCH}/Device_${ARCH}.c"
    "USB/Core/${ARCH}/Endpoint_${ARCH}.c"
    "USB/Core/${ARCH}/EndpointStream_${ARCH}.c"
    "USB/Core/DeviceStandardReq.c"
)

target_sources(
  lufa_usbclass_device
  PRIVATE
    USB/Class/Device/AudioClassDevice.c
    USB/Class/Device/CCIDClassDevice.c
    USB/Class/Device/CDCClassDevice.c
    USB/Class/Device/HIDClassDevice.c
    USB/Class/Device/MassStorageClassDevice.c
    USB/Class/Device/MIDIClassDevice.c
    USB/Class/Device/PrinterClassDevice.c
    USB/Class/Device/RNDISClassDevice.c
)

target_sources(
  lufa_usbclass_host
  PRIVATE
    USB/Class/Host/AndroidAccessoryClassHost.c
    USB/Class/Host/AudioClassHost.c
    USB/Class/Host/CDCClassHost.c
    USB/Class/Host/HIDClassHost.c
    USB/Class/Host/MassStorageClassHost.c
    USB/Class/Host/MIDIClassHost.c
    USB/Class/Host/PrinterClassHost.c
    USB/Class/Host/RNDISClassHost.c
    USB/Class/Host/StillImageClassHost.c
)

target_sources(
  lufa_temperature
  PRIVATE
    Board/Temperature.c
)

target_sources(
  lufa_serial
  PRIVATE
    "Peripheral/${ARCH}/Serial_${ARCH}.c"
)

target_sources(
  lufa_twi
  PRIVATE
    "Peripheral/${ARCH}/TWI_${ARCH}.c"
)
