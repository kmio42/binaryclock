#include "serialInterface.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
extern ESP8266WebServer server;

static char serial_command_buffer_[128];
 SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");
static WiFiManager wifiManager;
static WiFiUDP udp;
static HTTPClient http;
static const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
static byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets


static void cmd_unrecognized(SerialCommands* sender, const char* cmd);
static void cmd_getip(SerialCommands* sender);
static void cmd_gethttp(SerialCommands* sender);
static void cmd_gethttps(SerialCommands* sender);
static void cmd_settime(SerialCommands* sender);
static void cmd_connection_status(SerialCommands* sender);
static void cmd_autoconfig(SerialCommands* sender);
static void sendNTP_packet(char *server);

SerialCommand cmds[] = {
  SerialCommand("getip", cmd_getip),
  SerialCommand("GET", cmd_gethttp),
  SerialCommand("GETS", cmd_gethttps),
  SerialCommand("ntp", cmd_settime),
  SerialCommand("stat", cmd_connection_status),
  SerialCommand("conf", cmd_autoconfig)
};

void init_cmds() {
  for(int i = 0; i < sizeof(cmds)/sizeof(cmds[0]); i++) {
    serial_commands_.AddCommand(&cmds[i]);
  }
  
  serial_commands_.SetDefaultHandler(cmd_unrecognized);
  //wifiManager.setDebugOutput(true);
  udp.begin(1337);

}

void readcmd() {
    serial_commands_.ReadSerial();
}

static void cmd_getip(SerialCommands* sender) {
  sender->GetSerial()->print("ip ");
  sender->GetSerial()->println(WiFi.localIP());
}

static void cmd_gethttps(SerialCommands* sender) {
   char* param = sender->Next();
   if(param == NULL) return;
   std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
   client->setInsecure();

  if(http.begin(*client,param)) {
    int httpCode = http.GET();
    sender->GetSerial()->print("httpresult ");
    sender->GetSerial()->println(httpCode);
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = http.getString();
      Serial.println(payload);
    }
    http.end();

  }else {
    sender->GetSerial()->println("httpresult -20");
  }
}

static void cmd_gethttp(SerialCommands* sender) {
   char* param = sender->Next();
   if(param == NULL) return;
   WiFiClient client;

  if(http.begin(client,param)) {
    int httpCode = http.GET();
    sender->GetSerial()->print("httpresult ");
    sender->GetSerial()->println(httpCode);
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = http.getString();
      Serial.println(payload);
    }
    http.end();

  }else {
    sender->GetSerial()->println("httpresult -20");
  }
}

static void cmd_settime(SerialCommands* sender) {

  while(udp.parsePacket() > 0); //ignore all previous incomming packages
  sendNTP_packet("pool.ntp.org");
  int timeout = 100;
  int sz = 0;

    do {
      delay(10);
      sz = udp.parsePacket();
      if(--timeout == 0) {
        sender->GetSerial()->println("st timeout");
        return;
      }
    } while(sz < NTP_PACKET_SIZE);
    udp.read(packetBuffer, NTP_PACKET_SIZE);
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
    setTime(epoch);
    
  sender->GetSerial()->print("st ");
  sender->GetSerial()->print(hour());
  sender->GetSerial()->print(":");
  sender->GetSerial()->print(minute());
  sender->GetSerial()->print(":");
  sender->GetSerial()->print(second());
  sender->GetSerial()->print(" ");
  sender->GetSerial()->print(day());
  sender->GetSerial()->print(".");
  sender->GetSerial()->print(month());
  sender->GetSerial()->print(".");
  sender->GetSerial()->print(year());
  sender->GetSerial()->println(" UTC");
}

static void cmd_connection_status(SerialCommands* sender) {
  sender->GetSerial()->print("wifistat ");
  sender->GetSerial()->println(WiFi.status());
}

static void cmd_autoconfig(SerialCommands* sender) {
  server.stop();
  wifiManager.setConfigPortalTimeout(600);
  if(!wifiManager.startConfigPortal("BinaryClock")) {
    sender->GetSerial()->println("wifistat -2");
    return;
  }
  sender->GetSerial()->print("wifistat ");
  sender->GetSerial()->println(WiFi.status());
}
    
static void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
}

static void sendNTP_packet(char *server) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(server, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
