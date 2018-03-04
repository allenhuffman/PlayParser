/*-----------------------------------------------------------------------------
 
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

 NOTE
 ----
 N (optional) followed by a letter from "A" to "G" or a number from 1 to 12.
 When using letters, they can be optionally followed by "#" or "+" to make it
 as sharp, or "-" to make it flat.

        C  C# D  D# E  F  F# G  G# A  A# B  (sharps)
        1  2  3  4  5  6  7  8  9  10 11 12
        C  D- D  E- E  F  G- G  A- A  B- B  (flats)

 Due to how the original PLAY command was coded by Microsoft, it also allows
 sharps and flats that would normally not be allowed. For instance, E# is the
 same as F, and F- is the same a E. Since notes are numbered 1-12, the code
 did not allow C- or B#. This quirk is replicated in this implementation.

 OCTAVE
 ======
 "O" followed by a number from 1 to 5. Default is octave 2, which includes
 middle C.

 LENGTH
 ======
 "L" followed by a number from 1 to 255, with an optional "." after it to
 add an additional 1/2 of the specified length. i.e., L4 is a quarter note,
 L4. is like L4+L8 (dotted quarter note). Default is 2.

 L1 - whole note
 L2 - 1/2 half node
 L3 - dotted quarter note (L4.)
 L4 - 1/4 quarter note
 L8 - 1/8 eighth note
 L16 - 1/16 note
 L32 - 1/32 note
 L64 - 1/64 note

 TEMPO
 =====
 "T" followed by a number from 1-255. Default is 2.

 VOLUME
 ======
 "V" followed by a number from 1-31. Default is 15.

 PAUSE
 "P" followed by a number from 1-255.

 SUBSTRINGS
 ==========
 To be documented, since we will need a special method to load
 them befoe we can use them.
 
 -----------------------------------------------------------------------------*/
#define VERSION "0.00"


/*---------------------------------------------------------------------------*/
// PROTOTYPES
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
// DEFINES
/*---------------------------------------------------------------------------*/
// 9C5B - Table of numerical note values for letter notes.
const byte g_NoteJumpTable[7] =
{
  /*   A,  B, C, D, E, F, G */
  /**/10, 12, 1, 3, 5, 6, 8 
};

/*---------------------------------------------------------------------------*/
// GLOBALS
/*---------------------------------------------------------------------------*/

byte g_Octave = 2;  // Octave (1-5, default 2)
byte g_Volume = 15; // Volume (1-31, default 15)
byte g_NoteLn = 4;  // Note Length (1-255, default 4) - quarter note
byte g_Tempo  = 2;  // Tempo (1-255, default 2)
byte g_DotVal = 0;  // Dot Value

/*---------------------------------------------------------------------------*/
// FUNCTIONS
/*---------------------------------------------------------------------------*/

