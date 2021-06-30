#ifndef Config_h
#define Config_h
#include <Arduino.h>
#include <fs.h>
#include "LogHandler.h"

//TODO hot reload on config post

class Config {

public:
    void initialize();
    double waterLevelHigh = 22;
private:
    const String _configFile = "/config.json";
};

void Config::initialize() {
    /*if(SPIFFS.exists(_configFile)){
        File file = SPIFFS.open(_configFile, "r");
        StaticJsonDocument<1024> doc;

        DeserializationError err = deserializeJson(doc, file);

        if (err) {
            LOG.error("Reading config failed: %s", err);
        }
        waterLevelHigh = doc["waterLevelHigh"];

        file.close();
    } else {
        LOG.error("Cannot find '/config.json'");
    }*/
}

#endif