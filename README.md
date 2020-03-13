# art-installation-arduino-sensors

This 2017 Arduino sketch (src/art-installation-arduino-sensors.ino) was part of an art installation project involving:

Hardware Sensors (3) -> Arduino (Teensy) -> Mac Pro (2nd Gen) -> QLab (v. 4.x.x) -> Audio/Video Out

The installation was designed to be shut down by museum staff every night and powered up every morning. To aid in this process, the Mac Pro was configured with an Automator script to launch the QLab project file and begin play based on MIDI data output from the Teensy. MIDI was selected as the communication protocol because it was deemed less likely to interfere with any additional normal operation of the Mac Pro, e.g., normal keyboard use for installation/debugging procedures.

The project was designed to read the user's position in relation to the projection surface and change video overlays via pre-programmed video stream fades in QLab, thus making an interactive video exhibit.

See [Mark Gerstein's](https://www.markgerstein.com/studio-work) 2017 Orlando Museum of Art Florida Prize in Contemporary Art piece "Broken Branches" [here](https://vimeo.com/265299350).

![screenshot](https://github.com/thatwhichis/art-installation-arduino-sensors/blob/master/screenshot.png)
