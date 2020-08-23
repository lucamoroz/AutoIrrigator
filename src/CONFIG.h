#ifndef AUTOIRRIGATOR_CONFIG_H
#define AUTOIRRIGATOR_CONFIG_H

// Insert your WiFi SSID and Password
const char *SSID     = "";
const char *PASSWORD = "";

// Define the name with which you can connect to the device (default is http://autoirrigator/)
const char *HOSTNAME = "AutoIrrigator";

// Depending on where you live you can set a different time zone (default is GMT+0)
const uint8_t HOUR_OFFSET = 0;
const uint8_t MINUTES_OFFSET = 0;

// This value defines the size of the EEPROM memory. In case of the ESP8266 the size is 4096 bytes:
// you may need to change it for different boards.
const int EEPROM_SIZE = 4096;

// Define the digital pin connected to the water pump. Default is the inboard-led for testing
// purposes.
const uint8_t WATER_PUMP_PIN = 16;

#endif //AUTOIRRIGATOR_CONFIG_H
