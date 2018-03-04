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

...assuming I don't get distracted and go do something else instead.

REVISION
========
* 2018-02-20 allenh - Project started.
* 2018-03-03 allenh - Mostly working code merged into master.

FILES
=====

* README.md - this file
* PlayParser.ino - C implementation of the Microsoft BASIC "PLAY" command.
* TonePlayer.ino - wrapper for Arduino tone() function that uses a note table to play notes 0-59.
* LineInput.ino - simple input routine that uppercases input.

CONFIGURATION
=============

Currently, the only configuration is in TonePlayer.ino where you can set which pin is being used for Arduino tone().

```
#define TONE_PIN 8
```

See the Arduino documentation for more details, but basically you hook up an 8 ohm speaker between GND (-) and that pin (+).

RUNNING
=======
 
When you run PlayParser, the default is an interactive mode that will take whatever you type and pass it to the PLAY command.

From the comments, here is a summary of the PLAY format:

 NOTE
 ----
 N (optional) followed by a letter from "A" to "G" or a number from 1 to 12.
 When using letters, they can be optionally followed by "#" or "+" to make it
 as sharp, or "-" to make it flat.
```
        C  C# D  D# E  F  F# G  G# A  A# B  (sharps)
        1  2  3  4  5  6  7  8  9  10 11 12
        C  D- D  E- E  F  G- G  A- A  B- B  (flats)
```
 Due to how the original PLAY command was coded by Microsoft, it also allows
 sharps and flats that would normally not be allowed. For instance, E# is the
 same as F, and F- is the same a E. Since notes are numbered 1-12, the code
 did not allow C- or B#. This quirk is replicated in this implementation.

 OCTAVE
 ------
 "O" followed by a number from 1 to 5. Default is octave 2, which includes
 middle C.

 LENGTH
 ------
 "L" followed by a number from 1 to 255, with an optional "." after it to
 add an additional 1/2 of the specified length. i.e., L4 is a quarter note,
 L4. is like L4+L8 (dotted quarter note). Default is 2.

 * L1 - whole note
 * L2 - 1/2 half node
 * L3 - dotted quarter note (L4.)
 * L4 - 1/4 quarter note
 * L8 - 1/8 eighth note
 * L16 - 1/16 note
 * L32 - 1/32 note
 * L64 - 1/64 note

 TEMPO
 -----
 "T" followed by a number from 1-255. Default is 2.

 VOLUME
 ------
 "V" followed by a number from 1-31. Default is 15. (Not supported on the Arduino.)

 PAUSE
 -----
 "P" followed by a number from 1-255.

 SUBSTRINGS
 ----------
 NOT IMPLEMENTED YET. To be documented, since we will need a special method to load them befoe we can use them.

Sample Music
============
Here is "Oh When the Saints Go Marching In" from the Extended Color BASIC manual:

```
  play("T5;C;E;F;L1;G;P4;L4;C;E;F;L1;G");
  play("P4;L4;C;E;F;L2;G;E;C;E;L1;D");
  play("P8;L4;E;E;D;L2.;C;L4;C;L2;E");
  play("L4;G;G;G;L1;F;L4;E;F");
  play("L2;G;E;L4;C;L8;D;D+;D;E;G;L4;A;L1;O3;C");
```
