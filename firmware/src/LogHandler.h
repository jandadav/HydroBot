#ifndef Logger_h
#define Logger_h
#include <stdarg.h>
#include <Log4Esp.h>
#include "CustomFileAppender.h"
#include "TimeHandler.h"

// Logger logger;
const char *FILENAME = "/system.log";

class LogHandler {
    public:
    void beginRollingFile();
    void readLogFile();
    void clearLogFile();
    void testPrintf();
};

void LogHandler::beginRollingFile() {
  // roughly 100k file
  LOG.getAppender().push_back(new CustomFileAppender(FILENAME, (uint16_t)50000));
  //tracing filter
  LOG.addLevelToAll(Appender::VERBOSE);
  //time formatter
  LOG.addFormatterToAll(
    [](Print &output, Appender::Level level, const char *msg, va_list *args) { 
        output.print(now());  
        output.print(F(" "));
        output.print(Appender::toString(level, true));
        output.print(F(" "));
        size_t length = vsnprintf(NULL, 0, msg, *args) + 1;
        char buffer[length];
        vsnprintf(buffer, length, msg, *args);
        output.print(buffer);
    }
  );
}

void LogHandler::clearLogFile() {
    SPIFFS.remove(FILENAME);
}


void LogHandler::readLogFile() {
  Serial.printf("\nOutput log file: %s\n\n", FILENAME);

  if (SPIFFS.exists(FILENAME)) {
    File file = SPIFFS.open(FILENAME, "r");
    String line;
    while (file.available()) {
      line = file.readStringUntil('\n');
      Serial.println(line);
    }
    file.close();
  }
}

void LogHandler::testPrintf() {

  int d = 392;
  uint16_t u = 7235;
  uint16_t o = 10; // 12 octal
  uint16_t x = 15; // f
  uint16_t X = 15; // F
  float f = 392.65;
  float F = 392.65;
  float e = 3.9265e+2;
  float E = 3.9265e+2;
  float g = 392.65;
  float G = 392.65;
  String s = "sample";
  char c = 'a';

  LOG.verbose("int = [%d]", d);                   // V|int = [392]
  LOG.verbose("uint16_t = [%u]", u);              // V|uint16_t = [7235]
  LOG.verbose("octal = [%o]", o);                 // V|octal = [12]
  LOG.verbose("hexadecimal = [%x]", x);           // V|hexadecimal = [f]
  LOG.verbose("hexadecimal uppercase = [%X]", X); // V|hexadecimal uppercase = [F]
  LOG.verbose("float (f) = [%f]", f);             // V|float (f) = [392.649994]
  LOG.verbose("float (F) = [%F]", F);             // V|float (F) = [3.92649994F+02]
  LOG.verbose("float (e) = [%e]", e);             // V|float (e) = [3.926500e+02]
  LOG.verbose("float (E) = [%E]", E);             // V|float (E) = [3.926500E+02]
  LOG.verbose("float (g) = [%g]", g);             // V|float (g) = [392.65]
  LOG.verbose("float (G) = [%G]", G);             // V|float (G) = [392.65]
  LOG.verbose("String = [%s]", s.c_str());        // V|String = [sample]
  LOG.verbose("char = [%c]", c);                  // V|char = [a]
  LOG.verbose(F("PROGMEM"));                      // V|PROGMEM
  LOG.verbose("%%");                              // V|%
  LOG.verbose("%s(%s:%d)", __func__, __FILE__, __LINE__); // V|testPrintf(../arduino/libraries/log4Esp/examples/BasicLogging/BasicLogging.ino:137)
}

#endif