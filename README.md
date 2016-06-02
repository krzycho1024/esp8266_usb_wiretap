# esp8266_usb_wiretap

WARNING!!! I don't take responsibility for damage that this software or connecting esp8266 to usb may cause.

It's rather proof of concept than fully working and stable solution.

It dumps second(first is request) packet from usb 1.1 1.5mbps wire and sends it to every connected client.

I used ESP-03 module.

pin D+ -> GPIO12
pin D- -> GPIO13

Usb signals are 3.3-3.6V- so there is no need to convert.
ESP needs also power- I connected 3.3V regulator with 10u capacitor.

It can connect directly or with 2.2k resistor- when cpu is booted before connecting USB.
When cpu starts it pulls up both pins- USB doesn't like this and device isn't recoginzed. I used 5V zenner diode to prevent that.
I also connected 10k pulldown.

Software uses Espressif GPIO lib and ESP8266_RTOS_SDK, compiled it using xtensa gcc.

It has some issues- socket needs to reconnect after some time.
I'm not electronic- there could be(and probably is) better way connecting to usb bus.

Wirerapping tested only with Logitech K120 keybord and Windows 7/8.1. Other OS could handle USB a bit dirrerent and this could not work.
USB ports hardware: Dell Latitude 3340(USB 3.0 port) and Lenovo Miix 2 10 ports in docking station(USB 2.0).

Client is wriiten in .net, compiled using Visual Studio 2015.

Support boot_v1.2 and +
user1.1024.new.2.bin--->0x01000

thanks:
jcmvbkbc http://bbs.espressif.com/viewtopic.php?t=200 for get_ccount
http://espressif.com/en/products/software/esp-mesh for tcp server example
dxguidan, hreintke http://www.esp8266.com/viewtopic.php?f=13&t=1259 for gpio code
