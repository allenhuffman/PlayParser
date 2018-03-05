/*---------------------------------------------------------------------------*/
/* 
Sub-Etha Software's PLAY Parser
By Allen C. Huffman
www.subethasoftware.com

This is an implementation of the Microsoft BASIC "PLAY" command, based on the
6809 assembly version in the Tandy/Radio Shack TRS-80 Color Computer's
Extended Color BASIC.

2018-02-20 0.00 allenh - Project began.
2018-02-28 0.00 allenh - Initial framework.
2018-03-02 0.00 allenh - More work on PLAY and its options.
2018-03-03 0.00 allenh - Most things seem to work now.
2018-03-04 0.00 allenh - Supports Flash-strings. Tweaking debug output.

TODO:
* DONE: Data needs to be moved to PROGMEM.
* Pause may be slightly off (or the demo song is just wrong).
* DONE: Need a "reset to defaults" command.
* DONE: Add support for PROGMEM strings.
* Set variable (2-digit name, numeric or string).
* Use variables to support Xvar$; and =var; 

NOTE
----
N (optional) followed by a letter from "A" to "G" or a number from 1 to 12.
When using letters, they can be optionally followed by "#" or "+" to make it
as sharp, or "-" to make it flat. When using numbers, they must be separated
by a ";" (semicolon).

      C  C# D  D# E  F  F# G  G# A  A# B  (sharps)
      1  2  3  4  5  6  7  8  9  10 11 12
      C  D- D  E- E  F  G- G  A- A  B- B  (flats)

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

L1 - whole note
L2 - 1/2 half node
L3 - dotted quarter note (L4.)
L4 - 1/4 quarter note
L8 - 1/8 eighth note
L16 - 1/16 note
L32 - 1/32 note
L64 - 1/64 note

TEMPO
-----
"T" followed by a number from 1-255. Default is 2. (Supports modifiers.)

VOLUME
------
"V" followed by a number from 1-31. Default is 15. (Supports modifiers.)

PAUSE
"P" followed by a number from 1-255.

SUBSTRINGS
----------
To be documented, since we will need a special method to load them befoe
we can use them.

Non-Standard Extensions
-----------------------
"Z" to reset back to default settings.

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
*/
/*---------------------------------------------------------------------------*/

#define VERSION "0.00"

#define DEBUG // enable debug output

/*---------------------------------------------------------------------------*/
// PROTOTYPES
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
// DEFINES
/*---------------------------------------------------------------------------*/
#if defined(DEBUG)
#define DEBUG_PRINT(s)    Serial.print(s)
#define DEBUG_PRINTLN(s)  Serial.println(s)
#else
#define DEBUG_PRINT(s)
#define DEBUG_PRINTLN(s)
#endif

// 9C5B - Table of numerical note values for letter notes.
const byte g_NoteJumpTable[7] PROGMEM =
{
  /*   A,  B, C, D, E, F, G */
  /**/10, 12, 1, 3, 5, 6, 8 
};

#define STRINGTYPE_RAM   0
#define STRINGTYPE_FLASH 1

/*---------------------------------------------------------------------------*/
// GLOBALS
/*---------------------------------------------------------------------------*/
// Used for resetting defaults later, too.
#define DEFAULT_OCTAVE 2
#define DEFAULT_VOLUME 15
#define DEFAULT_NOTELN 4
#define DEFAULT_TEMPO  2

byte g_Octave = DEFAULT_OCTAVE; // Octave (1-5, default 2)
byte g_Volume = DEFAULT_VOLUME; // Volume (1-31, default 15)
byte g_NoteLn = DEFAULT_NOTELN; // Note Length (1-255, default 4) - quarter note
byte g_Tempo  = DEFAULT_TEMPO;  // Tempo (1-255, default 2)

/*---------------------------------------------------------------------------*/

/*
 * play()
 */
void play(const __FlashStringHelper *playString)
{
  unsigned int address = (unsigned int)playString;

  DEBUG_PRINT(F("play(F(\""));
  DEBUG_PRINT(playString);
  DEBUG_PRINTLN(F("\"))"));

  playWorker(address, STRINGTYPE_FLASH);
}

void play(const char *playString)
{
  unsigned int address = (unsigned int)playString;

  DEBUG_PRINT(F("play(\""));
  DEBUG_PRINT(playString);
  DEBUG_PRINTLN(F("\")"));

  playWorker(address, STRINGTYPE_RAM);
}

