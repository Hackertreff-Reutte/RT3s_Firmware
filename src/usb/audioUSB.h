#ifndef AUDIO_USB_H
#define AUDIO_USB_H

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/dwc/otg_common.h>
#include <libopencm3/cm3/common.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/memorymap.h>
#include <libopencm3/stm32/memorymap.h>

#include "descriptors.h"
#include "usbAddresses.h"


#define WAVEFORM_SAMPLES 8
extern int16_t waveform_data[WAVEFORM_SAMPLES];

enum usbd_request_return_codes audio_control_request(usbd_device *usbd_dev,
	struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
	void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req));


enum usbd_request_return_codes audio_control_request_endpoint(usbd_device *usbd_dev,
	struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
	void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req));


void usbaudio_iso_mic_stream_callback(usbd_device *usbd_dev, uint8_t ep);
void usbaudio_iso_speaker_stream_callback(usbd_device *usbd_dev, uint8_t ep);

//will be removed later
void init_waveform_data();
#endif