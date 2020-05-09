#include <Arduino.h>
#include "WifiAgent.h"
#include "WebServerAgent.h"
#include <FS.h> 
#include "LogHandler.h"
#include "TimeHandler.h"
#include "OtaHandler.h"
#include "Relays.h"

WifiAgent wifiAgent;
WebServerAgent webServerAgent;
LogHandler logHandler;
TimeHandler timeHandler;
Relays relays;

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
  //TODO is this memmory leak?
  webServerAgent.commandHandler.addCommandCallback("time", [](String c) {char time[20]; timeHandler.getTime(time); return (String)(time); });

  webServerAgent.commandHandler.addCommandCallback("pump", [](String c) { 
    if (c == "on") {
      relays.pumpOn();
    } else {
      relays.pumpOff();
    }
    return String(relays.getPumpState());
  });

  relays.setup();

  OtaStart("hydrobot");

  LOG.verbose(F("=== STARTUP COMPLETE ==="));

}

void loop(void) {
  OtaUpdate();
  timeHandler.update();
  delay(200);
}