void setup() {
  // put your setup code here, to run once:
  char *buffer = "CDE;F'GAB";
  char commandCharacter;
  char *commandPointer;

  Serial.begin(9600);
  Serial.println(F("Begin."));

  commandPointer = &buffer[0];

  while(1)
  {
    // L9A43
    commandCharacter = getNextCommand(&commandPointer);
    if (commandCharacter == '\0') break;

    // 9A4A - ;
    if (commandCharacter == ';') continue;

    // 9A4E - '
    if (commandCharacter == '\'') continue;

    // 9A52
    // X - sub-string (x$; or xx$;)
    if (commandCharacter == 'X') continue;

    // 9A5C
    // O - octave (1-5, default 2)
    //    Modifiers

    // 9a6D
    //  V - volume (1-31, default 15)
    //    Mofifiers

    // 9a8b
    //  L - note length
    //    Modifiers
    //    . - dotted node

    //  T - tempo (1-255, default 2)
    //    Modifiers

    //  P - pause (1-255)

    //  N - note (A-G, 1-12)
    //    A-G
    //      # - sharp
    //      + - sharp
    //      - - flat

    Serial.print(commandCharacter);
  }

  Serial.println(F("End."));
}

void loop() {
  // put your main code here, to run repeatedly:

}

/*
 * Get Next Command
 */
// L9B98
char getNextCommand(char **ptr)
{
  char commandCharacter;
  
  if ((ptr != NULL) && (**ptr != '\0'))
  {
    commandCharacter = **ptr;
    (*ptr)++;
  }
  else
  {
    commandCharacter = '\0'; // NIL character
  }

  return commandCharacter;
}

// Modifiers
/*
  Get command character
  // + - add one
  // - - substract one
  // > - multiply to two
  // < - divited by two

  // Numberic variables do not make much sense to support.
  // = - variable?
  // remember start position...
  // scan to find ';'
  // lookup variable starting at saved position
  // use that value

*/

// End of PlayParser

