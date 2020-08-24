#ifndef AUTOIRRIGATOR_WATERPUMP_H
#define AUTOIRRIGATOR_WATERPUMP_H

#include "EEPROM.h"
#include "Arduino.h"
#include "../RemoteTime/RemoteTime.h"

using namespace std;

const int EEPROM_ADDR_BEGIN = 0;
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
 * Class that handles a water pump attached to GPIO (pin: WATER_PUMP_PIN in CONFIG.h).
 * To work properly, before instantiating the class, EEPROM.begin(size) must be called.
 *
 * On creation previously set intervals are restored using EEPROM memory.
 */
class WaterPump {
public:
    uint8_t pumpPin;
    RemoteTime &remoteTime;

    vector<IrrigationInterval> intervals;
    bool isEnabled = true;
    bool isActive = false;
    bool isForcedOn = false;
    long forcedOnRemainingMillis = -1;

    WaterPump(uint8_t pumpPin, RemoteTime &remoteTime) : pumpPin(pumpPin), remoteTime(remoteTime) {
        pinMode(pumpPin, OUTPUT);
        this->isEnabled = loadIsEnabled();
        this->intervals = loadIntervals();
    }

    void enable() {
        this->isEnabled = true;
        saveIsEnabled(this->isEnabled);
    }

    void disable() {
        this->isEnabled = false;
        saveIsEnabled(this->isEnabled);
    }

    void turnOn() {
        if (!this->isActive) {
            this->isActive = true;
            digitalWrite(this->pumpPin, HIGH);
        }
    }

    /**
     * Activates the water pump for the given seconds.
     * @param seconds
     */
    void turnOnTimed(long seconds) {
        if (seconds <= 0) return;

        this->forceDurationMillis = seconds * 1000;
        this->isForcedOn = true;
        this->forceStartTimestamp = millis();
        turnOn();
    }

    /**
     * Turn of the water pump and any timed-activation.
     */
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
     * Must be called repeatedly to update the current status and eventually activate the water pump.
     */
    void check() {

        // Update forced-on status if necessary
        if (this->isForcedOn) {
            unsigned long nowTimestamp = millis();
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
            shouldBeOn = intervalsContain(now, this->intervals);
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
        // +1 for water pump status
        int addr = EEPROM_ADDR_BEGIN + 1;

        EEPROM.write(addr, (uint8_t) newIntervals.size());

        addr += sizeof(uint8_t);

        for (auto & newInterval : newIntervals) {
            newInterval.id = addr;
            EEPROM.put(addr, newInterval);
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
    unsigned long forceStartTimestamp = 0;
    unsigned long forceDurationMillis = 0;

    static vector<IrrigationInterval> loadIntervals() {
        // +1 for water pump status
        int addr = EEPROM_ADDR_BEGIN + 1;

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

    static void saveIsEnabled(bool isEnabled) {
        int addr = EEPROM_ADDR_BEGIN;
        EEPROM.write(addr, isEnabled);
        EEPROM.commit();
    }

    static bool loadIsEnabled() {
        return EEPROM.read(EEPROM_ADDR_BEGIN);
    }

    static bool intervalsContain(struct tm *time, vector<IrrigationInterval> intervals) {
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
