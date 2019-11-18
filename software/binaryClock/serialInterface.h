#ifndef SerialInterface_h
#define SerialInterface_h

#include "Arduino.h"
#include "binclock.h"
#include "SerialCommands.h"

class SerialInterface {
  public:
    SerialInterface(Stream *serial,BinaryClock *binClock);
    void readSerial();
    void activate(boolean);
    void helloMsg();
    void addCmd(SerialCommand *cmd);
    void printDisplay(uint8_t row1, uint8_t row2, uint8_t row3);
    static void cmd_unrecognized(SerialCommands* sender, const char* cmd);
    static void cmd_bright(SerialCommands* sender);
    static void cmd_setTime(SerialCommands* sender);
    static void cmd_getTime(SerialCommands* sender);
    static void cmd_help(SerialCommands* sender);
    static void cmd_disp(SerialCommands* sender);
    static void cmd_keyPlus(SerialCommands* sender);
    static void cmd_keyMinus(SerialCommands* sender);
    static void cmd_keySet(SerialCommands* sender);
    static void cmd_effect(SerialCommands* sender);
    static void cmd_ambient(SerialCommands* sender);
    static void cmd_set(SerialCommands* sender);
    static void cmd_save(SerialCommands* sender);    
  private:
    SerialCommands serial_commands_;
    static SerialCommand cmds[];
    Stream *serial;
    char serial_command_buffer_[32];
    static BinaryClock *binClock;
};
#endif