void setup() {
  // put your setup code here, to run once:
  char *string;

  Serial.begin(9600);
  Serial.println();
  Serial.println(F("PlayParser Test."));

  // Test the notes.
  //PlayNote(0, 1000);
  //PlayNote(59, 1000);
  //PlayNote(60, 1000); // this should error

  // Input and Play routine.
  char buffer[80];
  while(1)
  {
    Serial.print(F("PLAY>"));
    lineInput(buffer, sizeof(buffer));
    play(buffer);
  }

  // Example from the Extended Color BASIC manual.
  play("T5;C;E;F;L1;G;P4;L4;C;E;F;L1;G");
  play("P4;L4;C;E;F;L2;G;E;C;E;L1;D");
  play("P8;L4;E;E;D;L2.;C;L4;C;L2;E");
  play("L4;G;G;G;L1;F;L4;E;F");
  play("L2;G;E;L4;C;L8;D;D+;D;E;G;L4;A;L1;O3;C");
  delay(2000);

/*
1000 CLS:PRINT@43,"SINFONIA"
1010 PRINT@73,"BY J.S. BACH"
1020 PRINT@139,"ARRANGED"
1030 PRINT@165,"FOR THE COLOR COMPUTER"
1040 PRINT@206,"BY"
1050 PRINT@233,"TOMMY POLLOCK"
1060 PRINT@269,"AND"
1070 PRINT@298,"GAIL POLLOCK"
1080 PRINT@357,"PRESS ANY KEY TO BEGIN"
*/
  play("O2L8CO5L16CO4BO5L8CO4GEGL16CDCO3BO4L8CO3GEGL16CGDGEGFGEGDGCO4CO3BAGO4CO3BAGFEDCGDGEGFGEGDGCO4CO3BAGO4CO3BAGFEDCDEFGABO4CDEFDEGCDEFGABO5CO4ABO5CO4GFGEGFGEGDG");
  play("CO5CO4BAGCACGCFC EGDGEGFGEGDGCO5CO4BAGCACGCFC EO3CO4CO3CBCO4CO3");
  play("CO4DO3CBC O4CO3CO4EO3CO4DO3CO4EO3CO4FO3CO4DO3C O4ECO3CO4CO2BO4CO3CO4CO3DO4CO2BO4C O3CO4CO3EO4CO3DO4CO3EO4CO3F");
  play("O4CO3DO4C O3CDCDEGCDEGCD EFEFGO4CO3EFGO4CO3EF GAGAB-O4EO3GAB-O4EO3GA B-O4GECO3B-GECO2B-AB-G ABABO3C#EO2ABO3C#EO2AB");
  play("O3C#DC#DEAC#DEAC#D EFEFGO4C#O3EFGO4C#O3EF GO4EC#O3AO4GEFDC#EO3AG FAFDO4DO3BO4CO3AG#BED CECO2AO3CEAEO4CO3AO4EO3A");
  play("G#BG#EO4ED#ED#EO3BO4CO3A G#BG#EO4DC#DC#DO3BO4CO3A G#BG#EAEG#EL8AF L4EO4DP8L8O3B L4O4CO3AP8O4L8D# L4EO5DP8L8O4B L4O5CO4AP8L8O3A");
  play("L16 CD#AD#O4CO3D#AD#O4CO3D#AD# CD#AD#O4CO3D#AD#O4CO3D#AD# O2BO3EAEBEAEBEAE DEG#EBEG#EBEG#E AO4AGFEAEDCECO3B AO4AEDCECO3BAO4CO3AG L8F#DF#AL4O4C L16O3BO4GF#EDGDCO3BO4DO3BA GO4GFEDFDCO3BO4DO3BA");
  play("L2O4FL16FEFD ECEGO5CO4GECP16O4GO5CO4B");
  play("L4O5CO4L2G L8GL16FEL2F L8FL16EDL4EL16ECO3GO4C O3FO4CDCO3BGBO4CDO3GO4DE L4.FL8GL4E L16EFEL32FDL32DEDEDEDEDEDEDEDEL4.DL8C L16CO5CO4BAGO5CO4FO5CO4EO5CO4DO5C O4CO5CO4BAGCFCECDC P16O3EGBO4CEGBL4O5C");
  delay(2000);

  Serial.println(F("Length test:"));
  play("L1 CDC L2 CDC L4 CDC L8 CDC L16 CDC");
  delay(2000);

  Serial.println(F("Octave parsing:"));
  play("O1 C O2 C O3 C O4 C O5 C O2");
  delay(2000);
  
  Serial.println(F("Numeric notes."));
  play("1;2;3;4;5;6;7;8;9;10;11;12");
  delay(2000);

  Serial.println(F("Normal notes."));
  play("CDEFGAB");
  delay(2000);

  Serial.println(F("Sharps using #."));
  play("CC#DD#EFF#GG#AA#B");
  delay(2000);

  Serial.println(F("Sharps using +."));
  play("CC+DD+EFF+GG+AA+B");
  delay(2000);

  Serial.println(F("Flats."));
  play("CD-DE-EFG-GA-AB-B");
  delay(2000);  

} // end of setup()

/*---------------------------------------------------------------------------*/

void loop() {
  // put your main code here, to run repeatedly:

}

/*---------------------------------------------------------------------------*/

/*
 * play()
 */
