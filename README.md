# PlayParser

Recreation of the Microsoft Extended Color BASIC "PLAY" command for Arduino.
============================================================================
This is for my SirSound project, a serial sound device for the Tandy/Radio Shack TRS-80 Color Computer. (Or any other retro system with a serial port.) One of the modes SirSound will support is a PLAY command, similar to the one from Extended Color BASIC. This standalone code will be written to make use of the Arduino "tone()" function:

https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/

Perhaps this will make an easy way for Arduino sketches to add music.

Because the sound device will support 3-voice music, the command will be extended to support this. Currently, I am using the model of MSX-BASIC, which is basically:

```
PLAY "{voice 1 notes}","{voice 2 notes}","{voice 3 notes}"
```

I am using the "Extended BASIC Unravelled II" disassembly of the original CoCo PLAY command as reference, and initialy will recreate the same features in the same way:

http://www.colorcomputerarchive.com/coco/Documents/Books/Unravelled%20Series/extended-basic-unravelled.pdf

..assuming I don't get distracted and go do something else instead.

REVISION
========
* 2018-02-20 allenh - Project started.

FILES
=====

* README.md - this file
* PlayParser.ino

CONFIGURATION
=============

TBA

```
/*      Chip Pin        Arduino Pin */
#define WHATEVER        2  //D2
```

RUNNING
=======
 
More to come...
