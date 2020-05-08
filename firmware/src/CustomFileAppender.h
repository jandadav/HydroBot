#ifndef CUSTOM_FILE_APPENDER_h
#define CUSTOM_FILE_APPENDER_h

//#define cfadebug

#include <Log4Esp.h>

class CustomFileAppender : public Appender {

public:    
    CustomFileAppender(const char *fileName, uint16_t maxFilePosition = 6000);

    Print &getOutput();

protected:
    void begin(Level level, const char *msg, va_list *args);

    void end(Level level, const char *msg, va_list *args);

private:
    const char *_fileName = NULL;
    File _file;
    uint16_t _lastOffset;
    uint16_t _maxRowLength;
    uint16_t _maxRows;
    uint16_t _maxFilePosition;
    Logger _log = Logger();

    const static int OFFSET_LENGTH = 11; // 10 digits to keep an full integer value and 1 digit for CR
    
    void writeOffset(uint16_t offset);
    uint16_t readOffset();
};

CustomFileAppender::CustomFileAppender(const char *fileName, uint16_t maxFilePosition) {

    _fileName = fileName;
    _maxFilePosition = maxFilePosition;

    setFormatter(Appender::getDefaultFormatter());

    if (SPIFFS.begin()) {
        _log.verbose(F("File system mounted."));
    } else {
        _log.warning(F("Mounting file system failed."));
    }


    //TODO after file is wiped, device needs to be restarted
    //to recreate the file. This could be fixed

    if (SPIFFS.exists(_fileName)) {
        // open existing file 
        _file = SPIFFS.open(_fileName, "r+");
        if (_file) {
            _log.trace(F("Open log file [%s] successful."), _fileName);
            //set position to last offset
            _file.seek(readOffset(), SeekSet);
            _log.trace(F("Log file [%s] position set to [%u]"), _fileName, _file.position());

        } else {
            _log.error(F("Open log file [%s] failed."), _fileName);
        }
    } else {
        // create new file
        _file = SPIFFS.open(_fileName, "w+");
        if (_file) {
            _log.trace(F("Creating new log file [%s] was successful."), _fileName);
            writeOffset(OFFSET_LENGTH);
            _file.println();
        } else {
            _log.error(F("Creating new log file [%s] failed."), _fileName);
        }
    }
}

Print &CustomFileAppender::getOutput() { return _file;}

void CustomFileAppender::begin(Level level, const char *msg, va_list *args) {}

void CustomFileAppender::end(Level level, const char *msg, va_list *args) {
    getOutput().println();
    uint16_t offset = _file.position();
    writeOffset(offset);
    
    //horrid rollover 
    if (offset >= (_maxFilePosition)) {
        offset = OFFSET_LENGTH + 1;
    }
    _file.seek(offset, SeekSet);
}

void CustomFileAppender::writeOffset(uint16_t offset) {

  File file = _file;
  if (file) {
    #ifdef cfadebug
        Serial.println("write offset of: " + String(offset));
    #endif
    file.seek(0, SeekSet);
    char digits[10]; // max offset value consists of 10 digits
    sprintf(digits, "%-10d", offset);
    file.print(digits);
    file.flush();
  }
}

uint16_t CustomFileAppender::readOffset() {

  uint16_t offset = OFFSET_LENGTH + 1; // assume first log row position as fallback solution
  File file = _file;
  if (file) {
    file.seek(0, SeekSet);
    String line = file.readStringUntil('\n');
    offset = line.toInt();
    #ifdef cfadebug 
        Serial.println("read offset of: " + line); 
    #endif
    if (offset == 0) {
      _log.error(F("Log file [%s] doesn't contain a valid offset value."), _fileName);
    }
  }
  #ifdef cfadebug
    Serial.println("Offset returning: " + String(offset));
  #endif
  
  return offset;
}

#endif