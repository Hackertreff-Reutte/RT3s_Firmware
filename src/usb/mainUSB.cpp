#include "mainUSB.h"

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];

const char *usb_strings[] = {
	"UnHold Technologies",
	"CDC-ACM Demo",
	"DEMO",
    "Audio Alex"
};

int setupUSB() {

    usbd_device *usbd_dev;

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
    usbd_dev = usbd_init(&otgfs_usb_driver, &device_descriptor, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
    OTG_FS_GCCFG |= OTG_GCCFG_NOVBUSSENS | OTG_GCCFG_PWRDWN; //fix = https://github.com/libopencm3/libopencm3/issues/1309

    
    //setup cdc acm (usb UART)
    cdcacm_setup(usbd_dev);


    while (1) {
		usbd_poll(usbd_dev);
	}
}