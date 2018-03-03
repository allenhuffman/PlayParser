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

byte g_Octave = 3;  // Octave (1-5, default 3)
byte g_Volume = 15; // Volume (1-31, default 15)
byte g_NoteLn = 5;  // Note Length (default 5)
byte g_Tempo  = 2;  // Tempo (1-255, default 2)
byte g_DotVal = 0;  // Dot Value

/*---------------------------------------------------------------------------*/
// FUNCTIONS
/*---------------------------------------------------------------------------*/

void setup() {
  // put your setup code here, to run once:
  char *string;

  Serial.begin(9600);
  Serial.println(F("PlayParser Test."));

  string = (char*)"ABCDEFG";
  Serial.print((unsigned int)string);
  Serial.print(" - ");
  Serial.println(string);
  play(string);

  delay(2000);

  string = (char*)"CDEFGAB";
  Serial.print((unsigned int)string);
  Serial.print(" - ");
  Serial.println(string);
  play(string);

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

  if (playString == NULL) return;

  // Get pointer to play string.
  commandPtr = (char*)playString;

  //Serial.print("play(");
  //Serial.print((unsigned int)commandPtr);
  //Serial.println(")");

  done = false;
  do
  {
    // L9A43
    // L9B98
    // * GET NEXT COMMAND - RETURN VALUE IN ACCA
    commandChar = getNextCommand(&commandPtr);

    //Serial.print("parsing ");
    //Serial.print(commandChar);
    //Serial.print(": ");

    //Serial.print( (unsigned int)commandPtr);
    //Serial.print(" = ");
    //Serial.println( commandChar );

    //Serial.print("switch(");
    //Serial.print(commandChar);
    //Serial.println(")");
    switch( commandChar )
    {
      case '\0':
        Serial.println("done.");
        done = true;
        break;

      // 9A4A - ;
      // SUB COMMAND TERMINATED
      case ';':
        // IGNORE SEMICOLONS
        break;
        
      // 9A4E - '
      // CHECK FOR APOSTROPHE
      case '\'':
        // IGNORE THEM TOO
        break;

      // 9A52
      // CHECK FOR AN EXECUTABLE SUBSTRING
      case 'X':
        // X - sub-string (x$; or xx$;)
        // process substring
        break;

      // CHECK FOR OTHER COMMANDS
      
      // 9A5C
      case 'O':
        // // ADJUST OCTAVE?
        // O - octave (1-5, default 2)
        //    Modifiers
        value = checkModifier(&commandPtr, g_Octave);
        if (value >=1 && value <= 5)
        {
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
        if (value >=1 && value <= 31)
        {
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
        if (value > 0 )
        {
          g_NoteLn = value;
        }
        else
        {
          value = 0; // ?FC ERROR
          done = true;
        }
        //    . - dotted node
        // DotVal = 0
        // Can we keep adding dotted nodes?
        // DotVal++
        break;
        
      // L9AB2
      case 'T':
        //  T - tempo (1-255, default 2)
        //    Modifiers
        value = checkModifier(&commandPtr, g_Tempo);
        if (value > 0)
        {
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
        // get value
        // pause for that amount?
        break;

      // L9AEB
      case 'N':
        //  N - note (optional)
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
        if (commandChar >= 'A' || commandChar <= 'G')
        {
          //Serial.print("A-G ");
          // Get numeric note value of letter note. (0-11)
          note = g_NoteJumpTable[commandChar - 'A'];
          // note is now 1-12
          
          // Check for sharp/flat character.
          commandChar = getNextCommand(&commandPtr);
          
          // Done if there is no more.
          if (commandChar == '\0')
          {
            // Nothing to see after this one is done.
            done = true;
          }

          //      # - sharp
          //      + - sharp
          if (commandChar == '#' || commandChar == '+') // Sharp
          {
            note++; // Add one to note number (charp)
          }
          else if (commandChar == '-') // Flat
          {
            //      - - flat
            note--;
          }
          else
          {
            //Serial.print("not # + or -: ");
            //Serial.print(commandChar);
            // Put it back.
            //Serial.print("  ptr:");
            //Serial.print((unsigned int)commandPtr);
            *(commandPtr--);
            //Serial.print(" -> ");
            //Serial.println((unsigned int)commandPtr);
          }
        }
        else // not A-G
        {
          Serial.println("checking for number");
          // L9BBE - Evaluate decimal expression in command string.
          // Jump to cmp '=' thing in modifier!
          note = checkForVariableOrNumeric(&commandPtr, commandChar);
          if (note == 0)
          {
            value = 0; // ?FC ERROR
            break;
          }
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
        Serial.println(note);
        PlayNote(note, g_NoteLn);
        
        break;
        
    } // end switch( commandChar );

  } while( done == false );

  if (value == 0)
  {
    Serial.println(F("?FC ERROR"));
  }

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
        if (value > 0)
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
  byte      value;
  uint16_t  temp; // MUL A*B = D

  switch( commandChar )
  {
    // SirSound will not have a way to support this.
  
    // CHECK FOR VARIABLE EQUATE
    case '=':
      // "=XX;" - value = whatever XX is set to.
      // Skip until semicolon or end of string.
      do
      {
        commandChar = getNextCommand(ptr);
        if (commandChar == '\0') break;
        
      } while( commandChar != ';' );
      
      value = 0; // ?FC ERROR since we do not support this year.
      break;
  
    // Else, check for numeric string.
    default:
      temp = 0;
      value = 0;
      do
      {
        // Stop at a non-numeric character.
        if ( isdigit(commandChar) == 0) // not digit
        {
          // Rewind so we end up where we started.
          (*ptr)--;
          break;
        }
  
        // Base 10. First time it will be 0 * 10.
        temp = temp * 10;
        // Convert ASCII number to value.
        temp = temp + (commandChar - '0');
  
        if (temp>255)
        {
          // In BASIC we would ?FC ERROR.
          // Skip past numbers.
          do
          {
            commandChar = getNextCommand(ptr);
            if (commandChar == '\0') break;
          } while(isdigit(commandChar) != 0); // digit
  
          // Rewind
          (*ptr)--;
          temp = 0;
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

