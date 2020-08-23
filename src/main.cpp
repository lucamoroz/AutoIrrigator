#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <../lib/WaterPump/WaterPump.h>
#include <../lib/WetServer.h>
#include <CONFIG.h>

WiFiUDP udp;

RemoteTime *remoteTime;
WaterPump *pump;
WetServer *server;

void setup() {
    EEPROM.begin(EEPROM_SIZE);
    Serial.begin(9600);

    remoteTime = new RemoteTime(udp, HOUR_OFFSET, MINUTES_OFFSET);
    pump = new WaterPump(WATER_PUMP_PIN, *remoteTime);
    server = new WetServer(*pump);

    // Wait for WiFi connection
    Serial.println("Connecting to WiFi...");
    WiFi.hostname(HOSTNAME);
    WiFi.mode(WIFI_STA); // Default is both client and access point
    WiFi.begin(SSID, PASSWORD);
    bool ledBlink = false;
    pinMode(LED_BUILTIN_AUX, OUTPUT);
    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(LED_BUILTIN_AUX, ledBlink);
        ledBlink = !ledBlink;
        delay(500);
    }
    Serial.println("WiFi connected!");
    digitalWrite(LED_BUILTIN_AUX, false);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
}

void loop() {

    if (WiFi.isConnected()) {
        pump->check();
        server->handleClient();
    } else {
        WiFi.reconnect();
    }

    delay(200);
}