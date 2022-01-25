#ifndef AUDIO_USB_DESCRIPTORS
#define AUDIO_USB_DESCRIPTORS

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/audio.h>

#define SAMPLE_RATE 8000

static const struct {
    struct usb_audio_header_descriptor_head header_head;
    struct usb_audio_header_descriptor_body header_body;
    struct usb_audio_output_terminal_descriptor output_terminal_desc;
} __attribute__((packed)) audio_control_functional_descriptors = {
    .header_head = {
        .bLength = sizeof(struct usb_audio_header_descriptor_head) +
                   1 * sizeof(struct usb_audio_header_descriptor_body),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = USB_AUDIO_TYPE_HEADER,
        .bcdADC = 0x0100, //TODO CHECKK THIS (Audio Device Class Specification Release Number)
        .wTotalLength =
               sizeof(struct usb_audio_header_descriptor_head) +
               1 * sizeof(struct usb_audio_header_descriptor_body) +
               sizeof(struct usb_audio_output_terminal_descriptor),
        .binCollection = 1, //only 1 AudioStreaming Interface (Change this is another is added)
    },
    .header_body = {
        .baInterfaceNr = 4, //the first audio interface number
    },
    .output_terminal_desc = {
        .bLength = sizeof(struct usb_audio_output_terminal_descriptor),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = USB_AUDIO_TYPE_OUTPUT_TERMINAL,
        .bTerminalID = 3, //TODO CHANGE TO 1 LATER
        .wTerminalType = 0x0101, /* USB Streaming */
        .bAssocTerminal = 0,
        .bSourceID = 0x02, /* Feature unit 2 */
        .iTerminal = 0,
    }
};


static const struct usb_interface_descriptor aduio_control_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 3,
	.bAlternateSetting = 0,
	.bNumEndpoints = 0, //because no audio controll endpoint is present (optional)
	.bInterfaceClass = USB_CLASS_AUDIO,
	.bInterfaceSubClass = USB_AUDIO_SUBCLASS_CONTROL,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = 0,

    .extra = &audio_control_functional_descriptors,
    .extralen = sizeof(audio_control_functional_descriptors)
} };

/*
static const struct usb_audio_stream_audio_endpoint_descriptor audio_streaming_cs_ep_desc[] = { {
    .bLength = sizeof(struct usb_audio_stream_audio_endpoint_descriptor),
    .bDescriptorType = USB_AUDIO_DT_CS_ENDPOINT,
    .bDescriptorSubtype = 1, // EP_GENERAL 
    .bmAttributes = 0,
    .bLockDelayUnits = 0x02, // PCM samples 
    .wLockDelay = 0x0000,
} };

static const struct usb_endpoint_descriptor isochronous_ep[] = { {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = 0x82,
    .bmAttributes = USB_ENDPOINT_ATTR_ASYNC | USB_ENDPOINT_ATTR_ISOCHRONOUS,
    .wMaxPacketSize = 256, //maybe change to 64
    .bInterval = 0x01, // 1 millisecond 

    // not using usb_audio_stream_endpoint_descriptor??
    // (Why? These are USBv1.0 endpoint descriptors)

    .extra = &audio_streaming_cs_ep_desc[0],
    .extralen = sizeof(audio_streaming_cs_ep_desc[0])
} };
*/

#define AS_GENERAL 0x01
#define PCM_FORMAT 0x0001
#define FORMAT_TYPE 0x02


static const struct {
    struct usb_audio_stream_interface_descriptor audio_cs_streaming_iface_desc;
    struct usb_audio_format_type1_descriptor_1freq audio_type1_format_desc;
} __attribute__((packed)) audio_streaming_functional_descriptors = {
    .audio_cs_streaming_iface_desc = {
        .bLength = sizeof(struct usb_audio_stream_interface_descriptor),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = AS_GENERAL,
        .bTerminalLink = 3, /* Terminal 3 */
        .bDelay = 0x1, //one frame delay 
        .wFormatTag = PCM_FORMAT,
    },
    .audio_type1_format_desc = {
        .head = {
            .bLength = sizeof(struct usb_audio_format_type1_descriptor_1freq),
            .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
            .bDescriptorSubtype = FORMAT_TYPE,
            .bFormatType = 1,
            .bNrChannels = 1,
            .bSubFrameSize = 2,
            .bBitResolution = 16, //16 bits per sample
            .bSamFreqType = 1, //one frequency supporter 
        },
        .freqs = { {
            .tSamFreq = SAMPLE_RATE,
        } },
    }
};

static const struct usb_interface_descriptor audio_streaming_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 4,
	.bAlternateSetting = 0,
	.bNumEndpoints = 0,
	.bInterfaceClass = USB_CLASS_AUDIO,
	.bInterfaceSubClass = USB_AUDIO_SUBCLASS_AUDIOSTREAMING,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = 0,

    .extra = &audio_streaming_functional_descriptors,
    .extralen = sizeof(audio_streaming_functional_descriptors)
} };

#endif