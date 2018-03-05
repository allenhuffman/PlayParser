# PlayParser

Recreation of the Microsoft Extended Color BASIC "PLAY" music command for Arduino.
==================================================================================
BACKGROUND: This is being written for my SirSound project. SirSound is a sound device for the Tandy/Radio Shack TRS-80 Color Computer (or any other retro system with a serial port). One of the modes SirSound will support is the PLAY command, similar to the one from Extended Color BASIC.

This standalone code is be written to make use of the Arduino "tone()" function:

https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/

Instead of having to map out all the note frequencies manually, this code will let you play music using simple strings that represent the notes and timing information. 

The original BASIC "PLAY" command looked like this:
```
PLAY "C D E F G A B"
```
And this Arduino C implementation would be used like this:
````
play("C D E F G A B");    // play RAM-based string
play(F("C D E F G A B")); // play Flash-based string
````

My code attempts to replicate the functionality of the 6809 assembly langauge version in the Color Computer. I used the "Extended BASIC Unravelled II" disassembly of the original CoCo PLAY command as reference:

http://www.colorcomputerarchive.com/coco/Documents/Books/Unravelled%20Series/extended-basic-unravelled.pdf

REVISION
========
* 2018-02-20 allenh - Project started.
* 2018-03-03 allenh - Mostly working code merged into master.
* 2018-03-04 allenh - Updated to handle flash-based strings.

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
as sharp, or "-" to make it flat. When using numbers, they must be separated
by a ";" (semicolon).
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
middle C. (Supports modifiers.)

LENGTH
------
"L" followed by a number from 1 to 255, with an optional "." after it to
add an additional 1/2 of the specified length. i.e., L4 is a quarter note,
L4. is like L4+L8 (dotted quarter note). Default is 2. (Supports modifiers.)

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
"T" followed by a number from 1-255. Default is 2. (Supports modifiers.)

VOLUME
------
"V" followed by a number from 1-31. Default is 15. (Supports modifiers.)
(Does nothing on the Arduino.)

PAUSE
-----
"P" followed by a number from 1-255.

SUBSTRINGS
----------
NOT IMPLEMENTED YET. To be documented, since we will need a special method to load them befoe we can use them.
 
Non-Standard Extensions
-----------------------
"Z" to reset back to default settings:

* Octave (1-5, default 2)
* Volume (1-31, default 15)
* Note Length (1-255, default 4) - quarter note
* Tempo (1-255, default 2)

MODIFIERS
---------
Many items that accept numbers can also use a modifier instead. The
modifier will apply to whatever the last value was. Modifiers are:

* "+" increase value by 1.
* "-" decreate value by 1.
* ">" double value.
* "<" halve value.

For instance, if the octave is currently 1 (O1), using "O+" will make it
octave 2. If Tempo was currenlty 2 (T2), using "T>" would make it 4. If a
modifier causes the value to go out of allowed range, the command will fail
the same as if the out-of-range value was used.

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

TODO
====
* DONE: Data needs to be moved to PROGMEM.
* Pause may be slightly off (or the demo song is just wrong).
* DONE: Need a "reset to defaults" command.
* DONE: Add support for PROGMEM strings.
* Set variable (2-digit name, numeric or string).
* Use variables to support Xvar$; and =var; 
