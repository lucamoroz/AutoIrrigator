# AutoIrrigator
This projects allows to setup an automated solution for the irrigation of a garden.
The setup is made via web by defining a set of time intervals during which water will be delivered.

## Prerequisites
Install platformio core:
https://docs.platformio.org/en/latest/core/installation.html

## Build & Upload
The following steps are required:
1. Define the port that is connected to the board by changing the `upload_port` value in the `platformio.ini` file.
See https://docs.platformio.org/en/latest/projectconf/section_env_upload.html#upload-port for more.
2. Open the file `src/CONFIG.h` and define:
    - WiFi `SSID` & `PASSWORD`
    - Time zone values (`HOUR_OFFSET` and `MINUTES_OFFSET`)
    - Digital pin connected to the water pump (`WATER_PUMP_PIN`)
    - If you are not using the ESP 12-E, you may want to change the EEPROM memory size (`EEPROM_SIZE`)

Finally you can:
- Build project: `platformio run`
- Upload to board: `platformio run -- target upload`

## Hardware setup
TODO

## Available platforms & boards
Currently the project has been tested only on the espressif8266 platform (ESP 12E) with the Arduino framework.

You can change platform & board by modifying the platform.ini file: see https://docs.platformio.org/en/latest/projectconf/section_env_platform.html for more information.

Note that the only dependency is the NTP client, that is available for a wide range of platforms shown here:
https://platformio.org/lib/show/1734/EasyNTPClient/ 

## Further development
Here follows a "nice to have" list of features:
- Check via internet the weather to avoid irrigating when raining
- Improve web page look
