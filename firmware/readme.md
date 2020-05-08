## ESP8266 controller

## Before Build

src/Secrets.h file is required to be created before building, that has the following structure. This file is not checked into scm.
```c
#ifndef Secrets_h
#define Secrets_h
#define STASSID "SSID"
#define STAPSK  "PASS"
#define OTAPSK  "OTAPASS"
#endif
```

## OTA capability:

When device is built in, you can have option to flash over the air. No precautions are made in the firmware to get device running program into stable state ready for flash. Care should be taken.

using EspOTA pio upload setting the following in platformio.ini
```
upload_protocol = espota
upload_port = mcu.local
```
The flash will use the following settings:
```
{'esp_ip': 'mcu.local', 'host_ip': '0.0.0.0', 'esp_port': 8266, 'host_port': 36130, 'auth': '******', 'image': '.pio\\build\\nodemcuv2\\firmware.bin', 'spiffs': False, 'debug': True, 'progress': True}
```
or invoke directly with the following command:
```
python ~/.platformio/packages/framework-arduinoespressif8266/tools/espota.py --ip=mcu.local --host_ip=0.0.0.0 --port=8266 --host_port=36130 --auth=******** --file=.pio/build/nodemcuv2/firmware.bin --debug -- progress
```