void play(const char *playString)
{
  char    *commandPtr;
  char    commandChar;
  bool    done;
  byte    value;
  byte    note;
  byte    dotVal;

  Serial.print(F("play(\""));
  Serial.print(playString);
  Serial.println(F("\")"));

  if (playString == NULL) return;

  // Get pointer to play string.
  commandPtr = (char*)playString;

  done = false;
  value = 0; // force ?FC ERROR
  do
  {
    // L9A43
    // L9B98
    // * GET NEXT COMMAND - RETURN VALUE IN ACCA
    commandChar = getNextCommand(&commandPtr);

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
        Serial.print(F(" ; "));
        break;
        
      // 9A4E - '
      // CHECK FOR APOSTROPHE
      case '\'':
        // IGNORE THEM TOO
        Serial.print(F(" ' "));
        break;

      // 9A52
      // CHECK FOR AN EXECUTABLE SUBSTRING
      case 'X':
        // X - sub-string (x$; or xx$;)
        // process substring
        Serial.print(F(" X "));
        /***********************************************************/
        // TODO!
        /***********************************************************/
        break;

      // CHECK FOR OTHER COMMANDS
      
      // 9A5C
      case 'O':
        // // ADJUST OCTAVE?
        // O - octave (1-5, default 2)
        //    Modifiers
        value = checkModifier(&commandPtr, g_Octave);
        Serial.print(F(" O"));
        if (value >=1 && value <= 5)
        {
          Serial.print(value);
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
        //    Mofifiers
        value = checkModifier(&commandPtr, g_Volume);
        Serial.print(F(" V"));
        if (value >=1 && value <= 31)
        {
          Serial.print(value);
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
        //    Modifiers
        value = checkModifier(&commandPtr, g_NoteLn);
        Serial.print(F(" L"));
        if (value > 0 )
        {
          Serial.print(value);
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
          commandChar = getNextCommand(&commandPtr);
          // Done if there is no more.
          if (commandChar == '\0')
          {
            value = 1; // no error
            done = true;
            break;
          }
          else if (commandChar == '.')
          {
            Serial.print(F("."));
            dotVal++;
          }
          else // Not a dot.
          {
            // Not a dot. Put it back.
            *(commandPtr--);
            break;
          }
        }
        break;
        
      // L9AB2
      case 'T':
        //  T - tempo (1-255, default 2)
        //    Modifiers
        value = checkModifier(&commandPtr, g_Tempo);
        Serial.print(F(" T"));
        if (value > 0)
        {
          Serial.print(value);
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
        commandChar = getNextCommand(&commandPtr);
        // Done if there is no more.
        if (commandChar == '\0')
        {
          value = 0; // ?FC ERROR
          done = true;
          break;
        }
        
        value = checkForVariableOrNumeric(&commandPtr, commandChar);
        Serial.print(F(" P"));
        if (value > 0)
        {
          Serial.print(value);

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

      // L9AEB
      case 'N':
        //  N - note (optional)
        Serial.print(F(" N"));
        // Get next command character.
        commandChar = getNextCommand(&commandPtr);
        
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
          //Serial.print("A-G ");
          // Get numeric note value of letter note. (0-11)
          note = g_NoteJumpTable[commandChar - 'A'];
          // note is now 1-12

          Serial.print(F(" "));
          Serial.print(note);
          
          // Check for sharp/flat character.
          commandChar = getNextCommand(&commandPtr);
          
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
            Serial.print(F("#"));
            note++; // Add one to note number (charp)
          }
          else if (commandChar == '-') // Flat
          {
            Serial.print(F("-"));
            note--;
          }
          else
          {
            *(commandPtr--);
          }
        }
        else // NOT A-G, check for 1-12
        {
          // L9BBE - Evaluate decimal expression in command string.
          // Jump to cmp '=' thing in modifier!
          note = checkForVariableOrNumeric(&commandPtr, commandChar);
          if (note == 0)
          {
            value = 0; // ?FC ERROR
            done = true;
            break;
          }
          //Serial.print(F(" n"));
          Serial.print(note);
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
    Serial.println();
    Serial.print(F("?FC ERROR"));
  }

  Serial.println();
  Serial.println(F("End."));
} // end of play()

/*---------------------------------------------------------------------------*/

/*
 * Get Next Command
 */
// L9B98
char getNextCommand(char **ptr)
{
  char commandChar;

  //Serial.print("getNextCommand(");
  //Serial.print((unsigned int)*ptr);
  //Serial.print(") - ");
  
  if (ptr == NULL)
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
      commandChar = *(*ptr);

      if ( commandChar == '\0') break;

      // Increment pointer.
      (*ptr)++;

      if ( commandChar != ' ') break;
    }
  }

  //Serial.print(" ... ");
  //Serial.println((unsigned int)*ptr);

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
byte checkModifier(char **ptr, byte value)
{
  char      commandChar;
  
  if ((ptr != NULL) && (*(*ptr) != '\0'))
  {
    commandChar = getNextCommand(ptr);

    switch( commandChar )
    {
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
        value = checkForVariableOrNumeric(ptr, commandChar);
        break;
        
    } // end of switch( commandChar )

  } // end of NULL check

  return value;  
} // end of checkModifiers()

/*---------------------------------------------------------------------------*/

byte checkForVariableOrNumeric(char **ptr, char commandChar)
{
  byte      value = 0;
  uint16_t  temp; // MUL A*B = D

  switch( commandChar )
  {
    // SirSound will not have a way to support this.
  
    // CHECK FOR VARIABLE EQUATE
    case '=':
      // "=XX;" - value = whatever XX is set to.
      // Skip until semicolon or end of string.
      Serial.print(F("(skip =xx;)"));
      do
      {
        commandChar = getNextCommand(ptr);
        
        if (commandChar == '\0') break;
        
      } while( commandChar != ';' );
      
      value = 0; // ?FC ERROR since we do not support this yet.
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
        commandChar = getNextCommand(ptr);
        
      } while( commandChar != '\0' );

      value = temp;
      break;
  } // end of switch( commandChar )

  return value;
}


/*---------------------------------------------------------------------------*/
// End of PlayParser

