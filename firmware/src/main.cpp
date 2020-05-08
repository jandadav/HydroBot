#include <Arduino.h>
#include "WifiAgent.h"
#include "WebServerAgent.h"
#include <FS.h> 
#include "LogHandler.h"
#include "TimeHandler.h"
#include "OtaHandler.h"

#define PIN D1

WifiAgent wifiAgent;
WebServerAgent webServerAgent;
LogHandler logHandler;
TimeHandler timeHandler;

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

  webServerAgent.commandHandler.addCommandCallback("pin", [](String c) { 
    digitalWrite(PIN, !digitalRead(PIN));
    return String(digitalRead(PIN)); 
  });

  OtaStart("hydrobot");

  LOG.verbose(F("=== STARTUP COMPLETE ==="));

  pinMode(PIN, OUTPUT);
}

void loop(void) {
  timeHandler.update();
  OtaUpdate();
  delay(200);
}