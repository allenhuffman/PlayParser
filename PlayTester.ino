/*---------------------------------------------------------------------------*/
/* 
Sub-Etha Software's PLAY Parser
By Allen C. Huffman
www.subethasoftware.com

Test program for the PlayParser.

2018-02-20 0.00 allenh - Project began.
*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
// FUNCTIONS
/*---------------------------------------------------------------------------*/

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println();
  Serial.println(F("PlayParser Test."));
  showFreeRam();

} // end of setup()

/*---------------------------------------------------------------------------*/

void loop() {
  // put your main code here, to run repeatedly:

  // Test the notes.
  //PlayNote(0, 1000);
  //PlayNote(59, 1000);
  //PlayNote(60, 1000); // this should error

  Serial.println(F("Testing..."));
  play(F("C D E F G"));

  // Input and Play routine.
  char buffer[80];
  while(1)
  {
    Serial.print(F("PLAY string (or 'bye')>"));
    lineInput(buffer, sizeof(buffer));
    //if (strncmp(buffer, "BYE", 3)==0) break;
    if (strncmp_P(buffer, PSTR("BYE"), 3)==0) break;
    play(buffer);
  }

  Serial.println(F("Volume (not implemented):"));
  play(F("Z"));
  play(F("V1 C V5 C V10 C V15 C V20 C V25 C V31 C"));
  delay(2000);
  
  Serial.println(F("Length test:"));
  play(F("Z"));
  play(F("L1 CDEF L2 CDEF L4 CDEF L8 CDEF L16 CDEF"));
  delay(2000);

  Serial.println(F("Octave parsing:"));
  play(F("Z"));
  play(F("O1 C O2 C O3 C O4 C O5 C O4 C O3 C O2 C O1 C"));
  delay(2000);
  
  Serial.println(F("Numeric notes."));
  play(F("Z"));
  play(F("1;2;3;4;5;6;7;8;9;10;11;12"));
  delay(2000);

  Serial.println(F("Normal notes."));
  play(F("Z"));
  play(F("CDEFGAB"));
  delay(2000);

  Serial.println(F("Sharps using #."));
  play(F("Z"));
  play(F("CC#DD#EFF#GG#AA#B"));
  delay(2000);

  Serial.println(F("Sharps using +."));
  play(F("Z"));
  play(F("CC+DD+EFF+GG+AA+B"));
  delay(2000);

  Serial.println(F("Flats."));
  play(F("Z"));
  play(F("CD-DE-EFG-GA-AB-B"));
  delay(2000);  
  
  Serial.println(F("Example from the Extended Color BASIC manual."));
  play(F("Z"));
  play(F("T5;C;E;F;L1;G;P4;L4;C;E;F;L1;G"));
  play(F("P4;L4;C;E;F;L2;G;E;C;E;L1;D"));
  play(F("P8;L4;E;E;D;L2.;C;L4;C;L2;E"));
  play(F("L4;G;G;G;L1;F;L4;E;F"));
  play(F("L2;G;E;L4;C;L8;D;D+;D;E;G;L4;A;L1;O3;C"));
  delay(2000);

  // Relative octave jumps:
  // Peter Gunn: L8 CCDCL>DD+L<CFE
  // Popeye:     L16 EGGGL<FL>EL<GL>
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
  Serial.println(F("Sinfonia (Bach) arranged for CoCo by Tommy & Gail Pollok"));
  play(F("Z"));
  play(F("O2L8CO5L16CO4BO5L8CO4GEGL16CDCO3BO4L8CO3GEGL16CGDGEGFGEGDGCO4CO3BAGO4CO3BAGFEDCGDGEGFGEGDGCO4CO3BAGO4CO3BAGFEDCDEFGABO4CDEFDEGCDEFGABO5CO4ABO5CO4GFGEGFGEGDG"));
  play(F("CO5CO4BAGCACGCFC EGDGEGFGEGDGCO5CO4BAGCACGCFC EO3CO4CO3CBCO4CO3"));
  play(F("CO4DO3CBC O4CO3CO4EO3CO4DO3CO4EO3CO4FO3CO4DO3C O4ECO3CO4CO2BO4CO3CO4CO3DO4CO2BO4C O3CO4CO3EO4CO3DO4CO3EO4CO3F"));
  play(F("O4CO3DO4C O3CDCDEGCDEGCD EFEFGO4CO3EFGO4CO3EF GAGAB-O4EO3GAB-O4EO3GA B-O4GECO3B-GECO2B-AB-G ABABO3C#EO2ABO3C#EO2AB"));
  play(F("O3C#DC#DEAC#DEAC#D EFEFGO4C#O3EFGO4C#O3EF GO4EC#O3AO4GEFDC#EO3AG FAFDO4DO3BO4CO3AG#BED CECO2AO3CEAEO4CO3AO4EO3A"));
  play(F("G#BG#EO4ED#ED#EO3BO4CO3A G#BG#EO4DC#DC#DO3BO4CO3A G#BG#EAEG#EL8AF L4EO4DP8L8O3B L4O4CO3AP8O4L8D# L4EO5DP8L8O4B L4O5CO4AP8L8O3A"));
  play(F("L16 CD#AD#O4CO3D#AD#O4CO3D#AD# CD#AD#O4CO3D#AD#O4CO3D#AD# O2BO3EAEBEAEBEAE DEG#EBEG#EBEG#E AO4AGFEAEDCECO3B AO4AEDCECO3BAO4CO3AG L8F#DF#AL4O4C L16O3BO4GF#EDGDCO3BO4DO3BA GO4GFEDFDCO3BO4DO3BA"));
  play(F("L2O4FL16FEFD ECEGO5CO4GECP16O4GO5CO4B"));
  play(F("L4O5CO4L2G L8GL16FEL2F L8FL16EDL4EL16ECO3GO4C O3FO4CDCO3BGBO4CDO3GO4DE L4.FL8GL4E L16EFEL32FDL32DEDEDEDEDEDEDEDEL4.DL8C L16CO5CO4BAGO5CO4FO5CO4EO5CO4DO5C O4CO5CO4BAGCFCECDC P16O3EGBO4CEGBL4O5C"));
  delay(2000);

  Serial.println(F("Super Mario Brothers"));
  play(F("Z T6"));
  // From http://www.mariopiano.com/Mario-Sheet-Music-Overworld-Main-Theme.pdf
  // Page 1:
  play(F("O3 L4EEP4EP4CEP4GP4P2O-GP4P2"));
  // Begin repeat:
  play(F("O3 CP2O-G P2EP4 P4AP4B P4B-AP4"
    "O2 L3GO+E L4GO-L4O+A P4FG P4EP4C DO-BP2"
    "O3 CO-P2G P2EP4 P4AP4B P4B-AP4"
    "O3 L3O-GO+E L4GO-L4O+A P4FG P4EP4C DO-BP2"));

  // Page 2:
  play(F("O3 O-CO+P4GG- FD#O-CO+E O-FO+G#AO+C O-O-CO+AO+CD"
    "O3 O-CO+P4GG- FD#O-CO+E P4O+CP4C CP4O-O-GO+O+P4"
    "O3 O-CO+P4GG- FD#O-CO+E O-FO+G#AO+C O-O-CO+AO+CD"
    "O3 O-CP4O+E-P4 P4DP2 CP2O-O-G GP4CP4 O+O+"));

  // Page 3 - repeat of 2:
  play(F("O2 CO+P4GG- FD#O-CO+E O-FO+G#AO+C O-O-CO+AO+CD"
    "O2 CO+P4GG- FD#O-CO+E P4O+CP4C CP4O-O-GO+O+P4"
    "O2 CO+P4GG- FD#O-CO+E O-FO+G#AO+C O-O-CO+AO+CD"
    "O2 CP4O+E-P4 P4DP2 CP2O-O-G GP4CP4 O+O+"));

  // Page 4:
  play(F("O3 CCP4C P4CDP4 ECP4O-A GP4O-GP4"
    "O3 CCP4C P4CDE O-GP2C P2O-GP4 O+"
    "O3 CCP4C P4CDP4 ECP4O-A GP4O-GP4"
    "O3 EEP4E P4CEP4 GP4P2 O-GP4P2"));

  // Page 5 - repeat of 1 starting 2nd line:
  play(F("O3 CP2O-G P2EP4 P4AP4B P4B-AP4"
    "O3 L3O-GO+E L4GO-L4O+A P4FG P4EP4C DO-BP2"
    "O3 CP2O-G P2EP4 P4AP4B P4B-AP4"
    "O3 L3O-GO+E L4GO-L4O+A P4FG P4EP4C DO-BP2"));

  // Page 6:
  play(F("O3 ECP4O-G O-GP4O+G#P4 AO+FO-AO+F O-ACO-FP4"
    "O3 L3O-BO+AL4A L3AGL4F ECO-O-GO+A GCO-GP4"
    "O3 ECP4O-G O-GP4O+G#P4 AO+FO-AO+F O-ACO-FP4"
    "O2 BO+FP4F L3FEL4D CO-EGE CP4P2"));

  // Page 7 - repeat of 6:
  play(F("O3 ECP4O-G O-GP4O+G#P4 AO+FO-AO+F O-ACO-FP4"
    "O3 L3O-BO+AL4A L3AGL4F ECO-O-GO+A GCO-GP4"
    "O3 ECP4O-G O-GP4O+G#P4 AO+FO-AO+F O-ACO-FP4"
    "O2 BO+FP4F L3FEL4D CO-EGE CP4P2"));

  // Page 8 - repeat of page 4:
  play(F("O3 CCP4C P4CDP4 ECP4O-A GP4O-GP4"
    "O3 CCP4C P4CDE O-GP2C P2O-GP4 O+"
    "O3 CCP4C P4CDP4 ECP4O-A GP4O-GP4"
    "O3 EEP4E P4CEP4 GP4P2 O-GP4P2"));

  // Page 9 - repeat of page 6:
  play(F("O3 ECP4O-G O-GP4O+G#P4 AO+FO-AO+F O-ACO-FP4"
    "O3 L3O-BO+AL4A L3AGL4F ECO-O-GO+A GCO-GP4"
    "O3 ECP4O-G O-GP4O+G#P4 AO+FO-AO+F O-ACO-FP4"
    "O2 BO+FP4F L3FEL4D CO-EGEC"));
  delay(2000);
}

/*---------------------------------------------------------------------------*/
// Some functions to display free memory, used for testing.

unsigned int freeRam()
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void showFreeRam()
{
  Serial.print(F("Free RAM: "));
  Serial.println(freeRam());
}

/*---------------------------------------------------------------------------*/
// End of PlayTester

