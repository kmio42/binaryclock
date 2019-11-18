#include "serialInterface.h"
#include "binclockHAL.h"
//#include <TimeLib.h>
#include "config.h"
#include <Time.h>
#include "wifi.h"
BinaryClock *SerialInterface::binClock;
SerialCommand SerialInterface::cmds[] = {
  SerialCommand("b", cmd_bright),
  SerialCommand("t", cmd_getTime),
  SerialCommand("st",cmd_setTime),
  SerialCommand("+", cmd_keyPlus),
  SerialCommand("-", cmd_keyMinus),
  SerialCommand("s", cmd_keySet),
  SerialCommand("h", cmd_help),
  SerialCommand("disp", cmd_disp),
  SerialCommand("effect", cmd_effect),
  SerialCommand("amb", cmd_ambient),
  SerialCommand("set",cmd_set),
  SerialCommand("save",cmd_save)
};

SerialInterface::SerialInterface(Stream *serial, BinaryClock *binClock): 
    serial_commands_(serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " "),serial(serial)
    {
  SerialInterface::binClock = binClock;
  for(uint8_t i = 0; i < sizeof(cmds)/sizeof(cmds[0]); i++) {
    serial_commands_.AddCommand(&cmds[i]);
  }
  
  serial_commands_.SetDefaultHandler(cmd_unrecognized);

}
void SerialInterface::addCmd(SerialCommand *cmd) {
  serial_commands_.AddCommand(cmd);
}

void SerialInterface::readSerial() {
    serial_commands_.ReadSerial();
}
void SerialInterface::activate(boolean active) {
  if(active) {
    serial_commands_.AttachSerial(serial);
  }
  else
    serial_commands_.DetachSerial();
}

void SerialInterface::helloMsg() {
  serial_commands_.GetSerial()->println(F("\nHello, I am your binary clock! \nIt's a pleasure for me to show you the time!"));
  serial_commands_.GetSerial()->println();
  printDisplay(hour(), minute(), second());
  serial_commands_.GetSerial()->println(F("\nPress h to get possible commands\ncmd>"));
}

void SerialInterface::printDisplay(uint8_t row1, uint8_t row2, uint8_t row3) {
    serial_commands_.GetSerial()->print("   ");
  for(int i = 0; i < 5; i++) {
      serial_commands_.GetSerial()->print((row1 & ( 1 << (4-i))?" x ":" _ "));
  }
  serial_commands_.GetSerial()->println();
  for(int i = 0; i < 6; i++) {
      serial_commands_.GetSerial()->print((row2 & ( 1 << (5-i))?" x ":" _ "));
  }
  serial_commands_.GetSerial()->println();
  for(int i = 0; i < 6; i++) {
      serial_commands_.GetSerial()->print((row3 & ( 1 << (5-i))?" x ":" _ "));
  }
  serial_commands_.GetSerial()->println();
  serial_commands_.GetSerial()->println();
  serial_commands_.GetSerial()->println(F("32 16  8  4  2  1"));
  
}
void SerialInterface::cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
  sender->GetSerial()->print(F("Unrecognized command ["));
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println("]\ncmd>");
}

void SerialInterface::cmd_bright(SerialCommands* sender) {
  char* pwm_str = sender->Next();
  if (pwm_str == NULL)
  {
    sender->GetSerial()->println(F("value missing\ncmd>"));
    return;
  }
  int pwm = atoi(pwm_str);
  binClock->setBrightness(pwm);
  sender->GetSerial()->print(F("OK. brightness set to: "));
  sender->GetSerial()->println(pwm);
  sender->GetSerial()->println("cmd>");
}

void SerialInterface::cmd_setTime(SerialCommands* sender) {
  char* param = sender->Next();
  char *endstr;
  char *token;

  BinaryClock::rtcGet();
  
  int hh,mm,ss, dd,mnth,yyyy;
  if(param == NULL) return;
  
  token = strtok_r(param, ":",&endstr);
  if(token == NULL) return;
  hh = atoi(token);
  if(hh < 0 || hh > 23) return;
  token = strtok_r(NULL, ":",&endstr);
  if(token == NULL) return;
  mm = atoi(token);
  if(mm < 0 || mm > 59) return;
  token = strtok_r(NULL, ":",&endstr);
  if(token == NULL) ss = second();
  ss = atoi(token);
  if(ss < 0 || ss > 59) return;
  
  param = sender->Next();
  if(param == NULL) {
    DateTime blobb(now());
    setTime(DateTime(blobb.year(),blobb.month(),blobb.day(),hh,mm,ss).unixtime());
    BinaryClock::rtcSet();
    sender->GetSerial()->println(F("OK. Time set.\ncmd>"));
    return;
  }
  token = strtok_r(param, ".",&endstr);
  if(token == NULL) return;
  dd = atoi(token);
  if(dd < 0 || dd > 31) return;
  token = strtok_r(NULL, ".",&endstr);
  if(token == NULL) return;
  mnth = atoi(token);
  if(mnth < 0 || mnth > 12) return;
  token = strtok_r(NULL, ".",&endstr);
  if(token == NULL) return;
  yyyy = atoi(token);
  if(yyyy < 2000 || yyyy > 2200) return;

  setTime(DateTime(yyyy,mnth,dd,hh,mm,ss).unixtime());
  sender->GetSerial()->println(F("OK. Date+Time set.\ncmd>"));

  param = sender->Next();
  if(param != NULL) {
    if(*param == 'U') {
      hh = ClockConfig::timezone;
      if(BinaryClock::isSummerTime(yyyy,mnth,dd,hh,0) && ClockConfig::summerWinterSwitch) {
        hh++;
      }
      adjustTime(hh*3600);
    }
  }
  BinaryClock::rtcSet();
}

