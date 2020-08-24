#ifndef AUTOIRRIGATOR_WETSERVER_H
#define AUTOIRRIGATOR_WETSERVER_H

#include <ESP8266WebServer.h>
#include "Pages/Index.h"
#include "WaterPump/WaterPump.h"

ESP8266WebServer espServer(80);

// TODO to improve extensibility, there could be a "master" server that calls init() for each service (like WetServer)

class WetServer {
public:

    WaterPump &waterPump;

    WetServer(WaterPump &waterPump) : waterPump(waterPump) {

        espServer.on("/", std::bind(&WetServer::handleRoot, this));
        espServer.on("/enable", std::bind(&WetServer::handleEnable, this));
        espServer.on("/disable", std::bind(&WetServer::handleDisable, this));
        espServer.on("/turnOn", std::bind(&WetServer::handleTurnOn, this));
        espServer.on("/turnOff", std::bind(&WetServer::handleTurnOff, this));
        espServer.on("/turnOnTimed", std::bind(&WetServer::handleTurnOnTimed, this));
        espServer.on("/addIrrigationInterval", std::bind(&WetServer::handleAddIrrigationInterval, this));
        espServer.on("/removeIrrigationInterval", std::bind(&WetServer::handleRemoveIrrigationInterval, this));
        espServer.on("/toggleWaterPumpStatus", std::bind(&WetServer::handleToggleWaterPumpStatus, this));

        espServer.begin();
    }

    /**
     * Must be called repeatedly to accept incoming HTTP requests.
     */
    void handleClient() {
        espServer.handleClient();
    }

private:

    void handleRoot() {
        String page;
        if (espServer.args() > 0 && !espServer.arg("msg").isEmpty())
            page = Index::getPage(waterPump, espServer.arg("msg"));
        else
            page = Index::getPage(waterPump);

        espServer.send(200, "text/html", page);
    }

    void handleTurnOn() {
        waterPump.turnOn();
        redirectToIndex();
    }

    void handleTurnOff() {
        waterPump.turnOff();
        redirectToIndex();
    }

    void handleEnable() {
        waterPump.enable();
        redirectToIndex();
    }

    void handleDisable() {
        waterPump.disable();
        redirectToIndex();
    }

    void handleTurnOnTimed() {
        long seconds = espServer.arg("seconds").toInt();
        if (seconds > 0) {
            waterPump.turnOnTimed(seconds);
            redirectToIndex();
        } else {
            redirectToIndex("Error: invalid seconds.");
        }

    }

    void handleToggleWaterPumpStatus() {
        if (waterPump.isEnabled)
            waterPump.disable();
        else
            waterPump.enable();

        redirectToIndex();
    }

    void handleAddIrrigationInterval() {
        uint8_t day = espServer.arg("day").toInt();

        String from = espServer.arg("fromTime");
        String to = espServer.arg("toTime");

        uint8_t fromHour = from.substring(0, from.indexOf(':')).toInt();
        uint8_t fromMin = from.substring( from.indexOf(':') + 1, from.length()).toInt();

        uint8_t toHour = to.substring(0, to.indexOf(':')).toInt();
        uint8_t toMin = to.substring( to.indexOf(':') + 1, to.length()).toInt();

        if (fromHour > toHour || ((fromHour == toHour) && fromMin > toMin) || from.isEmpty() || to.isEmpty()) {
            redirectToIndex("Error: invalid interval.");
            return;
        }

        IrrigationInterval interval{0, day, fromHour, fromMin, toHour, toMin};
        waterPump.addIrrigationInterval(interval);

        redirectToIndex();
    }

    void handleRemoveIrrigationInterval() {
        uint8_t id = espServer.arg("id").toInt();
        waterPump.removeIrrigationInterval(id);
        redirectToIndex();
    }

    void redirectToIndex(String msg = "") {
        String value = msg.isEmpty() ? "/" : "/?msg=" + msg;
        espServer.sendHeader("Location", value, true);

        espServer.send ( 302, "text/plain", "");
    }


};

#endif //AUTOIRRIGATOR_WETSERVER_H
