#include "mainUSB.h"

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[1024];

const char *usb_strings[] = {
	"UnHold Technologies",
	"CDC-ACM Demo",
	"DEMO",
    "Audio MIC Alex"
};


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


usbd_device *usbd_dev_main;

void pollUSB(){
    usbd_poll(usbd_dev_main);
}

int setupUSB() {

    setup_cdc();
    setup_audio();

    //enable GPIOA
    rcc_periph_clock_enable(RCC_GPIOA);

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

    return 0;
}