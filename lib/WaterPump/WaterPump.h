#ifndef AUTOIRRIGATOR_WATERPUMP_H
#define AUTOIRRIGATOR_WATERPUMP_H

#include "EEPROM.h"
#include "Arduino.h"
#include "../RemoteTime/RemoteTime.h"

using namespace std;

#define EVERYDAY 7


struct IrrigationInterval {
    // Id is set by this class
    uint8_t id;
    uint8_t day;
    uint8_t fromHour;
    uint8_t fromMin;
    uint8_t toHour;
    uint8_t toMin;
};

/**
 * Class that handles a water pump attached to GPIO.
 * To work properly, before instantiating the class, EEPROM.begin(size) must be called.
 *
 * On instantiation previously set intervals are restored.
 */
class WaterPump {
public:
    RemoteTime &remoteTime;
    uint8_t pumpPin;
    vector<IrrigationInterval> intervals;
    bool isEnabled = true;
    bool isActive = false;
    bool isForcedOn = false;
    long forcedOnRemainingMillis = -1;

    WaterPump(uint8_t pumpPin, RemoteTime &remoteTime) : pumpPin(pumpPin), remoteTime(remoteTime) {
        pinMode(pumpPin, OUTPUT);
        // todo save & restore enabled
        this->intervals = loadIntervals();
    }

    void enable() {
        this->isEnabled = true;
    }

    void disable() {
        this->isEnabled = false;
    }

    void turnOn() {
        if (!this->isActive) {
            this->isActive = true;
            digitalWrite(this->pumpPin, HIGH);
        }
    }

    void turnOnTimed(long seconds) {
        if (seconds <= 0) return;

        this->forceDurationMillis = seconds * 1000;
        this->isForcedOn = true;
        this->forceStartTimestamp = millis();
        turnOn();
    }

    void turnOff() {
        if (this->isForcedOn) {
            this->isForcedOn = false;
        }

        if (this->isActive) {
            this->isActive = false;
            digitalWrite(this->pumpPin, LOW);
        }
    }

    /**
     * Must be called repeatedly to update the current status.
     */
    void check() {

        // Update forced-on status if necessary
        if (this->isForcedOn) {
            long nowTimestamp = millis();
            this->forcedOnRemainingMillis = forceDurationMillis - (nowTimestamp - forceStartTimestamp);
            if ( forcedOnRemainingMillis < 0) {
                this->isForcedOn = false;
            } else {
                this->isForcedOn = true;
            }
        }

        bool shouldBeOn = false;
        if (!this->isForcedOn && this->isEnabled) {
            struct tm *now = remoteTime.getTime();
            shouldBeOn = intervalsContain(now);
        }

        if (this->isForcedOn || shouldBeOn)
            turnOn();
        else
            turnOff();
    }

    /**
     * Saves intervals to device EEPROM memory.
     * @modifies newIntervals setting a new ID.
     * @param newIntervals
     */
    void setIrrigationIntervals(vector<IrrigationInterval> &newIntervals) {
        uint8_t addr = 0; // TODO init starting address and set maximum usable memory

        EEPROM.write(addr, (uint8_t) newIntervals.size());

        addr += sizeof(uint8_t);

        for (int i=0; i<newIntervals.size(); i++) {
            newIntervals[i].id = addr;
            EEPROM.put(addr, newIntervals[i]);
            addr += sizeof(IrrigationInterval);
        }

        EEPROM.commit();
        this->intervals = newIntervals;
    }

    void addIrrigationInterval(const IrrigationInterval &interval) {
        vector<IrrigationInterval> newIntervals;
        for (const auto &oldInterval : intervals)
            newIntervals.push_back(oldInterval);

        newIntervals.push_back(interval);
        setIrrigationIntervals(newIntervals);
    }

    void removeIrrigationInterval(uint8_t id) {
        vector<IrrigationInterval> newIntervals;
        for (const auto &interval : intervals) {
            if (interval.id != id)
                newIntervals.push_back(interval);
        }

        setIrrigationIntervals(newIntervals);
    }

    struct tm* getWaterPumpTime() {
        return remoteTime.getTime();
    }

private:
    long forceStartTimestamp;
    long forceDurationMillis = -1;

    vector<IrrigationInterval> loadIntervals() {
        uint8_t addr = 0;
        uint8_t size = EEPROM.read(addr);
        addr += sizeof(uint8_t);

        if (size <= 0)
            return vector<IrrigationInterval>();

        vector<IrrigationInterval> loadedIntervals(size);

        for (int i = 0; i < size; i++) {
            EEPROM.get(addr, loadedIntervals[i]);
            addr += sizeof(IrrigationInterval);
        }

        return loadedIntervals;
    }

    bool intervalsContain(struct tm *time) {
        for (const auto& interval : intervals) {

            if (
                    (interval.day == EVERYDAY || interval.day == time->tm_wday) &&
                    (interval.fromHour <= time->tm_hour && time->tm_hour <= interval.toHour) &&
                    (interval.fromMin <= time->tm_min && time->tm_min <= interval.toMin)
                    ) {
                return true;
            }

        }

        return false;
    }
};

#endif //AUTOIRRIGATOR_WATERPUMP_H
