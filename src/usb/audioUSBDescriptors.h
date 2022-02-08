#ifndef AUDIO_USB_DESCRIPTORS
#define AUDIO_USB_DESCRIPTORS

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/audio.h>
#include "usbAddresses.h"

#define SAMPLE_RATE_MIC 8000
#define RESOLUTION_MIC 16

#define SAMPLE_RATE_SPEAKER 8000
#define RESOLUTION_SPEAKER 16


//------------------------------------- ASSOCIATION -------------------------------------------

static const struct usb_iface_assoc_descriptor audio_mic_iface_assoc = {  //  Interface Association for sub-interfaces.
	.bLength = USB_DT_INTERFACE_ASSOCIATION_SIZE,
	.bDescriptorType = USB_DT_INTERFACE_ASSOCIATION,
	.bFirstInterface = USB_AUDIO_CONTROL_INTERFACE_ADDR,  //  First associated interface is audio_control_iface, interface ID is 3
	.bInterfaceCount = 3,  //  We have 3 associated interfaces: audio_control_iface and audio_mic_streaming_iface and audio_speaker_streaming_iface
	.bFunctionClass = USB_CLASS_AUDIO, //  This is a USB AUDIO (Audio Device Class) interface
	.bFunctionSubClass = 0x02,  //  not sure what this is (maybe audio conntroll)
	.bFunctionProtocol = 1,   //  not sure why
	.iFunction = 0  //  Name of Audio interface (index of string descriptor) ???
};


#define EP_GENERAL 1
#define GENERIC_MIC 0x0201
#define USB_STREAMING 0x0101 //not bidirectional
#define GENERIC_SPEAKER 0x301 //not bidirectional 

#define GENERIC_BIDIRECTIONAL 0x0400
#define HEADSET_HANDHELD 0x0401 
#define HEADSET_HEADMOUNTED 0x0402

//Terminal IDs
#define TERMINAL_ID_MIC_INPUT 1
#define TERMINAL_ID_MIC_OUTPUT 2
#define TERMINAL_ID_SPEAKER_INPUT 3
#define TERMINAL_ID_SPEAKER_OUTPUT 4

#define MONO_AUDIO 0


//------------------------------------- CONTROL -------------------------------------------

