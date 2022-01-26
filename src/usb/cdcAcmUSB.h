#ifndef CDC_ACM_USB_H
#define CDC_ACM_USB_H

#include <stdlib.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>



//FUNCTION DECLARATION
void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue);

#endif