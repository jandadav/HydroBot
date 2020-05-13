#include <Arduino.h>
#include "WifiAgent.h"
#include "WebServerAgent.h"
#include <FS.h> 
#include "LogHandler.h"
#include "TimeHandler.h"
#include "OtaHandler.h"
#include "Relays.h"
#include "Distance.h"

WifiAgent wifiAgent;
WebServerAgent webServerAgent;
LogHandler logHandler;
TimeHandler timeHandler;
Relays relays;
Distance distance;

enum SystemState {active, idle};
SystemState activeState = idle;

boolean pumpOn = false;

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
  // webServerAgent.commandHandler.addCommandCallback("pump", [](String c) { 
  //   if (pumpOn) {
  //     pumpOn = false;
  //   } else {
  //     pumpOn = true;
  //   }
  //   return (String)("inverting pump");
  // });
  webServerAgent.commandHandler.addCommandCallback("waterLevel", [](String c) {return String(distance.getDistanceCm());});
  webServerAgent.commandHandler.addCommandCallback("start", [](String c) {activeState = active; LOG.verbose("cycle start"); return String("cycle start");});
  webServerAgent.commandHandler.addCommandCallback("stop", [](String c) {activeState = idle; LOG.verbose("cycle stop"); return String("cycle stop");});

  relays.setup();

  distance.setup();

  OtaStart("hydrobot");

  LOG.verbose(F("=== STARTUP COMPLETE ==="));
}

void loop(void) {

  // if (pumpOn) {
  //   relays.pumpOn();
  // } else {
  //   relays.pumpOff();
  // }

  if(activeState == active) {
    if (distance.getDistanceCm() < 20) {
      activeState = idle;
    }
    relays.pumpOn();
  } else {
    relays.pumpOff();
       if (distance.getDistanceCm() > 20) {
    activeState = active;
  }
  }

  

  OtaUpdate();
  timeHandler.update();
  delay(200);
}