static const struct {
    struct usb_audio_header_descriptor_head header_head;
    struct usb_audio_header_descriptor_body header_mic_body;
    struct usb_audio_header_descriptor_body header_speaker_body;
    struct usb_audio_input_terminal_descriptor input_mic_terminal_desc;
    struct usb_audio_output_terminal_descriptor output_mic_terminal_desc;
    struct usb_audio_input_terminal_descriptor input_speaker_terminal_desc;
    struct usb_audio_output_terminal_descriptor output_speaker_terminal_desc;
} __attribute__((packed)) audio_control_functional_descriptors = {
    .header_head = {
        .bLength = sizeof(struct usb_audio_header_descriptor_head)+
                   sizeof(struct usb_audio_header_descriptor_body) * 2,
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = USB_AUDIO_TYPE_HEADER,
        .bcdADC = 0x0100, //(Audio Device Class Specification Release Number)
        .wTotalLength =
               sizeof(struct usb_audio_header_descriptor_head) +
               sizeof(struct usb_audio_header_descriptor_body) * 2 +
               sizeof(usb_audio_input_terminal_descriptor) * 2 +
               sizeof(struct usb_audio_output_terminal_descriptor) * 2,
        .binCollection = 2, //only 2 AudioStreaming Interface (Change this is another is added)
    },
    .header_mic_body = {
        .baInterfaceNr = USB_AUDIO_MIC_STREAMING_INTERFACE_ADDR, //the first audio interface number
    },
    .header_speaker_body = {
        .baInterfaceNr = USB_AUDIO_SPEAKER_STREAMING_INTERFACE_ADDR, //the second audio interface number
    },
    .input_mic_terminal_desc = {
        .bLength = sizeof(struct usb_audio_input_terminal_descriptor),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = USB_AUDIO_TYPE_INPUT_TERMINAL,
        .bTerminalID = TERMINAL_ID_MIC_INPUT,
        .wTerminalType = HEADSET_HEADMOUNTED,
        .bAssocTerminal = TERMINAL_ID_SPEAKER_OUTPUT,
        .bNrChannels = 1,
        .wChannelConfig = MONO_AUDIO,
        .iChannelNames = 0,
        .iTerminal = 0,
    },
    .output_mic_terminal_desc = {
        .bLength = sizeof(struct usb_audio_output_terminal_descriptor),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = USB_AUDIO_TYPE_OUTPUT_TERMINAL,
        .bTerminalID = TERMINAL_ID_MIC_OUTPUT, 
        .wTerminalType = USB_STREAMING,
        .bAssocTerminal = 0,
        .bSourceID = TERMINAL_ID_MIC_INPUT,
        .iTerminal = 0,
    },
    .input_speaker_terminal_desc = {
        .bLength = sizeof(struct usb_audio_input_terminal_descriptor),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = USB_AUDIO_TYPE_INPUT_TERMINAL,
        .bTerminalID = TERMINAL_ID_SPEAKER_INPUT,
        .wTerminalType = USB_STREAMING,
        .bAssocTerminal = 0,
        .bNrChannels = 1,
        .wChannelConfig = MONO_AUDIO,
        .iChannelNames = 0,
        .iTerminal = 0,
    },
    .output_speaker_terminal_desc = {
        .bLength = sizeof(struct usb_audio_output_terminal_descriptor),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = USB_AUDIO_TYPE_OUTPUT_TERMINAL,
        .bTerminalID = TERMINAL_ID_SPEAKER_OUTPUT, 
        .wTerminalType = HEADSET_HEADMOUNTED,
        .bAssocTerminal = TERMINAL_ID_MIC_INPUT,
        .bSourceID = TERMINAL_ID_SPEAKER_INPUT,
        .iTerminal = 0,
    }
};


static const struct usb_interface_descriptor audio_control_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = USB_AUDIO_CONTROL_INTERFACE_ADDR,
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


//------------------------------------- MIC & SPEAKER -------------------------------------------

static const struct usb_audio_stream_audio_endpoint_descriptor audio_streaming_cs_ep_desc[] = { {
    .bLength = sizeof(struct usb_audio_stream_audio_endpoint_descriptor),
    .bDescriptorType = USB_AUDIO_DT_CS_ENDPOINT,
    .bDescriptorSubtype = EP_GENERAL,
    .bmAttributes = 1, //was 0 (doesn't really matter)
    .bLockDelayUnits = 0x0, // PCM samples 
    .wLockDelay = 0x0000,
} };

//------------------------------------- MIC -------------------------------------------

static const struct usb_endpoint_descriptor isochronous_mic_ep[] = { {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = USB_AUDIO_MIC_STREAMING_EP_ADDR,
    .bmAttributes =  USB_ENDPOINT_ATTR_ISOCHRONOUS | USB_ENDPOINT_ATTR_ADAPTIVE,
    .wMaxPacketSize = 256, //should not be too small otherwise it won't work
    .bInterval = 0x0F, // 1 frame //was 0x01

    .extra = &audio_streaming_cs_ep_desc[0],
    .extralen = sizeof(audio_streaming_cs_ep_desc[0])
} };



#define AS_GENERAL 0x01
#define PCM_FORMAT 0x0001
#define FORMAT_TYPE 0x02


static const struct {
    struct usb_audio_stream_interface_descriptor audio_cs_streaming_iface_desc;
    struct usb_audio_format_type1_descriptor_1freq audio_type1_format_desc;
} __attribute__((packed)) audio_streaming_mic_functional_descriptors = {
    .audio_cs_streaming_iface_desc = {
        .bLength = sizeof(struct usb_audio_stream_interface_descriptor),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = AS_GENERAL,
        .bTerminalLink = TERMINAL_ID_MIC_OUTPUT,
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
            .bBitResolution = RESOLUTION_MIC, //16 bits per sample
            .bSamFreqType = 1, //one frequency supporter 
        },
        .freqs = { {
            .tSamFreq = SAMPLE_RATE_MIC,
        } },
    }
};

