#include "mainUSB.h"


/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];

static const char *usb_strings[] = {
	"UnHold Technologies",
	"CDC-ACM Demo",
	"DEMO",
};



static const struct usb_device_descriptor device_descriptor = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE, //type = device descriptor
	.bcdUSB = 0x0200, //USB version 2
	.bDeviceClass = USB_CLASS_CDC,  //Communications and CDC Control
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64, //packet size 8 16 32 64
	.idVendor = 0x0483, //STMircoelectronics
	.idProduct = 0x5740, //STMircoelectronics Virtual COM Port
	.bcdDevice = 0x0200,
	.iManufacturer = 1, //usb_strings -> UnHold Technologies
	.iProduct = 2, //usb_strings ->  CDC-ACM Demo
	.iSerialNumber = 3, //usb_strings -> DEMO
	.bNumConfigurations = 1, //num of configurations
};


static const struct usb_endpoint_descriptor data_endp[] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x01, //0b1 = Endpoint number 1 + OUT
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64, 
	.bInterval = 1,
    .extra = NULL,
    .extralen = 0
}, {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x82, //0b10000010 = Endpoint number 2 + IN
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 1,
    .extra = NULL,
    .extralen = 0
} };

static const struct usb_interface_descriptor data_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 1,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2, //number of endpoints (RX and TX)
	.bInterfaceClass = USB_CLASS_DATA,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = data_endp,

    .extra = NULL,
    .extralen = 0
} };



static const struct usb_interface ifaces[] = {
    {
    .cur_altsetting = 0,
	.num_altsetting = 1, //???
    .iface_assoc = NULL,
	.altsetting = data_iface,
    } 
};


static const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = 1, //number of interfaces
	.bConfigurationValue = 1, //configuration value
	.iConfiguration = 0, //index of string descriptor describing this descriptor (0 = none)
	.bmAttributes = 0x80, //power managment = (default)
	.bMaxPower = 0x32, //max power 100mA

	.interface = ifaces, //pointer to interface struct
};








//bounche back UART messages
static void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)ep;

	char buf[64];
	int len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);

	if (len) {
		while (usbd_ep_write_packet(usbd_dev, 0x82, buf, len) == 0);
	}
}

//function to handle controll messages
static enum usbd_request_return_codes cdcacm_control_request(usbd_device *usbd_dev,
	struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
	void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
	(void)complete;
	(void)buf;
	(void)usbd_dev;

	switch (req->bRequest) {
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
		/*
		 * This Linux cdc_acm driver requires this to be implemented
		 * even though it's optional in the CDC spec, and we don't
		 * advertise it in the ACM functional descriptor.
		 */
		return USBD_REQ_HANDLED;
		}
	case USB_CDC_REQ_SET_LINE_CODING:
		if (*len < sizeof(struct usb_cdc_line_coding)) {
			return USBD_REQ_NOTSUPP;
		}

		return USBD_REQ_HANDLED;
	}
	return USBD_REQ_NOTSUPP;
}


static void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	(void)wValue;

	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64,
			cdcacm_data_rx_cb);
	usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);

    //setup the controll callback function for usb controll packages 
	usbd_register_control_callback(
				usbd_dev,
				USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				cdcacm_control_request);
}


int setupUSB() {
    usbd_device *usbd_dev;

    //enable GPIOA
    rcc_periph_clock_enable(RCC_GPIOA);

    //LED
    gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
    gpio_set(GPIOE, GPIO0);	//green led on

    //enable fullspeed 
    rcc_periph_clock_enable(RCC_OTGFS);

    //Needed for USB_D+ and USB_D-
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11 | GPIO12);

    //selecting AF 10 for PA11 & PA12  -> OTG_FS_DM, OTG_FS_DP
    gpio_set_af(GPIOB, GPIO_AF10, GPIO11 | GPIO12);

    //setup the USB
    usbd_dev = usbd_init(&otgfs_usb_driver, &device_descriptor, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));

    //register callback
    usbd_register_set_config_callback(usbd_dev, cdcacm_set_config);

    gpio_clear(GPIOE, GPIO0);	//green led off

    while (1) {
		usbd_poll(usbd_dev);
	}
}