void playWorker(unsigned int commandPtr, byte stringType)
{
  char    commandChar;
  bool    done;
  byte    value;
  byte    note;
  byte    dotVal;

  if (commandPtr == 0) return;

  done = false;
  value = 0; // force ?FC ERROR
  do
  {
    // L9A43
    // L9B98
    // * GET NEXT COMMAND - RETURN VALUE IN ACCA
    commandChar = getNextCommand(&commandPtr, stringType);

    dotVal = 0; // Start out with no dotted value.
    switch( commandChar )
    {
      case '\0':
        value = 1; // no error
        done = true;
        break;

      // 9A4A - ;
      // SUB COMMAND TERMINATED
      case ';':
        // IGNORE SEMICOLONS
        DEBUG_PRINT(F(" ;"));
        break;
        
      // 9A4E - '
      // CHECK FOR APOSTROPHE
      case '\'':
        // IGNORE THEM TOO
        DEBUG_PRINT(F(" '"));
        break;

      // 9A52
      // CHECK FOR AN EXECUTABLE SUBSTRING
      case 'X':
        // X - sub-string (x$; or xx$;)
        DEBUG_PRINT(F(" (X")); // not supported
        // process substring
        // Skip until semicolon or end of string.
        do
        {
          commandChar = getNextCommand(&commandPtr, stringType);
          
          if (commandChar == '\0') break;
  
          DEBUG_PRINT(commandChar);
          
        } while( commandChar != ';' );

        DEBUG_PRINT(F(")")); // not supported
        
        break;

      // CHECK FOR OTHER COMMANDS
      
      // 9A5C
      case 'O':
        // // ADJUST OCTAVE?
        DEBUG_PRINT(F(" O"));
        // O - octave (1-5, default 2)
        //    Modifiers
        value = checkModifier(&commandPtr, stringType, g_Octave);
        if (value >=1 && value <= 5)
        {
          DEBUG_PRINT(value);
          g_Octave = value;
        }
        else
        {
          value = 0; // ?FC ERROR
          done = true;
        }
        break;

      // 9a6D
      case 'V':
        //  V - volume (1-31, default 15)
        DEBUG_PRINT(F(" V"));
        //    Mofifiers
        value = checkModifier(&commandPtr, stringType, g_Volume);
        if (value >=1 && value <= 31)
        {
          DEBUG_PRINT(value);
          g_Volume = value;
        }
        else
        {
          value = 0; // ?FC ERROR
          done = true;
        }
        break;

      // 9a8b
      case 'L':
        //  L - note length
        DEBUG_PRINT(F(" L"));
        //    Modifiers
        value = checkModifier(&commandPtr, stringType, g_NoteLn);
        if (value > 0 )
        {
          DEBUG_PRINT(value);
          g_NoteLn = value;
        }
        else
        {
          value = 0; // ?FC ERROR
          done = true;
          break;
        }
        //    . - dotted note
        dotVal = 0;
        while(1)
        {
          commandChar = getNextCommand(&commandPtr, stringType);
          // Done if there is no more.
          if (commandChar == '\0')
          {
            value = 1; // no error
            done = true;
            break;
          }
          else if (commandChar == '.')
          {
            DEBUG_PRINT(F("."));
            dotVal++;
          }
          else // Not a dot.
          {
            // Not a dot. Put it back.
            commandPtr--;
            break;
          }
        }
        break;
        
      // L9AB2
      case 'T':
        //  T - tempo (1-255, default 2)
        DEBUG_PRINT(F(" T"));
        //    Modifiers
        value = checkModifier(&commandPtr, stringType, g_Tempo);
        if (value > 0)
        {
          DEBUG_PRINT(value);
          g_Tempo = value;
        }
        else
        {
          value = 0; // ?FC ERROR
          done = true;
        }
        break;
        
      // L9AC3
      case 'P':
        //  P - pause (1-255)
        DEBUG_PRINT(F(" P"));

        commandChar = getNextCommand(&commandPtr, stringType);
        // Done if there is no more.
        if (commandChar == '\0')
        {
          value = 0; // ?FC ERROR
          done = true;
          break;
        }

        // since =var; is not supported, we default to note length
        value = checkForVariableOrNumeric(&commandPtr, stringType, commandChar, g_NoteLn);
        if (value > 0)
        {
          DEBUG_PRINT(value);

          unsigned long duration;
          // Create 60hz timing from Tempo and NoteLn (matching CoCo).
          duration = (256/g_NoteLn/g_Tempo);
          
          // Convert to 60/second
          // tm/60 = ms/1000
          // ms=(tm/60)*1000
          // no floating point needed this way
          // (tm*1000)/60
          duration = (duration*1000)/60;

          delay(duration);        
        }
        else
        {
          value = 0; // ?FC ERROR
          done = true;
        }
        break;

      /*-----------------------------------------------------*/
      // Non-standard PLAY extensions:
      case 'Z': // reset
        DEBUG_PRINT(F(" Z [Defaults]"));
        g_Octave = DEFAULT_OCTAVE; // Octave (1-5, default 2)
        g_Volume = DEFAULT_VOLUME; // Volume (1-31, default 15)
        g_NoteLn = DEFAULT_NOTELN; // Note Length (1-255, default 4) - quarter note
        g_Tempo  = DEFAULT_TEMPO;  // Tempo (1-255, default 2)
        break;
      /*-----------------------------------------------------*/

      // L9AEB
      case 'N':
        //  N - note (optional)
        DEBUG_PRINT(F(" N"));
        // Get next command character.
        commandChar = getNextCommand(&commandPtr, stringType);
        
        // Done if there is no more.
        if (commandChar == '\0')
        {
          value = 0; // ?FC ERROR
          done = true;
          break;
        }
        // Drop down to looking for note value.
        // no break

      // L9AF2
      default:
        // (A-G, 1-12)
        //    A-G
        note = 0;
        if (commandChar >= 'A' && commandChar <= 'G')
        {
          //DEBUG_PRINT("A-G ");
          // Get numeric note value of letter note. (0-11)
          note = pgm_read_byte_near(&g_NoteJumpTable[commandChar - 'A']);
          // note is now 1-12

          DEBUG_PRINT(F(" "));
          DEBUG_PRINT(commandChar);
          
          // Check for sharp/flat character.
          commandChar = getNextCommand(&commandPtr, stringType);
          
          // Done if there is no more.
          if (commandChar == '\0')
          {
            // Nothing to see after this one is done.
            value = 1; // no error
            done = true;
          }

          //      # - sharp
          //      + - sharp
          if (commandChar == '#' || commandChar == '+') // Sharp
          {
            DEBUG_PRINT(commandChar);
            note++; // Add one to note number (charp)
          }
          else if (commandChar == '-') // Flat
          {
            DEBUG_PRINT(commandChar);
            note--;
          }
          else
          {
            // Not a #, + or -. Put it back.
            commandPtr--;
          }
        }
        else // NOT A-G, check for 1-12
        {
          // L9BBE - Evaluate decimal expression in command string.
          // Jump to cmp '=' thing in modifier!
          note = checkForVariableOrNumeric(&commandPtr, stringType, commandChar, 0);
          if (note == 0)
          {
            value = 0; // ?FC ERROR
            done = true;
            break;
          }
          DEBUG_PRINT(F(" "));
          DEBUG_PRINT(note);
        }
        
        // L9B22 - Process note value.
        note--; // Adjust note number, BASIC uses 1-2, internally 0-11

        // If not was C (1), and was flat (0), this would make it 255.
        if (note > 11)
        {
          value = 0; // ?FC ERROR
          done = true;
          break;
        }

        /*--------------------------------------------------------*/
        // PROCESS NOTE HERE!
        /*--------------------------------------------------------*/
        
        // Convert tempo and length to milliseconds
        unsigned long duration, dotDuration;

        // Create 60hz timing from Tempo and NoteLn (matching CoCo).
        duration = (256/g_NoteLn/g_Tempo);

        if (dotVal != 0)
        {
          dotDuration = (duration / 2 );

          while(dotVal > 0)
          {
            duration = duration + dotDuration;
            dotVal--;
          }
        }
        
        // Add on dotted notes.

        // Convert to 60/second
        // tm/60 = ms/1000
        // ms=(tm/60)*1000
        // no floating point needed this way
        // (tm*1000)/60
        duration = (duration*1000)/60;
        
        PlayNote(note+(12*(g_Octave-1)), duration);
        
        break;
        
    } // end switch( commandChar );

  } while( done == false );

  if (value == 0)
  {
    DEBUG_PRINTLN();
    DEBUG_PRINT(F("?FC ERROR"));
  }

  DEBUG_PRINTLN();
  //DEBUG_PRINTLN(F("End."));
} // end of play()

