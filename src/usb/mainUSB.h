
#ifndef mainUSB_H
#define mainUSB_H

#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/usb/dwc/otg_fs.h> // for OTG_FS_GCCFG


#include "cdcAcmUSB.h"
#include "audioUSB.h"
#include "descriptors.h"
#include "usbAddresses.h"


extern usbd_device *usbd_dev_main;

void pollUSB();
int setupUSB();

#endif