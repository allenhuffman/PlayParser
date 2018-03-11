//#define DEBUG_TONEPLAYER
/*---------------------------------------------------------------------------*/
/*
Arduino tone() command notes and frequenceis, and a simple wrapper function
to play the specified note.

2018-03-03 0.00 allenh - Moving note data to PROGMEM.
2018-03-11 1.00 allenh - Let's just call this 1.0.

TODO:
* ...

TOFIX:
* ...

*/
/*---------------------------------------------------------------------------*/

#define TONEPLAYER_VERSION "1.00"


/*---------------------------------------------------------------------------*/
// DEFINES
/*---------------------------------------------------------------------------*/
/*
 * Connect an 8 ohm speaker between GND and this pin.
 */
#define TONE_PIN 8

#if defined(DEBUG_TONEPLAYER)
#define TONEPLAYER_PRINT(...)   Serial.print(__VA_ARGS__)
#define TONEPLAYER_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define TONEPLAYER_PRINT(...)
#define TONEPLAYER_PRINTLN(...)
#endif

/*
 * Notes and Frequencies
 * From https://www.arduino.cc/en/Tutorial/ToneKeyboard
 */
/* Dummy notes we can't play, to match piano keyboard. */
#define TONE_A0  31
#define TONE_AS0  31

/* Lowest note tone() can play */
#define TONE_B0  31

#define TONE_C1  33
#define TONE_CS1 35
#define TONE_D1  37
#define TONE_DS1 39
#define TONE_E1  41
#define TONE_F1  44
#define TONE_FS1 46
#define TONE_G1  49
#define TONE_GS1 52
#define TONE_A1  55
#define TONE_AS1 58
#define TONE_B1  62

#define TONE_C2  65
#define TONE_CS2 69
#define TONE_D2  73
#define TONE_DS2 78
#define TONE_E2  82
#define TONE_F2  87
#define TONE_FS2 93
#define TONE_G2  98
#define TONE_GS2 104
#define TONE_A2  110
#define TONE_AS2 117
#define TONE_B2  123

#define TONE_C3  131 // Lowest note for PLAY command ("O1 C").
#define TONE_CS3 139
#define TONE_D3  147
#define TONE_DS3 156
#define TONE_E3  165
#define TONE_F3  175
#define TONE_FS3 185
#define TONE_G3  196
#define TONE_GS3 208
#define TONE_A3  220
#define TONE_AS3 233
#define TONE_B3  247

#define TONE_C4  262 // Middle C
#define TONE_CS4 277
#define TONE_D4  294
#define TONE_DS4 311
#define TONE_E4  330
#define TONE_F4  349
#define TONE_FS4 370
#define TONE_G4  392
#define TONE_GS4 415
#define TONE_A4  440
#define TONE_AS4 466
#define TONE_B4  494
#define TONE_C5  523

#define TONE_CS5 554
#define TONE_D5  587
#define TONE_DS5 622
#define TONE_E5  659
#define TONE_F5  698
#define TONE_FS5 740
#define TONE_G5  784
#define TONE_GS5 831
#define TONE_A5  880
#define TONE_AS5 932
#define TONE_B5  988

#define TONE_C6  1047
#define TONE_CS6 1109
#define TONE_D6  1175
#define TONE_DS6 1245
#define TONE_E6  1319
#define TONE_F6  1397
#define TONE_FS6 1480
#define TONE_G6  1568
#define TONE_GS6 1661
#define TONE_A6  1760
#define TONE_AS6 1865
#define TONE_B6  1976

#define TONE_C7  2093
#define TONE_CS7 2217
#define TONE_D7  2349
#define TONE_DS7 2489
#define TONE_E7  2637
#define TONE_F7  2794
#define TONE_FS7 2960
#define TONE_G7  3136
#define TONE_GS7 3322
#define TONE_A7  3520
#define TONE_AS7 3729
#define TONE_B7  3951
#define TONE_C8  4186

#define TONE_CS8 4435
#define TONE_D8  4699
#define TONE_DS8 4978

