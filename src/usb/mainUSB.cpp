#include "mainUSB.h"
#include <libopencm3/cm3/nvic.h>

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[1024];

const char *usb_strings[] = {
	"UnHold Technologies",
	"CDC-ACM Demo",
	"DEMO",
    "Audio MIC Alex"
};


//struct to handle controll messages

static enum usbd_request_return_codes audio_control_request(usbd_device *usbd_dev,
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


static enum usbd_request_return_codes audio_control_request_endpoint(usbd_device *usbd_dev,
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


#define WAVEFORM_SAMPLES 8

int16_t waveform_data[WAVEFORM_SAMPLES] = {0};

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

void set_config(usbd_device *usbd_dev, uint16_t wValue){

    usbd_ep_setup(usbd_dev, USB_AUDIO_MIC_STREAMING_EP_ADDR, USB_ENDPOINT_ATTR_ISOCHRONOUS, WAVEFORM_SAMPLES * 2, usbaudio_iso_mic_stream_callback);
    usbd_ep_setup(usbd_dev, USB_AUDIO_SPEAKER_STREAMING_EP_ADDR, USB_ENDPOINT_ATTR_ISOCHRONOUS, WAVEFORM_SAMPLES * 2, usbaudio_iso_speaker_stream_callback);


    //maybe place this somewhere else?
    usbd_ep_write_packet(usbd_dev, USB_AUDIO_MIC_STREAMING_EP_ADDR, waveform_data, WAVEFORM_SAMPLES * 2);
    //usbd_ep_read_packet(usbd_dev, USB_AUDIO_SPEAKER_STREAMING_EP_ADDR, waveform_data, WAVEFORM_SAMPLES * 2); //TODO check this
    //set config cdc acm (usb uart)
    if(wValue == 0){

    }

    //setup cdc acm
    cdcacm_set_config(usbd_dev, wValue);


    usbd_register_control_callback(
				usbd_dev,
				USB_REQ_TYPE_OUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
				0xFF,
				audio_control_request);



    //LOG EVERYTHING
    //TODO change this (set frequency callback)
    usbd_register_control_callback(
				usbd_dev,
				USB_REQ_TYPE_OUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_ENDPOINT,
				0xFF,//0b01111111,  //8 bit zero catch IN and OUT
				audio_control_request_endpoint);

}



int setupUSB() {

    usbd_device *usbd_dev_main;


    init_waveform_data();

    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    //enable GPIOA
    rcc_periph_clock_enable(RCC_GPIOA);

    //LED
    gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);

    //enable fullspeed 
    rcc_periph_clock_enable(RCC_OTGFS);

    //Needed for OTG_FS_ID, USB_D+ and USB_D-
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10 | GPIO11 | GPIO12);

    //selecting AF 10 for PA10 & PA11 & PA12  -> OTG_FS_ID, OTG_FS_DM, OTG_FS_DP 
    gpio_set_af(GPIOA, GPIO_AF10, GPIO10 | GPIO11 | GPIO12);

    //setup the USB
    usbd_dev_main = usbd_init(&otgfs_usb_driver, &device_descriptor, &config, usb_strings, sizeof(usb_strings)/sizeof(char*), usbd_control_buffer, sizeof(usbd_control_buffer));

    OTG_FS_GCCFG |= OTG_GCCFG_NOVBUSSENS | OTG_GCCFG_PWRDWN; //fix = https://github.com/libopencm3/libopencm3/issues/1309

    //setup confiog
    usbd_register_set_config_callback(usbd_dev_main, set_config);



    while (1) {
        usbd_poll(usbd_dev_main);
	}
}