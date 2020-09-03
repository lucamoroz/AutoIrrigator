# AutoIrrigator
This projects allows to setup an automated solution for the irrigation of a garden.
The setup is made via web by defining a set of time intervals during which water will be delivered.

![](https://i.imgur.com/THlHZGS.png)

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
- Erase EEPROM flash memory (very important for the first upload!): `pio run --target erase`
- Upload to board: `platformio run -- target upload`

## Hardware requirements & setup
The list of components is the following:
- ESP8266 12E or similar boards (other boards have not been tested)
- A small breadboard
- Water pump/valve (e.g. https://www.banggood.com/12V-DC-1-or-2Inch-Electric-N-or-C-Solenoid-Valve-For-Water-Air-p-914446.html?cur_warehouse=CN&rmmds=search)
- Water pump/valve power adapter (e.g. https://www.banggood.com/12V-Power-Supply-AC-to-DC-Adapter-EU-US-Plug-Optional-Power-Converter-for-Electric-Drill-p-1438301.html?rmmds=search&ID=47184&cur_warehouse=CN)
- ESP8266 power supply (you can use a common mobile charger)
- A relay that will be controlled by the microcontroller (e.g. https://www.banggood.com/3Pcs-5V-Relay-5-12V-TTL-Signal-1-Channel-Module-High-Level-Expansion-Board-p-1178211.html?rmmds=search&cur_warehouse=CN)
- Utilities like cables and adapters to connect the water pump/valve to your water source

Intallation:
1. Connect relay VCC and GND to ESP8266 +3V and GND
2. Connect relay SIG to the WATER_PUMP_PIN (see Build & Upload - step 2)
3. Connect water power supply output to a relay pin and power supply GND to water pump/valve GND
4. Connect the remaining relay pin to water pump/valve VCC

![](https://i.imgur.com/arTxd30.jpg)

## Available platforms & boards
Currently the project has been tested only on the espressif8266 platform (ESP 12E) with the Arduino framework.

You can change platform & board by modifying the platform.ini file: see https://docs.platformio.org/en/latest/projectconf/section_env_platform.html for more information.

Note that the only dependency is the NTP client, that is available for a wide range of platforms shown here:
https://platformio.org/lib/show/1734/EasyNTPClient/ 

## Further development
Here follows a "nice to have" list of features:
- Check via internet the weather to avoid irrigating when raining
- Improve web page look