static const struct usb_interface_descriptor audio_mic_streaming_iface[] = {{
    .bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = USB_AUDIO_MIC_STREAMING_INTERFACE_ADDR,
	.bAlternateSetting = 0,
	.bNumEndpoints = 0,
	.bInterfaceClass = USB_CLASS_AUDIO,
	.bInterfaceSubClass = USB_AUDIO_SUBCLASS_AUDIOSTREAMING,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = 0,

    .extra = NULL,
    .extralen = 0
},
{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = USB_AUDIO_MIC_STREAMING_INTERFACE_ADDR,
	.bAlternateSetting = 1,
	.bNumEndpoints = 1,
	.bInterfaceClass = USB_CLASS_AUDIO,
	.bInterfaceSubClass = USB_AUDIO_SUBCLASS_AUDIOSTREAMING,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = isochronous_mic_ep,

    .extra = &audio_streaming_mic_functional_descriptors,
    .extralen = sizeof(audio_streaming_mic_functional_descriptors)
} };


// ------------------------- SPEAKER ---------------------------------------


static const struct usb_endpoint_descriptor isochronous_speaker_ep[] = { {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = USB_AUDIO_SPEAKER_STREAMING_EP_ADDR,
    .bmAttributes =  USB_ENDPOINT_ATTR_ISOCHRONOUS | USB_ENDPOINT_ATTR_ADAPTIVE,
    .wMaxPacketSize = 256,//256, //should not be too small otherwise it won't work
    .bInterval = 0x0F, // 1 frame //was 0x01

    .extra = &audio_streaming_cs_ep_desc[0],
    .extralen = sizeof(audio_streaming_cs_ep_desc[0])
} };

static const struct {
    struct usb_audio_stream_interface_descriptor audio_cs_streaming_iface_desc;
    struct usb_audio_format_type1_descriptor_1freq audio_type1_format_desc;
} __attribute__((packed)) audio_streaming_speaker_functional_descriptors = {
    .audio_cs_streaming_iface_desc = {
        .bLength = sizeof(struct usb_audio_stream_interface_descriptor),
        .bDescriptorType = USB_AUDIO_DT_CS_INTERFACE,
        .bDescriptorSubtype = AS_GENERAL,
        .bTerminalLink = TERMINAL_ID_SPEAKER_INPUT,
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
            .bBitResolution = RESOLUTION_SPEAKER, //16 bits per sample
            .bSamFreqType = 1, //one frequency supporter 
        },
        .freqs = { {
            .tSamFreq = SAMPLE_RATE_SPEAKER,
        } },
    }
};

static const struct usb_interface_descriptor audio_speaker_streaming_iface[] = {
{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = USB_AUDIO_SPEAKER_STREAMING_INTERFACE_ADDR,
	.bAlternateSetting = 0,
	.bNumEndpoints = 0,
	.bInterfaceClass = USB_CLASS_AUDIO,
	.bInterfaceSubClass = USB_AUDIO_SUBCLASS_AUDIOSTREAMING,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = 0,

    .extra = 0,
    .extralen = 0
},
{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = USB_AUDIO_SPEAKER_STREAMING_INTERFACE_ADDR,
	.bAlternateSetting = 1,
	.bNumEndpoints = 1,
	.bInterfaceClass = USB_CLASS_AUDIO,
	.bInterfaceSubClass = USB_AUDIO_SUBCLASS_AUDIOSTREAMING,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = isochronous_speaker_ep,

    .extra = &audio_streaming_speaker_functional_descriptors,
    .extralen = sizeof(audio_streaming_speaker_functional_descriptors)
} };


#endif