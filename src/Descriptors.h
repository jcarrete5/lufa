#ifndef DESCRIPTORS_H_
#define DESCRIPTORS_H_

#include <avr/pgmspace.h>
#include <LUFA/Drivers/USB/USB.h>

/** Type define for the device configuration descriptor structure. This must be
 * defined in the application code, as the configuration descriptor contains
 * several sub-descriptors which vary between devices, and which describe the
 * device's usage to the host.
 */
struct usb_descriptor_configuration {
    USB_Descriptor_Configuration_Header_t Config;

    // Generic HID Interface
    USB_Descriptor_Interface_t HID_Interface;
    USB_HID_Descriptor_HID_t HID_GenericHID;
    USB_Descriptor_Endpoint_t HID_ReportOUTEndpoint;
    USB_Descriptor_Endpoint_t HID_ReportINEndpoint;
};

/** Enum for the device interface descriptor IDs within the device. Each
 * interface descriptor should have a unique ID index associated with it, which
 * can be used to refer to the interface from other descriptors.
 */
enum interface_descriptors {
    INTERFACE_ID_GenericHID = 0, /**< GenericHID interface descriptor ID */
};

/** Enum for the device string descriptor IDs within the device. Each string
 * descriptor should have a unique ID index associated with it, which can be
 * used to refer to the string from other descriptors.
 */
enum string_descriptors {
    STRING_ID_Language = 0, /**< Supported Languages string descriptor ID (must be zero) */
    STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
    STRING_ID_Product = 2, /**< Product string ID */
};

/** Endpoint address of the Generic HID reporting IN endpoint. */
#define GENERIC_IN_EPADDR  (ENDPOINT_DIR_IN | 1)
#define GENERIC_OUT_EPADDR (ENDPOINT_DIR_OUT | 2)

/** Size in bytes of the Generic HID reporting endpoint. */
#define GENERIC_EPSIZE 64

uint16_t CALLBACK_USB_GetDescriptor(
    const uint16_t wValue, const uint16_t wIndex, const void **const DescriptorAddress
) ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif /* DESCRIPTORS_H_ */
