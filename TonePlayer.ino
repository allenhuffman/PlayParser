/*---------------------------------------------------------------------------*/
/*
Arduino tone() command notes and frequenceis, and a simple wrapper function
to play the specified note.
*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
// DEFINES
/*---------------------------------------------------------------------------*/
/*
 * Connect an 8 ohm speaker between GND and this pin.
 */
#define TONE_PIN 8

#if defined(DEBUG)
#define DEBUG_PRINT(s)    Serial.print(s)
#define DEBUG_PRINTLN(s)  Serial.println(s)
#else
#define DEBUG_PRINT(s)
#define DEBUG_PRINTLN(s)
#endif

/*
 * Notes and Frequencies
 * From https://www.arduino.cc/en/Tutorial/ToneKeyboard
 */
#define NOTE_B0  31

#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62

/*  A,  B, C, D, E, F, G */
/* 10, 12, 1, 3, 5, 6, 8 */
#define NOTE_C2  65 //0
#define NOTE_CS2 69 //1
#define NOTE_D2  73 //2
#define NOTE_DS2 78 //3
#define NOTE_E2  82 //4
#define NOTE_F2  87 //5
#define NOTE_FS2 93 //6
#define NOTE_G2  98 //7
#define NOTE_GS2 104 //8
#define NOTE_A2  110 //9
#define NOTE_AS2 117 //10
#define NOTE_B2  123 //11

#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247

#define NOTE_C4  262 // Middle C
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523

#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988

#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976

#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186

#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

/*---------------------------------------------------------------------------*/
// GLOBALS
/*---------------------------------------------------------------------------*/

/*
 * This table contains all the notes the PLAY command could handle.
 * 0 = C3 (O1)
 * 59 = B7 (O5)
 */
static const uint16_t g_ToneTable[] =
{
  /*
   * Lowest note Arduino can produce:
   */
  //NOTE_B0,
  /*
   * Not supported by PLAY command.
   */
  //NOTE_C1, NOTE_CS1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_F1,
  //NOTE_FS1, NOTE_G1, NOTE_GS1, NOTE_A1, NOTE_AS1, NOTE_B1,
  /*
   * Not supported by PLAY command.
   */
  //NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2,
  //NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2,
  /*
   * Octave 1:
   */
  NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3,
  NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3,
  /*
   * Octave 2 - Middle C is C4.
   */
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4,
  NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
  /*
   * Octave 3:
   */
  NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5,
  NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
  /*
   * Octave 4:
   */
  NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6,
  NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
  /*
   * Octave 5:
   */
  NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7,
  NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7,
  /*
   * Last few notes Arduino can produce:
   */
  //NOTE_C8, NOTE_CS8, NOTE_D8, NOTE_DS8
};


/*
 * PlayNode()
 * 
 * Play the frequency for the specified note number.
 */
void PlayNote(byte note, unsigned long duration)
{
  /*
  DEBUG_PRINT(F(" PlayNote("));
  DEBUG_PRINT(note);
  DEBUG_PRINT(F(", "));
  DEBUG_PRINT(duration);
  DEBUG_PRINTLN(F(")"));
  */
  
  if (note >= sizeof(g_ToneTable)/sizeof(g_ToneTable[0]))
  {
    /*
    DEBUG_PRINT(F("Invalid note: "));
    DEBUG_PRINTLN(note);
    */
    return;
  }

  tone(TONE_PIN, g_ToneTable[note], duration);

  delay(duration);
  
  noTone(TONE_PIN);
}


/*
void showit()
{
  Serial.print("sizeof(g_ToneTable) = ");
  Serial.println(sizeof(g_ToneTable)/sizeof(g_ToneTable[0]));
}
*/

/*---------------------------------------------------------------------------*/
// End of TonePlayer