/*---------------------------------------------------------------------------*/

/*
 * Get Next Command
 */
// L9B98
char getNextCommand(unsigned int *ptr, byte stringType)
{
  char commandChar;

  //DEBUG_PRINT("getNextCommand(");
  //DEBUG_PRINT((unsigned int)*ptr);
  //DEBUG_PRINT(") - ");
  
  if (ptr == 0)
  {
    // Return nil character, and leave pointer alone.
    commandChar = '\0'; // NIL character
  }
  else
  {
    // Return next character, skipping spaces.
    while(1)
    {
      // Get character at current position.

      if (stringType == STRINGTYPE_RAM)
      {
        char *p = (char*)*ptr;
        commandChar = *p;
      }
      else
      {
        commandChar = pgm_read_byte_near(*ptr);
      }

      if ( commandChar == '\0') break;

      // Increment pointer.
      (*ptr)++;

      if ( commandChar != ' ') break;
    }
  }

  //DEBUG_PRINT(" ... ");
  //DEBUG_PRINTLN((unsigned int)*ptr);

  return commandChar;
}

/*---------------------------------------------------------------------------*/

/*
 * Check Modifiers
 */
/*
  // + - add one
  // - - substract one
  // > - multiply to two
  // < - divited by two
  // = - variable value (not suppoted; just skip)
  // number - use that value (skip if >255)
*/