/*---------------------------------------------------------------------------*/
// GLOBALS
/*---------------------------------------------------------------------------*/

/*
 * This table contains all the notes the Arduino tone() command can play.
 * 
 * The PLAY command is limited to a 5-octave range (octave 3 through 7).
 * The PlayParser will need to know what offset to apply to convert the
 * 60 notes it could play to where they are in the tone() command range.
 * 
 * 0 = C3 (O1)
 * 59 = B7 (O5)
 */
#define TONE_PLAY_OFFSET TONE_C3

static const uint16_t g_ToneTable[] PROGMEM =
{
  /*
   * Dummy notes we can't play, to present normal keyboard range.
   */
  TONE_A0, TONE_AS0,
  /*
   * Lowest note Arduino can produce:
   */
  TONE_B0,
  /*
   * Not supported by PLAY command.
   */
  TONE_C1, TONE_CS1, TONE_D1, TONE_DS1, TONE_E1, TONE_F1,
  TONE_FS1, TONE_G1, TONE_GS1, TONE_A1, TONE_AS1, TONE_B1,
  /*
   * Not supported by PLAY command.
   */
  TONE_C2, TONE_CS2, TONE_D2, TONE_DS2, TONE_E2, TONE_F2,
  TONE_FS2, TONE_G2, TONE_GS2, TONE_A2, TONE_AS2, TONE_B2,
  /*
   * PLAY "O1":
   */
  TONE_C3, TONE_CS3, TONE_D3, TONE_DS3, TONE_E3, TONE_F3,
  TONE_FS3, TONE_G3, TONE_GS3, TONE_A3, TONE_AS3, TONE_B3,
  /*
   * PLAY "O2" - Middle C is C4.
   */
  TONE_C4, TONE_CS4, TONE_D4, TONE_DS4, TONE_E4, TONE_F4,
  TONE_FS4, TONE_G4, TONE_GS4, TONE_A4, TONE_AS4, TONE_B4,
  /*
   * PLAY "O3":
   */
  TONE_C5, TONE_CS5, TONE_D5, TONE_DS5, TONE_E5, TONE_F5,
  TONE_FS5, TONE_G5, TONE_GS5, TONE_A5, TONE_AS5, TONE_B5,
  /*
   * PLAY "O4":
   */
  TONE_C6, TONE_CS6, TONE_D6, TONE_DS6, TONE_E6, TONE_F6,
  TONE_FS6, TONE_G6, TONE_GS6, TONE_A6, TONE_AS6, TONE_B6,
  /*
   * PLAY "O5":
   */
  TONE_C7, TONE_CS7, TONE_D7, TONE_DS7, TONE_E7, TONE_F7,
  TONE_FS7, TONE_G7, TONE_GS7, TONE_A7, TONE_AS7, TONE_B7,
  /*
   * Last few notes Arduino can produce:
   */
  TONE_C8, TONE_CS8, TONE_D8, TONE_DS8
};


/*
 * tonePlayNote()
 * 
 * Play the frequency for the specified note number.
 */
void tonePlayNote(byte note, unsigned long duration)
{
  // Cease any note currently playing, first.
  noTone(TONE_PIN);

  TONEPLAYER_PRINT(F(" tonePlayNote("));
  TONEPLAYER_PRINT(note);
  TONEPLAYER_PRINT(F(", "));
  TONEPLAYER_PRINT(duration);
  TONEPLAYER_PRINT(F(") "));

  if ( note >= (sizeof(g_ToneTable)/sizeof(g_ToneTable[0])) )
  {
    TONEPLAYER_PRINT(F("Invalid note: "));
    TONEPLAYER_PRINTLN(note);

    return;
  }

  if (duration == 0)
  {
    duration = 1;
  }

  tone(TONE_PIN, pgm_read_word_near(&g_ToneTable[note]), duration);
  //delay(duration);
  //noTone(TONE_PIN);
}

/*---------------------------------------------------------------------------*/
// End of TonePlayer

