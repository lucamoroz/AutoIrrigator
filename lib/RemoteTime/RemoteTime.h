#ifndef AUTOIRRIGATOR_REMOTETIME_H
#define AUTOIRRIGATOR_REMOTETIME_H

#include <Arduino.h>
#include <time.h>
#include <WiFiUdp.h>
#include "../../.pio/libdeps/nodemcu/EasyNTPClient/src/EasyNTPClient.h"

const unsigned long UPDATE_INTERVAL = 6 * 3600; // seconds

const String dayToString[] = {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Everyday"
};

class RemoteTime {
public:

    RemoteTime(WiFiUDP &udp, uint8_t hOffset, uint8_t mOffset)
        : ntpClient(udp, "pool.ntp.org", ((hOffset*60*60)+(mOffset*60))) { }

    struct tm* getTime() {
        time_t unixTimestamp = getUnixTime();
        return localtime(&unixTimestamp);
    }

private:
    unsigned long getUnixTime() {
        // Arduino timestamp overflows after ~50 days and reset to zero
        unsigned long currSeconds = Arduino_h::millis() / 1000;

        if (lastUpdateSeconds == 0 || (currSeconds - lastUpdateSeconds) > UPDATE_INTERVAL) {
            lastUpdateSeconds = currSeconds;
            lastUnixTimestamp = ntpClient.getUnixTime(); // Returns Unix time in seconds
            return lastUnixTimestamp;
        } else {
            unsigned long diff = currSeconds - lastUpdateSeconds;
            return (lastUnixTimestamp + diff);
        }
    }

private:
    EasyNTPClient ntpClient;
    unsigned long lastUpdateSeconds = 0;
    unsigned long lastUnixTimestamp = 0;
};

#endif //AUTOIRRIGATOR_REMOTETIME_H
