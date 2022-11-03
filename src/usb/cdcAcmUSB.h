#ifndef CDC_ACM_USB_H
#define CDC_ACM_USB_H

#include <stdlib.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include "usbAddresses.h"
#include "utils/ringBuffer.h"
#include "mainUSB.h"

#define RX_BUFFER_SIZE 256
#define TX_BUFFER_SIZE 256

extern RING_BUFFER rx_buffer;
extern RING_BUFFER tx_buffer;

//FUNCTION DECLARATION
void sendTxBuffer();
void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue);
void setup_cdc();

#endif