# SPC2MID
SNES (SPC) music to MIDI converters

This is a series of programs that convert music from SNES SPC files to MIDI (some later programs may convert to tracker format). Different programs support a different set of games - i.e. those using a different sound engine.

Currently, the following programs are available:
 * SNESProbe - Probe Software
 * SNESCapcom - Capcom
 * SNESSculpt - Sculptured Software (Berlioz)

Depending on the game, you may get multiple songs present in audio RAM, in some cases all the songs in the game. Also note that for some games, there will be invalid/unplayable MIDIs resulting from false positives (i.e. pointers that don't actually lead to a song). There are also some additional flags to use, some of which are required to generate proper MIDIs for some games. For example, the "S" flag in SNESCapcom is required to get sound effects, as well as the Dr. Light capsule music in all Mega Man X games, and the "1" flag is required for Chuck Rock with SNESProbe. However, all games' music should be able to be converted, and all known released games using each respective sound engine have been tested to be compatible.

Note: To run the applications, you will need Visual Studio redistributables.

See also: my programs PROB2MID and CAP2MID which also support SNES as well as other consoles, and GB2MID which aims to support all GB/GBC games.

Lists of supported games for each program as well as required parameters for certain games will come soon.
