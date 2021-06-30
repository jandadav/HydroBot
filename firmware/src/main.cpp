#include <Arduino.h>
#include "WifiAgent.h"
#include "WebServerAgent.h"
#include <FS.h> 
#include "LogHandler.h"
#include "TimeHandler.h"
//#include "OtaHandler.h"
#include "Relays.h"
#include "Distance.h"
#include "AlarmHandler.h"
#include "Config.h"

WifiAgent wifiAgent;
WebServerAgent webServerAgent;
LogHandler logHandler;
TimeHandler timeHandler;
Relays relays;
Distance distance;
Config config;

enum SystemState {fill, idle};

AlarmHandler alarmHandler;
SystemState activeState = idle;

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
  webServerAgent.commandHandler.addCommandCallback("waterLevel", [](String c) {return String(distance.getDistanceCm());});
  webServerAgent.commandHandler.addCommandCallback("reset", [](String c) {ESP.reset(); return (String)(""); });
  webServerAgent.commandHandler.addCommandCallback("start", [](String c) {activeState = fill; LOG.verbose("cycle start"); return String("cycle start");});
  webServerAgent.commandHandler.addCommandCallback("stop", [](String c) {activeState = idle; LOG.verbose("cycle stop"); return String("cycle stop");});

  relays.setup();

  distance.setup();

  //OtaStart("hydrobot");

  config.initialize();


  alarmHandler.setup(proc);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  LOG.verbose(F("=== STARTUP COMPLETE ==="));

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

}

void loop(void) {

  if(activeState == fill) {
    if (distance.getDistanceCm() < config.waterLevelHigh) {
      activeState = idle;
    }
    relays.pumpOn();
  }

  if(activeState == idle) {
    relays.pumpOff();
  }  

  //OtaUpdate();
  timeHandler.update();

  alarmHandler.update(400);
}

void proc() {
    LOG.verbose("Starting watering cycle");
    activeState = fill;
}


