#include "audioUSB.h"



int16_t waveform_data[WAVEFORM_SAMPLES] = {0};

//struct to handle control messages
enum usbd_request_return_codes audio_control_request(usbd_device *usbd_dev,
	struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
	void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
	(void)complete;
	(void)buf;
	(void)usbd_dev;
    (void)len;


    //this is the set alt interface request
    //TODO ADD MORE CHECKS AND HANDLE THIS BETTER
    if(req->bRequest == USB_REQ_SET_INTERFACE){

        streaming_iface_mic_cur_altsetting = 1;
        //check for wIndex and wValue
        //gpio_set(GPIOE, GPIO0);
        return USBD_REQ_HANDLED;
    }

    return USBD_REQ_NOTSUPP;
}


enum usbd_request_return_codes audio_control_request_endpoint(usbd_device *usbd_dev,
	struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
	void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
	(void)complete;
	(void)buf;
	(void)usbd_dev;
    (void)len;
    (void)req;

    if(req->bmRequestType == 0x22){
        return USBD_REQ_HANDLED;
    }else{
        return USBD_REQ_NOTSUPP;
    }
}

//generate sawthooth, but is no longer used (first speaker package overrides it)
void init_waveform_data()
{
    /* Just transmit a boring sawtooth waveform on both channels */
    for (int i = 0; i < WAVEFORM_SAMPLES; i++) {
        waveform_data[i] = 4095 * i - 4095 * WAVEFORM_SAMPLES / 2;
    }
}

/* HACK: upstream libopencm3 currently does not handle isochronous endpoints
 * correctly. We must program the USB peripheral with an even/odd frame bit,
 * toggling it so that we respond to every iso IN request from the host.
 * If this toggling is not performed, we only get half the bandwidth. */
#define USB_REBASE(x) MMIO32((x) + (USB_OTG_FS_BASE))
#define USB_DIEPCTLX_SD1PID     (1 << 29) // Odd frames
#define USB_DIEPCTLX_SD0PID     (1 << 28) // Even frames
void toggle_isochronous_frame_mic(uint8_t ep)
{
    static int toggle_mic = 0;
    if (toggle_mic++ % 2 == 0) {
        USB_REBASE(OTG_DIEPCTL(ep)) |= USB_DIEPCTLX_SD0PID; //OTG_DIEPCTL because IN endpoint
    } else {
        USB_REBASE(OTG_DIEPCTL(ep)) |= USB_DIEPCTLX_SD1PID;
    }
}

void toggle_isochronous_frame_speaker(uint8_t ep)
{
    static int toggle_speaker = 0;
    if (toggle_speaker++ % 2 == 0) {
        USB_REBASE(OTG_DOEPCTL(ep)) |= USB_DIEPCTLX_SD0PID;  //OTG_DOEPCTL because OUT endpoint
    } else {
        USB_REBASE(OTG_DOEPCTL(ep)) |= USB_DIEPCTLX_SD1PID;
    }
}


void usbaudio_iso_mic_stream_callback(usbd_device *usbd_dev, uint8_t ep)
{
    (void)ep;
    toggle_isochronous_frame_mic(ep);

    //2 times the waveform_smaples because 8 * 16bit = 16 * 2 = 32 byte
    usbd_ep_write_packet(usbd_dev, USB_AUDIO_MIC_STREAMING_EP_ADDR, waveform_data, WAVEFORM_SAMPLES * 2);

}


void usbaudio_iso_speaker_stream_callback(usbd_device *usbd_dev, uint8_t ep)
{
    (void)ep;
    toggle_isochronous_frame_speaker(ep);

    usbd_ep_read_packet(usbd_dev, USB_AUDIO_SPEAKER_STREAMING_EP_ADDR, waveform_data, WAVEFORM_SAMPLES * 2);
}