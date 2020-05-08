#ifndef WebServerAgent_h
#define WebServerAgent_h
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "CommandHandler.h"
#include "LogHandler.h"

const String PARAM_COMMAND = "command";
const String PARAM_FILE = "file";
const String SETTINGS_FILE = "/config.json";

class WebServerAgent
{
public:
    AsyncWebServer *server;
    WebServerAgent();
    void begin();
    CommandHandler commandHandler;
};

WebServerAgent::WebServerAgent()
{
    server = new AsyncWebServer(80);
}
//TODO cors on all responses
void WebServerAgent::begin()
{
    LOG.trace(F("Adding [GET] '/' handler"));
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, F("text/plain"), F("Hello from MCU"));
    });

    LOG.trace(F("Adding [GET] '/data' handler"));
    server->on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncJsonResponse *response = new AsyncJsonResponse();
        response->addHeader(F("Access-Control-Allow-Origin"), "*");
        JsonVariant &root = response->getRoot();
        root["heap"] = ESP.getFreeHeap();
        root["vcc"] = ESP.getVcc();
        root["ssid"] = WiFi.SSID();
        response->setLength();
        request->send(response);
    });

    LOG.trace(F("Adding [GET] '/fs' handler"));
    server->on("/fs", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam(PARAM_FILE)) {
            String param = request->getParam(PARAM_FILE)->value();
            LOG.trace(F("/fs requesting file: %s"), param.c_str());
            if (SPIFFS.exists(param)) {
                AsyncWebServerResponse *response = request->beginResponse(SPIFFS, param);
                response->addHeader(F("Access-Control-Allow-Origin"), "*");
                request->send(response);
            } else {
                request->send(404, F("text/plain"), "File '" + param + "' not found on fs");
            }
        } else {
            request->send(400, F("text/plain"), "Query parameter 'file' missing");
        }
        
    });

    LOG.trace(F("Adding [DELETE] '/fs' handler"));
    server->on("/fs", HTTP_DELETE, [](AsyncWebServerRequest *request) {
        if (request->hasParam(PARAM_FILE)) {
            String param = request->getParam(PARAM_FILE)->value();
            if (SPIFFS.exists(param)) {
                LOG.verbose(F("/fs deleting file: %s"), param.c_str());
                SPIFFS.remove(param);
                AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "File " + param + " deleted");
                response->addHeader(F("Access-Control-Allow-Origin"), F("*"));
                request->send(response);
            } else {
                request->send(404, F("text/plain"), "File '" + param + "' not found on fs");
            }
        } else {
            request->send(400, F("text/plain"), "Query parameter 'file' missing");
        }
    });

    LOG.trace(F("Adding [POST] '/command' handler"));
    server->on("/command", HTTP_POST, [this](AsyncWebServerRequest *request) {
        
        if (request->hasParam(PARAM_COMMAND, true))
        {
            String command = request->getParam(PARAM_COMMAND, true)->value();
            String commandOutput = commandHandler.handle(command);
            AsyncWebServerResponse *response = request->beginResponse(200, F("text/plain"), commandOutput);
            response->addHeader(F("Access-Control-Allow-Origin"), F("*"));
            request->send(response);
        }
        else
        {
            AsyncWebServerResponse *response = request->beginResponse(400, F("text/plain"), "'command' parameter not found in request");
            response->addHeader(F("Access-Control-Allow-Origin"), F("*"));
            request->send(response);
        }
    });

    LOG.trace(F("Adding [POST] '/config' handler"));
    AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/config", [](AsyncWebServerRequest *request, JsonVariant &json) {
        JsonObject jsonObj = json.as<JsonObject>();
        File file = SPIFFS.open(SETTINGS_FILE, "w+");
        serializeJsonPretty(jsonObj, file);
        request->send(200, F("text/plain"), F("Stored as '/config.json'"));
        file.flush();
        file.close();
    });
    server->addHandler(handler);


    LOG.trace(F("Adding 404 handler"));
    server->onNotFound([](AsyncWebServerRequest *r) { r->send(404, F("text/plain"), "Not found"); });
    server->begin();
    LOG.verbose(F("WebServerAgent started"));
}

#endif