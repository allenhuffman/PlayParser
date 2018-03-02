/*-----------------------------------------------------------------------------
 
 Sub-Etha Software's PLAY Parser
 By Allen C. Huffman
 www.subethasoftware.com
 
 This is an implementation of the Microsoft BASIC "PLAY" command, based on the
 6809 assembly version in the Tandy/Radio Shack TRS-80 Color Computer's
 Extended Color BASIC.
 
 2018-02-20 0.00 allenh - Project began.
 2018-02-28 0.00 allenh - Initial framework.
 
 -----------------------------------------------------------------------------*/
#define VERSION "0.00"


/*---------------------------------------------------------------------------*/
// PROTOTYPES
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
// DEFINES
/*---------------------------------------------------------------------------*/

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

  Serial.begin(9600);
  Serial.println(F("PlayParser Test."));

  play("CDE;F'GAB");

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

  if (playString == NULL) return;

  // Get pointer to play string.
  commandPtr = (char*)playString;

  done = false;

  do
  {
    // L9A43
    // L9B98
    // * GET NEXT COMMAND - RETURN VALUE IN ACCA
    commandChar = getNextCommand(&commandPtr);

    switch( commandChar )
    {
      case '\0':
        done = true;
        break;

      // 9A4A - ;
      // SUB COMMAND TERMINATED
      case ':':
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
        g_Octave = checkModifier(&commandPtr, g_Octave);
        
        break;

      // 9a6D
      case 'V':
        //  V - volume (1-31, default 15)
        //    Mofifiers
        break;

      // 9a8b
      case 'L':
        //  L - note length
        //    Modifiers
        //    . - dotted node
        break;
        
      // L9AB2
      case 'T':
        //  T - tempo (1-255, default 2)
        //    Modifiers
        break;
        
      // L9AC3
      case 'P':
        //  P - pause (1-255)
        //  ??? Modifiers ???
        break;

      // L9AEB
      case 'N':
        //  N - note (A-G, 1-12)
        //    A-G
        //      # - sharp
        //      + - sharp
        //      - - flat
        break;

      default:
        break;
    } // end switch( commandChar );

    Serial.print(commandChar);

  } while( done == false );

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

      if ( commandChar != ' ') break;

      // Increment pointer.
      (*ptr)++;
    }
  }

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
  uint16_t  temp; // MUL A*B = D
  
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
            ptr--;
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
            ptr--;
            temp = 0;
            break;
          }
          // Get another command byte.
          commandChar = getNextCommand(ptr);  
        } while( commandChar != '\0' );

        value = temp;
        break;
    
    } // end of switch( commandChar )

  } // end of NULL check

  return value;  
} // end of checkModifiers()

/*---------------------------------------------------------------------------*/
// End of PlayParser