void SerialInterface::cmd_keyPlus(SerialCommands* sender) {
  binClock->keyPress(BinaryClock::SOFTKEY::KEY_PLUS);
}
void SerialInterface::cmd_keyMinus(SerialCommands* sender) {
  binClock->keyPress(BinaryClock::SOFTKEY::KEY_MINUS);
}
void SerialInterface::cmd_keySet(SerialCommands* sender) {
  binClock->keyPress(BinaryClock::SOFTKEY::KEY_SET);
}
void SerialInterface::cmd_getTime(SerialCommands* sender) {
    sender->GetSerial()->print(F("Current Time: "));
    char buf1[30];
    DateTime blobb(now());
    sprintf(buf1, "%02d:%02d:%02d %02d.%02d.%04d",  blobb.hour(),blobb.minute(),blobb.second(), blobb.day(), blobb.month(), blobb.year());
    sender->GetSerial()->println(buf1);
    sender->GetSerial()->println("cmd>");
}
void SerialInterface::cmd_help(SerialCommands* sender) {
    sender->GetSerial()->println(F("possible commands:\nt\t - get time\nst H:M[:S] [d.m.y] [UTC]\t - set time\nb value\t - set brightness\n+\t - virtual push + key\n-\t - virtual press minus key"));
    sender->GetSerial()->println(F("s\t - virtual press set key\ndisp row1 row2 row3\t - display numbers as binary in rows\ncmd>"));
}

void SerialInterface::cmd_disp(SerialCommands* sender) {
  uint8_t row[3];

  for(int i = 0; i < 3; i++) {
    char* param = sender->Next();
    if (param == NULL)
    {
      sender->GetSerial()->println(F("value missing - type h for more infos\ncmd>"));
       return;
    }
    row[i] = atoi(param);
  }
  binClock->manualMode(row,30);
}

void SerialInterface::cmd_effect(SerialCommands* sender) {
  char* param = sender->Next();
  if (param == NULL)
  {
    sender->GetSerial()->println(F("value missing\ncmd>"));
    return;
  }
  if(strcmp(param, "blink") == 0) {
    binClock->setDisplayEffect(BinaryClock::DISPEFFECT::BLINK);
  }
  else if(strcmp(param, "flicker") == 0) {
    binClock->setDisplayEffect(BinaryClock::DISPEFFECT::FLICKER);
  }
  else if(strcmp(param, "heart") == 0) {
    binClock->setDisplayEffect(BinaryClock::DISPEFFECT::HEARTBEAT);
  }
  else {
    binClock->setDisplayEffect(BinaryClock::DISPEFFECT::NORMAL);
  }
}

void SerialInterface::cmd_ambient(SerialCommands* sender) {
  sender->GetSerial()->println(analogRead(A7));
}

void SerialInterface::cmd_set(SerialCommands* sender) {
  char* param = sender->Next();
  if(strcmp(param,"bright") == 0) {
    param = sender->Next();
    if(strcmp(param,"auto") == 0) {
      ClockConfig::ambient_brightness = true;
      binClock->setBrightnessType(BinaryClock::BRIGHTNESSTYPE::AMBIENT);
    }
    else if(strcmp(param,"stat") == 0) {
      ClockConfig::ambient_brightness = false;
       binClock->setBrightnessType(BinaryClock::BRIGHTNESSTYPE::STATIC);
    }
  }
  else if(strcmp(param,"wifi") == 0) {
    param = sender->Next();
    if(strcmp(param,"on") == 0) {
      ClockConfig::permanent_wifi=ClockConfig::WL_ALWAYS_ON;
      WifiESP::power_on(false);
    }
    else if(strcmp(param,"sync") == 0) {
      ClockConfig::permanent_wifi=ClockConfig::WL_SYNC_ON;
    }
    else if(strcmp(param,"off") == 0) {
      ClockConfig::permanent_wifi=ClockConfig::WL_ALWAYS_OFF;
      WifiESP::power_hard_off();
    }
    sender->GetSerial()->println(F("wifi: "));
    sender->GetSerial()->println((ClockConfig::permanent_wifi==ClockConfig::WL_ALWAYS_ON)?"always on":(ClockConfig::permanent_wifi==ClockConfig::WL_SYNC_ON)?"on for sync":"always off");
  }
  else if(strcmp(param,"timezone") == 0) {
    param = sender->Next();
    if(param != NULL) {
      ClockConfig::timezone = atoi(param);
    }
    sender->GetSerial()->println("timezone: ");
    sender->GetSerial()->println(ClockConfig::timezone);
  }
  else if(strcmp(param,"summerwinter") == 0) {
    param = sender->Next();
    if(strcmp(param,"auto")) {
      ClockConfig::summerWinterSwitch = true;
    }
    else if(strcmp(param, "man") == 0) {
      ClockConfig::summerWinterSwitch = false;
    }
    sender->GetSerial()->println(F("daylight saving time adjustment: "));
    sender->GetSerial()->println(ClockConfig::summerWinterSwitch?"on":"off");
  }
}
void SerialInterface::cmd_save(SerialCommands* sender) {
}
