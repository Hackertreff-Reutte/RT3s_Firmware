#ifndef CDC_ACM_USB_H
#define CDC_ACM_USB_H

#include <stdlib.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>



//FUNCTION DECLARATION
void cdcacm_setup(usbd_device *usbd_dev);


#endif