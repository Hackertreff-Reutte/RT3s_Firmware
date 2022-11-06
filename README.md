# RT3s Custom Firmware

This repo contains a non-functional, but most of the times working firmware for
the RT3s handheld radio.
The project resulted as an attempt to create a user-friendly entry into
firmware hacking and development for the RT3s. I choose platform io, because most
Maker are already familiar with it (Arduino), it also allows the compliation and
upload of the firmware by the press of a button, which is quite nice.

## Goals
* [X] USB Serial
* [X] USB Audio (Microphone and Speaker) _needs more work_
* [X] Add FreeRTOS
* [X] Implement USB Serial via FreeRTOS
* [ ] Analog 70cm / 2m RX
* [ ] Analog 70cm / 2m TX
* [ ] Keyboard input handler
* [ ] Display handler
* [ ] Remote control via PC
* [ ] Stream audio to radio from PC and vice versa (full remote control)
* ... _more will follow_

## Flashing the Firmware

* Set the radio in the DFU mode (press the PTT button and the top button and
turn on the radio)
* Now the top led should blink between red and green.
* Connect the radio to the PC via des USB cable
* Press the upload-button in vs-code (->)
* Now the programm should be compiled and uploaded to the radio.
* If an error occurs follow the following steps

### Error Windows
* Can you find the radio in the device manager?
* If you launch radio_tool manually (located in `/scripts/radio_tool/radio_tool.exe`)
with the parameter `-h 1` does it print the help page. (CMD: `./radio_tool -h 1`)
* If not you need to make radio_tool runnable maybe visite the radio_tool github
repo for a new version: https://github.com/v0l/radio_tool
* If the help is displayed try running CMD: `./radio_tool.exe -l`
* If no devices are listed try installing the WinUSB driver on the radio via zadig
(https://zadig.akeo.ie/) (Tipp: if no devices are listed in zadig, go to options
and check: "List All Devices") After that install the WinUSB driver on the radio.
* If you get the error message `Failed to enumerate serial ports` while running
`./radio_tool.exe -l` then you can fix this by plugging in any other devices that
opens a serial port (COM port). After that you can unplug the serial devices and
the tool should now work. This error is due to a missing registry entry in windows
(`HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM`)`and plugging in a serial
devices creates this entry. (This bug was already reported: https://github.com/v0l/radio_tool/issues/24)
* The build/flash pipline uses python3 so make sure that python3 is installed and useable
via `python3`

### Error Linux
* Make sure that the nessesary packages are installed for radio_tool and that
python3 is installed and runnable via `python3`
* For radio_tool requirements you can visit the repo: https://github.com/v0l/radio_tool

## Contribution

You can fork this repo and or contribute, but maybe get in touch with me first.
See contact at the HTR website.


## Special thanks

* Thanks to the FreeRTOS forum for helping me getting FreeRTOS running on the radio.
* Thanks to the creator of the radio_tool for creating a tool that makes the
flashing process so much easier.
* Thanks to the OpenRTX project for helping getting started with my project, I hope
I can contribute to them at a later time.

## Additional Notes

This is more like a hobby project and in no way a standalone firmware. My main
goals will be to get the RX and TX running and to maybe control the radio via
USB from a PC (commands via usb-serial + audiostreaming (mic + speaker)).