#ifndef ALARMHANDLER_h
#define ALARMHANDLER_h
#include <Arduino.h>
#include <TimeAlarms.h>
#include <ArduinoJson.h>
#include <fs.h>
#include <Arduino.h>
#include "LogHandler.h"

class AlarmHandler {
    public:
        void setup(void (*f)());
        void update();
    private:
        const String _configFile = "/config.json";
};



void AlarmHandler::setup(void (*f)()) {
//     Alarm.timerRepeat(15, repeats);            // timer for every 15 seconds    
//   Alarm.alarmRepeat(19, 45, 0, timerAlarm);
    if(SPIFFS.exists(_configFile)){
        File file = SPIFFS.open(_configFile, "r");
        StaticJsonDocument<1024> doc;

        StaticJsonDocument<200> filter;
        filter["water"] = true;

        DeserializationError err = deserializeJson(doc, file, DeserializationOption::Filter(filter));

        if (err) {
            LOG.error("Reading config failed: %s", err);
        }
        
        //serializeJsonPretty(doc, Serial);
        
        JsonArray array = doc["water"].as<JsonArray>();
        
        for(JsonVariant v : array) {
            
            String s = v.as<String>();
            
            int hour = s.substring(0, s.indexOf(':')).toInt();
            int minute = s.substring(s.indexOf(':') + 1).toInt();
            LOG.verbose("Scheduling daily alarm at %d:%d", hour, minute);
            Alarm.alarmRepeat(hour, minute, 0, f);
        }

        file.close();
    } else {
        LOG.error("Cannot find '/config.json'");
    }
    
    
}

void AlarmHandler::update() {
    Alarm.delay(1);
}

#endif