// L9AC0 - JMP L9BAC
byte checkModifier(unsigned int *ptr, byte stringType, byte value)
{
  char      commandChar;
  
  if (ptr != 0)
  {
    commandChar = getNextCommand(ptr, stringType);

    switch( commandChar )
    {
      case '\0':
        break;
        
      // ADD ONE?
      case '+':
        if (value < 255)
        { 
          value++;
        }
        else
        {
          value = 0; // ?FC ERROR
        }
        break;
        
      // SUBTRACT ONE?
      case '-':
        if (value > 0)
        {
          value--;
        }
        else
        {
          value = 0; // ?FC ERROR
        }
        break;
        
      // MULTIPLY BY TWO?
      case '>':
        if (value <= 127)
        {
          value = value * 2;
        }
        else
        {
          value = 0; // ?FC ERROR
        }
        break;

      // DIVIDE BY TWO?
      case '<':
        if (value > 1)
        {
          value = value / 2;
        }
        else
        {
          value = 0; // ?FC ERROR
        }
        break;

      // Could be = or number, so we call a separate function since we
      // need this in the note routine as well.
      default:
        value = checkForVariableOrNumeric(ptr, stringType, commandChar, value);
        break;
        
    } // end of switch( commandChar )

  } // end of NULL check

  return value;  
} // end of checkModifiers()

/*---------------------------------------------------------------------------*/

byte checkForVariableOrNumeric(unsigned int *ptr, byte stringType, char commandChar, byte value)
{
  //byte      value = 0;
  uint16_t  temp; // MUL A*B = D

  switch( commandChar )
  {
    // SirSound will not have a way to support this.
  
    // CHECK FOR VARIABLE EQUATE
    case '=':
      // "=XX;" - value = whatever XX is set to.
      DEBUG_PRINT(F("(")); // not supported
      DEBUG_PRINT(commandChar);
      // Skip until semicolon or end of string.
      do
      {
        commandChar = getNextCommand(ptr, stringType);
        
        if (commandChar == '\0') break;

        DEBUG_PRINT(commandChar);
        
      } while( commandChar != ';' );
      DEBUG_PRINT(F(")")); // not supported
      // Leave value unchanged, since we don't support it.
      
      //value = 0; // ?FC ERROR since we do not support this yet.
      break;
  
    // Else, check for numeric string.
    default:
      temp = 0;
      value = 0;
      do
      {
        // Stop at a non-numeric character.
        if ( !isdigit(commandChar) ) // not digit
        {
          // Rewind so we end up where we started.
          (*ptr)--;
          break;
        }

        // If here, it must be a digit, 0-9
  
        // Base 10. First time it will be 0 * 10.
        temp = temp * 10;
        
        // Convert ASCII number to value.
        temp = temp + (commandChar - '0');

        if (temp > 255)
        {
/*
          // Skip past numbers.
          do
          {
            commandChar = getNextCommand(ptr);
            if (commandChar == '\0') break;
          } while(isdigit(commandChar) != 0); // digit
  
          // Rewind
          (*ptr)--;
          temp = 0;
*/
          temp = 0; // ?FC ERROR
          break;
        }
        
        // Get another command byte.
        commandChar = getNextCommand(ptr, stringType);
        
      } while( commandChar != '\0' );

      value = temp;
      break;
  } // end of switch( commandChar )

  return value;
}

/*---------------------------------------------------------------------------*/
// End of PlayParser

