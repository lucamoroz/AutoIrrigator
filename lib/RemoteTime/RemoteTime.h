#ifndef AUTOIRRIGATOR_REMOTETIME_H
#define AUTOIRRIGATOR_REMOTETIME_H

#include <Arduino.h>
#include <time.h>
#include <WiFiUdp.h>
#include "../../.pio/libdeps/nodemcu/EasyNTPClient/src/EasyNTPClient.h"

#define UPDATE_INTERVAL 6 * 3600 * 1000

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
// TODO singleton

class RemoteTime {
public:

    RemoteTime(WiFiUDP &udp, uint8_t hOffset, uint8_t mOffset)
        : ntpClient(udp, "pool.ntp.org", ((hOffset*60*60)+(mOffset*60))) { }

    struct tm* getTime() {
        time_t unixTimestamp = this->ntpClient.getUnixTime();
        return localtime(&unixTimestamp);
    }
    
    uint8_t getWeekDay() {
        time_t unixTimestamp = this->ntpClient.getUnixTime();

        struct tm *now_tm = localtime(&unixTimestamp);
        return now_tm->tm_wday; // days since sunday
    }

    uint8_t getHour() {
        time_t unixTimestamp = this->ntpClient.getUnixTime();

        struct tm *now_tm = localtime(&unixTimestamp);
        return now_tm->tm_hour;
    }

    uint8_t getMinutes() {
        time_t unixTimestamp = this->ntpClient.getUnixTime();
        struct tm *now_tm = localtime(&unixTimestamp);
        return now_tm->tm_min;
    }

    unsigned long getUnixTimeMillis() {
        // TODO FIX to reduce number of net calls
        // Arduino timestamp overflows after ~50 days and reset to zero
        unsigned long currTimestamp = Arduino_h::millis();

        if (currTimestamp < lastUpdateTimestamp || currTimestamp - lastUpdateTimestamp > UPDATE_INTERVAL) {
            this->lastUpdateTimestamp = currTimestamp;
            this->lastUnixTimestamp = ntpClient.getUnixTime() * 1000; // Returns Unix time in seconds
            return this->lastUnixTimestamp;
        } else {
            unsigned long diff = currTimestamp - lastUpdateTimestamp;
            return this->lastUnixTimestamp + diff;
        }
    }

private:
    EasyNTPClient ntpClient;
    unsigned long lastUpdateTimestamp = LONG_MAX;
    unsigned long lastUnixTimestamp = 0;
};

#endif //AUTOIRRIGATOR_REMOTETIME_H
