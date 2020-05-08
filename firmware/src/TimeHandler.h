#ifndef TimeHandler_h
#define TimeHandler_h

#include <Arduino.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include "WifiAgent.h"
#include "LogHandler.h"

static const char ntpServerName[] = "cz.pool.ntp.org";
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
const int timeZone = 2;
const unsigned int localPort = 8888;

WiFiUDP Udp;
time_t dummy();

class TimeHandler {
public:
    void start();
    void update();
    time_t getNtpTime();
    void logTime();
    void getTime(char* str);

private:
    byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
    bool shouldUpdate = false;
    bool shouldSet = false;
    void sendNTPpacket(IPAddress &address);
    void updateTimeFromNtp();
};

void TimeHandler::start() {
    Udp.begin(localPort);
    setSyncProvider(dummy);
    setSyncInterval(86400);
    
    // ensure successfull time update on startup
    LOG.verbose(F("Set NTP time"));
    while(now() < 10000) {
        updateTimeFromNtp();
    }
}

time_t dummy() {
    return 0;
}

//update time when can get NTP, or dummy update to trigger update and delay next retry
void TimeHandler::updateTimeFromNtp() {
    time_t retrievedTime = getNtpTime();
    if (retrievedTime != 0) {
        setTime(retrievedTime);
    } else {
        setTime(now());
    }
}

void TimeHandler::update() {
    switch (timeStatus()){
        case timeStatus_t::timeNotSet:
            LOG.verbose(F("Time status: Not Set, update time"));
            updateTimeFromNtp();
            break;
        case timeStatus_t::timeNeedsSync:
            LOG.verbose(F("Time status: Need Sync, update time"));
            updateTimeFromNtp();
            break;
        case timeStatus_t::timeSet:
            break;
        }
}

time_t TimeHandler::getNtpTime() {
    IPAddress ntpServerIP; // NTP server's ip address
    while (Udp.parsePacket() > 0) ; // discard any previously received packets
    LOG.verbose(F("Transmit NTP Request"));
    // get a random server from the pool
    WiFi.hostByName(ntpServerName, ntpServerIP);
    LOG.verbose(F("%s: %s"), ntpServerName, ntpServerIP.toString().c_str());
    sendNTPpacket(ntpServerIP);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
        LOG.verbose(F("Receive NTP Response"));
        Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
        unsigned long secsSince1900;
        // convert four bytes starting at location 40 to a long integer
        secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
        secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
        secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
        secsSince1900 |= (unsigned long)packetBuffer[43];
        return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
}
LOG.error(F("No NTP Response"));
return 0; // return 0 if unable to get the time
}

void TimeHandler::logTime() {
    LOG.verbose(F("Current Time: %02d:%02d:%02d %02d.%02d.%d"), hour(), minute(), second(), day(), month(), year());
}

void TimeHandler::getTime(char* str) {
    sprintf(str, "%02d:%02d:%02d %02d.%02d.%d", hour(), minute(), second(), day(), month(), year());
}

void TimeHandler::sendNTPpacket(IPAddress &address) {
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(address, 123); //NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}

#endif