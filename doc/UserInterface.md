#User Interface/ manual

## Keys
There are Three different Key actions:
* plus (+)
* minus (-)
* set - long press minus key

The concrete function of keys depend on state.

## Serial command line Interface
command line interface accepts following commands:
* b [value] - set brightness to value (possible between 0 and 100)
* t - print time and date to command line
* st H:M\[:S\] \[d.m.yyyy\] \[U\]-set time to given value. Attribute U means UTC. Then time offset and daylight saving time is considered according to settings
* disp row1 row2 row3 - displays arbitrary numbers as binary on display
* effect [mode] - set effect mode of display, mode can be:
* normal
* blink
* flicker
* heartbeat
* h - print help

for background details look at: [Interfaces](Interfaces.md)


