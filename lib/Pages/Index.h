#ifndef AUTOIRRIGATOR_INDEX_H
#define AUTOIRRIGATOR_INDEX_H

#include <Arduino.h>
#include <sstream>
#include "../WaterPump/WaterPump.h"

using namespace std;

namespace Index {

    void getHtmlIntervalsList(const vector<IrrigationInterval> &intervals, string &res) {
        string intervalsMsg = "";
        intervalsMsg += "<ul>";
        for (const auto& intv : intervals) {

            intervalsMsg += "<li>";
            intervalsMsg += String(intv.fromHour).c_str();
            intervalsMsg += ":";
            intervalsMsg += String(intv.fromMin).c_str();
            intervalsMsg += " - ";
            intervalsMsg += String(intv.toHour).c_str();
            intervalsMsg += ":";
            intervalsMsg += String(intv.toMin).c_str();
            intervalsMsg += " - ";
            intervalsMsg += dayToString[intv.day].c_str();

            intervalsMsg += R"(<form action="removeIrrigationInterval" method="get">)";
            intervalsMsg += R"(<input type="hidden" name="id" value=")";
            intervalsMsg += String(intv.id).c_str();
            intervalsMsg += R"("><button type="submit">Remove interval</button></form>)";

            intervalsMsg += "</li>";

        }

        intervalsMsg += "</ul>";

        res = intervalsMsg;
    }

    /**
     * Render Index page.
     * @param msg any message that can be used to notify something to the users (eg errors)
     * @param pump
     * @return Index page with HTML syntax
     */
    String getPage(WaterPump &pump, const String& msg = "") {
        char str[3000];

        const char* enabledMsg = pump.isEnabled ? "<font color=\"darkgreen\">enabled</font>" : "<font color=\"darkgrey\">disabled</font>";
        const char* activeMsg = pump.isActive ? "<font color=\"darkgreen\">active</font>" : "<font color=\"darkgrey\">inactive</font>";

        char forceTimeMsg[80];

        if (pump.isForcedOn) {
            snprintf(forceTimeMsg, 80, "<br><font color=\"red\">Warning</font>: remaining force-time seconds: %ld", pump.forcedOnRemainingMillis/1000);
        } else {
            sprintf(forceTimeMsg, "");
        }

        char timeMsg[30];

        // TODO this thing has problems
        //strftime(timeMsg, 30, "%d-%b-%Y %H:%M", pump.getWaterPumpTime());

        string intervalsMsg;
        getHtmlIntervalsList(pump.intervals, intervalsMsg);

        snprintf(str, 3000, R"(
<html>
<head>
<title>Dashboard</title>
</head
<body>
<h2>AutoIrrigator dashboard</h2>

<b>%s</b>

<br>

<b>Water pump status</b>: %s, %s

%s

<br><br>

Water pump time: %s

<br><br>

%s

<br>

<form action="addIrrigationInterval" method="get">
    <input type="time" name="fromTime">
    <input type="time" name="toTime">
    <select name="day">
        <option value="0">SUNDAY</option>
        <option value="1">MONDAY</option>
        <option value="2">TUESDAY</option>
        <option value="3">WEDNESDAY</option>
        <option value="4">THURSDAY</option>
        <option value="5">FRIDAY</option>
        <option value="6">SATURDAY</option>
        <option value="7">EVERYDAY</option>
    </select>
    <button type="submit">Add interval</button>
</form>

<br>

Turn on the water pump for a desired number of seconds:
<br><br>
<form action="/turnOnTimed" method="get">
    <input type="number" id="seconds" name="seconds" placeholder="seconds" min="0">
    <button type="submit">Force on timed</button>
</form>
<form action="/turnOff">
    <button type=submit">Stop timed-on</button>
</form>

<br><br>
<form action="/toggleWaterPumpStatus">
    <button type="submit">Enable/Disable water pump</button>
</form>

</body>

</html>

        )", msg.c_str(), enabledMsg, activeMsg, forceTimeMsg, "timeMsg", intervalsMsg.c_str());

        return str;
    }

}


#endif //AUTOIRRIGATOR_INDEX_H
