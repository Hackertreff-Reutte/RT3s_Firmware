#include "cdcAcmUSB.h"


//bounche back UART messages
void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	//TODO add check if it is the right endpoint
	(void)ep;

	char buf[64];
	//read the incoming packet
	int len = usbd_ep_read_packet(usbd_dev, USB_CDC_ACM_DATA_RX_EP_ADDR, buf, 64);

	//flip letter upper and lower case
	for(int i = 0; i < len; i++){
		if(buf[i] >= 'A' && buf[i] <= 'Z'){
			buf[i] += 32;
		}else if(buf[i] >= 'a' && buf[i] <= 'z'){
			buf[i] -= 32;
		}
	}

	//write back
	if (len) {
		while (usbd_ep_write_packet(usbd_dev, USB_CDC_ACM_DATA_TX_EP_ADDR, buf, len) == 0);
	}
}


//struct to handle controll messages
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




void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	(void)wValue;

    //register CDC RX endpoint
	usbd_ep_setup(usbd_dev, USB_CDC_ACM_DATA_RX_EP_ADDR, USB_ENDPOINT_ATTR_BULK, 64,
			cdcacm_data_rx_cb);

    //register CDC TX endpoint
	usbd_ep_setup(usbd_dev, USB_CDC_ACM_DATA_TX_EP_ADDR, USB_ENDPOINT_ATTR_BULK, 64, NULL);


	//register the controll endpoint
	usbd_ep_setup(usbd_dev, USB_CDC_ACM_CONTROL_EP_ADDR, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

    //setup the controll callback function for usb controll packages 
	usbd_register_control_callback(
				usbd_dev,
				USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				cdcacm_control_request);
}