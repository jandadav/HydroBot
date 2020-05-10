#include <Arduino.h>
#include "WifiAgent.h"
#include "WebServerAgent.h"
#include <FS.h> 
#include "LogHandler.h"
#include "TimeHandler.h"
#include "OtaHandler.h"
#include "Relays.h"
#include "Distance.h"
#include "AlarmHandler.h"

WifiAgent wifiAgent;
WebServerAgent webServerAgent;
LogHandler logHandler;
TimeHandler timeHandler;
Relays relays;
Distance distance;
AlarmHandler alarmHandler;

void proc();

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
  //webServerAgent.commandHandler.addCommandCallback("deser", [](String c) {alarmHandler.setup(); return (String)("parsing"); });
  webServerAgent.commandHandler.addCommandCallback("reset", [](String c) {ESP.reset(); return (String)(""); });

  relays.setup();

  distance.setup();

  OtaStart("hydrobot");

  alarmHandler.setup(proc);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  LOG.verbose(F("=== STARTUP COMPLETE ==="));

            
}

void loop(void) {
  OtaUpdate();
  timeHandler.update();
  alarmHandler.update();
}

void proc() {
    LOG.verbose("ALARM BZZZZZ");
}
