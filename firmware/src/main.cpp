#include <Arduino.h>
#include "WifiAgent.h"
#include "WebServerAgent.h"
#include <FS.h> 
#include "LogHandler.h"
#include "TimeHandler.h"
#include "OtaHandler.h"
#include "Relays.h"
#include "Distance.h"
#include <TimeAlarms.h>

WifiAgent wifiAgent;
WebServerAgent webServerAgent;
LogHandler logHandler;
TimeHandler timeHandler;
Relays relays;
Distance distance;

void repeats();
void timerAlarm();

void setup(void) {
  // order is important for some
  Serial.begin(115200);
  while (!Serial && !Serial.available()) {};
  delay(300);
  Serial.println();

  logHandler.beginRollingFile();
  LOG.verbose(F("=== SYSTEM STARTING ==="));

  wifiAgent.start();
  webServerAgent.begin();

  timeHandler.start();

  webServerAgent.commandHandler.addCommandCallback("dummy", [](String c) { return (String) ("dummy command handler receiving: "+c);});
  webServerAgent.commandHandler.addCommandCallback("time", [](String c) {char time[20]; timeHandler.getTime(time); return (String)(time); });
  webServerAgent.commandHandler.addCommandCallback("pump", [](String c) { 
    if (c == "on") {
      relays.pumpOn();
    } else {
      relays.pumpOff();
    }
    return String(relays.getPumpState());
  });
  webServerAgent.commandHandler.addCommandCallback("waterLevel", [](String c) {return String(distance.getDistanceCm());});

  relays.setup();

  distance.setup();

  OtaStart("hydrobot");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  LOG.verbose(F("=== STARTUP COMPLETE ==="));

  Alarm.timerRepeat(15, repeats);            // timer for every 15 seconds    
  Alarm.alarmRepeat(19, 45, 0, timerAlarm);              
}

void loop(void) {
  OtaUpdate();
  timeHandler.update();
  Alarm.delay(1);
}

void repeats() {
  Serial.println("releating alarm");
}

void timerAlarm() {
  LOG.verbose("Timer Alarm BZZZZZ");
}