#include "wifi.h"
#include "Arduino.h"
#include "binclockHAL.h"
int WifiESP::power;
bool WifiESP::powerstat;
enum WifiESP::WL_STATUS WifiESP::wifistat;
SerialInterface *WifiESP::interface;

SerialCommand WifiESP::cmds[] = {
  SerialCommand("ip", cmd_ip),
  SerialCommand("started", cmd_started),
  SerialCommand("wifistat",cmd_wifistat),
  SerialCommand("httpresult",cmd_httpresult)
};

void WifiESP::init(SerialInterface *interface) {
  power = 0;
  powerstat = false;
  wifistat = WL_STATUS::WL_IDLE_STATUS;
  WifiESP::interface = interface;
  for(int i = 0; i < sizeof(cmds)/sizeof(cmds[0]); i++) {
    interface->addCmd(&cmds[i]);
  }
}

void WifiESP::power_on(bool blocking) {
	char start_str[] = "started";
	uint8_t timeout = 100;
	uint8_t ptr = 0;
	
	if(power == 0) {
		BinaryClockHAL::wifi_power(true);
		if(blocking) {
			interface->activate(false);
			while(timeout--) {
				delay(10);
				if(Serial.available()) {
					char c = Serial.read();
					if(c == start_str[ptr]) {
						ptr++;
					} else {
						ptr = 0;
					}
					if(ptr == strlen(start_str)) {
						powerstat = true;
						break;
					}
				}
			}
			interface->activate(true);
		}
	}
	power++;
}

void WifiESP::power_soft_off() {
	if(power > 0) {
		power--;
		if(power == 0) {
			power_hard_off();
		}
	}
}

void WifiESP::power_hard_off() {
	power = 0;
	powerstat = false;
	BinaryClockHAL::wifi_power(false);
}

enum WifiESP::POWER_STAT WifiESP::getPowerStat() {
	if(power) {
		if(powerstat) {
			return POWER_STAT::POWER_ON;
		}
		else {
			return POWER_STAT::POWER_STARTING;
		}
	}
	else {
		return POWER_STAT::POWER_OFF;
	}
}

bool WifiESP::ntp_request() {
  if(power > 0 && powerstat) {
    Serial.println("stat");
  }
  if(wifistat == WL_STATUS::WL_CONNECTED) {
    Serial.println("ntp");
    return true;
  }
  if(power == 0) {
    power_on(false);
  }
  
  return false;
}

void WifiESP::config_menu() {
  Serial.println("conf");
}

void WifiESP::cmd_ip(SerialCommands* sender) {
  (void *) sender;
}
void WifiESP::cmd_started(SerialCommands* sender) {
  (void *) sender;
  powerstat = true;
}
void WifiESP::cmd_wifistat(SerialCommands* sender) {
  char* stat_str = sender->Next();
  if (stat_str == NULL)
  {
    sender->GetSerial()->println(F("unknown wifistat"));
    return;
  }
  wifistat = static_cast<enum WL_STATUS>(atoi(stat_str));
}
void WifiESP::update() {
  power_on(true);
  Serial.println(F("update"));
}
void WifiESP::cmd_httpresult(SerialCommands* sender) {
  (void *) sender;
}
