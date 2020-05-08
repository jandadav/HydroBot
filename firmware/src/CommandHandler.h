#ifndef CommandHandler_h
#define CommandHandler_h
#include <Arduino.h>
#include "LogHandler.h"

class CommandHandler
{
public:
    String handle(String command);
    void addCommandCallback(String command, String (*f) (String) );
private:
    String keyList[100];
    String (*functionList[100]) (String);
    int callbacksStored = 0;
};

String CommandHandler::handle(String command)
{
    LOG.verbose(F("Handling command: %s"), command.c_str());
    String output = F("No handler found");
    for (int i = 0; i < callbacksStored; i++){
        LOG.trace(F("Looking up callback: %s"), keyList[i].c_str());
        if( command.startsWith(keyList[i]) ) {
            LOG.trace(F("Found match, executing: %s"), keyList[i].c_str());
            output =  (*functionList[i]) ( command.substring(keyList[i].length() + 1) ); 
            break;
        }
    }
    return output;
};

void CommandHandler::addCommandCallback(String command, String (*f) (String)) {
    // TODO: handle uniqueness of keys
    keyList[callbacksStored] = command;
    functionList[callbacksStored] = f;
    callbacksStored++;
    LOG.trace(F("Added command callback: %s, total: %d"),command.c_str(), callbacksStored);
};

#endif