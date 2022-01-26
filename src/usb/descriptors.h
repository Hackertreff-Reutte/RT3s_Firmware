#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include "cdcAcmUSBDescriptors.h"
#include "audioUSBDescriptors.h"


const struct usb_device_descriptor device_descriptor = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE, //type = device descriptor
	.bcdUSB = 0x0200, //USB version 2
	.bDeviceClass = 0,  //0 = USB_CLASS_MISCELLANEOUS
	.bDeviceSubClass = 0, //device defined at interface level
	.bDeviceProtocol = 1, //use device assoc descriptor
	.bMaxPacketSize0 = 64, //packet size 8 16 32 64
	.idVendor = 0x0483, //STMircoelectronics
	.idProduct = 0x5740, //STMircoelectronics Virtual COM Port
	.bcdDevice = 0x0200,
	.iManufacturer = 1, //usb_strings -> UnHold Technologies
	.iProduct = 2, //usb_strings ->  CDC-ACM Demo
	.iSerialNumber = 3, //usb_strings -> DEMO
	.bNumConfigurations = 1, //num of configurations
};


static uint8_t streaming_iface_cur_altsetting = 1;

const struct usb_interface ifaces[] = {
	/*
{
	.cur_altsetting = 0,
	.num_altsetting = 1,
	.iface_assoc = &cdc_iface_assoc,
	.altsetting = comm_iface,
},
{
	.cur_altsetting = 0,
	.num_altsetting = 1,
	.iface_assoc = NULL,
	.altsetting = data_iface,
}, */
{
	.cur_altsetting = 0,
	.num_altsetting = 1,
	.iface_assoc = NULL,
	.altsetting = audio_control_iface,
}, 
{
	.cur_altsetting = &streaming_iface_cur_altsetting,
	.num_altsetting = 2,
	.iface_assoc = NULL,
	.altsetting = audio_streaming_iface,
}
};

const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0, //is calculated automatically
	.bNumInterfaces = 2, //number of interfaces
	.bConfigurationValue = 1, //configuration value
	.iConfiguration = 0, //index of string descriptor describing this descriptor (0 = none)
	.bmAttributes = 0x80, //power managment = (default)
	.bMaxPower = 0xfa, //max power 100mA //was 0x32

	.interface = ifaces, //pointer to interface struct
};

#endif