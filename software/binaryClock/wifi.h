#ifndef WIFIESP_H
#define WIFIESP_H
#include "serialInterface.h"


class WifiESP {
public:
	enum WL_STATUS {
		WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
		WL_IDLE_STATUS      = 0,
		WL_NO_SSID_AVAIL    = 1,
		WL_SCAN_COMPLETED   = 2,
		WL_CONNECTED        = 3,
		WL_CONNECT_FAILED   = 4,
		WL_CONNECTION_LOST  = 5,
		WL_DISCONNECTED     = 6
	};
	enum POWER_STAT {
		POWER_OFF = 0,
		POWER_STARTING = 1,
		POWER_ON = 2
	};
	static void init(SerialInterface*);
	static void power_on(bool);
	static void power_soft_off();
	static void power_hard_off();
	static enum POWER_STAT getPowerStat();
  static void setStarted();
  static bool ntp_request();
  static void config_menu();
  static void update();
  
  static void cmd_ip(SerialCommands* sender);
  static void cmd_started(SerialCommands* sender);
  static void cmd_wifistat(SerialCommands* sender);
  static void cmd_httpresult(SerialCommands* sender);


private:
	static int power;
	static bool powerstat;
 	static enum WL_STATUS wifistat;
  static SerialInterface *interface;
  static SerialCommand WifiESP::cmds[];
	
};
#endif
