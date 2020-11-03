# The-Gate-Opener_source-code

That's source code for "The Gate Opener".
Download files in a directory called "GateO" in your Arduino directory

---
> The software, schemes and PCB designs in this set of repositories are provided just for 
> illustrative purposes, sharing the personal experience on this project. 
> The software and design artifacts are provided as-is without any mantainence and without
> warranty of any kind. There is no guarantee of fitness for any particular application, 
> and no guarantee of completeness and correctness. 
> By downloading and using these materials the user accepts all risks associated with the
> construction and use of the circuits and devices described in these design files.

---

File description<br>
  <i>GateO.ino</i> - main sketch file: include setup() and main()<br<br>
  Led_Managing.ino - funcions called to control leds flashing and colour<br>
  Serial.ino - funcions managing serial port on USB. Used only on testing (\_TEST_ON_USB macro defined)<br>
  SIM800.ino - functions related to SIM800L control and command<br>
  SMS.ino - functions understanding SMS command<br>
  Phone_book.ino - funcions managing phone book over sim card via SIM800L<br>
  Common.ino - other general purpose functions.<br>
  \_GO_Defs.h - constants declaration via \#define<br>
  \_GO_GlobVar.h - global variables definition/declaration<br>
  \_GO_Lang_EN.h - English language labels for software localization<br>
  \_GO_Lang_IT.h - Italian language labels for software localization<br>
  \_GO_TestMsg.h - macros definition for the debugging levels<br>

