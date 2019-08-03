
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <ESP8266WebServer.h>

#include "serialInterface.h"

ESP8266WebServer server(80);

void handleRoot() {
  String cmd;
  String message = "CMD-Line Binary-Clock\n";
  if(server.args() > 0) {
    message +="cmd> ";
    cmd = server.argName(0);
    for (uint8_t i = 0; i < server.args(); i++) {
      cmd += " "+ server.arg(i);
    }
    serial_commands_.DetachSerial();
    Serial.println(cmd);
    message +=cmd;
    message +="\r\n";
    String ret;
    delay(100);
    int timeout = 10;
    while(timeout--) {
      delay(10);
      if(Serial.available()) {
        timeout = 3;
        message += Serial.read();
      }
    }
    serial_commands_.AttachSerial(&Serial);
  }
  
  server.send(200, "text/plain", message); 
}

void setup() {
  Serial.begin(9600);
  Serial.println("");
  init_cmds();
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  readcmd();
  server.handleClient();
}
