# The Gate Opener: source code

That's source code for "The Gate Opener".
Download files in a directory called "GateO" in your Arduino-IDE projects directory

---
> The software, schemes and PCB designs in this set of repositories are provided just for 
> illustrative purposes, sharing the personal experience on this project. 
> The software and design artifacts are provided as-is without any mantainence and without
> warranty of any kind. There is no guarantee of fitness for any particular application, 
> and no guarantee of completeness and correctness. 
> By downloading and using these materials the user accepts all risks associated with the
> construction and use of the circuits and devices described in these design files.

---

Files description
  * *GateO.ino* - main sketch file: include setup() and main()<br>
  * *Led_Managing.ino* - funcions called to control leds flashing and colour<br>
  * *Serial.ino* - funcions managing communication to the serial monitor. Used only on testing (\_TEST_ON_USB macro defined)<br>
  * *SIM800.ino* - functions related to SIM800L control and command<br>
  * *SMS.ino* - functions understanding SMS command<br>
  * *Phone_book.ino* - functions managing phone book over sim card via SIM800L<br>
  * *Common.ino* - other general purpose functions.<br>
  * *\_GO_Defs.h* - constants declaration via \#define<br>
  * *\_GO_GlobVar.h* - global variables definition/declaration<br>
  * *\_GO_Lang_EN.h* - English language labels for software localization<br>
  * *\_GO_Lang_IT.h* - Italian language labels for software localization<br>
  * *\_GO_TestMsg.h* - macros definition for the debugging levels<br>

---

<br>
<br>

# The Gate Opener: sorgente

Questo e' il codice sorgente per "The Gate Opener".
Scarica i file in una cartella chiamata "GateO" nella cartella dei progetti di Arduino-IDE

---
> Il software e gli schemi di progetto come i layout PCB in questa serie di repository 
> sono messe a disposizione con puro intento illustrativo e di condivisione dell'esperienza fatta
> Il software and gli elementi del progetto sono messi a disposizione "allo stato in cui sono"
> senza impegno di manutenzione e senza garanzie di alcun tipo. Piu' esplicitamente, non c'e' garanzia di 
> adeguatezza a soddisfare specifiche esigenze, ne di completezza o correttezza di alcuna parte.
> Scaricando ed utilizzando questo materiale l'utilizzatore accetta il rischio associato alla
> realizzazione ed all'utilizzo del circuito e dei componenti descritti in questi file.

---

Descrizione dei file
  * *GateO.ino* - sketch principale: include setup() e main()<br>
  * *Led_Managing.ino* - funzioni invocate per controllare lo stato dei led: accensione e colore<br>
  * *Serial.ino* - funzioni per gestire la comunicazione verso il serial monitor. Usato solo in fase di test (\_TEST_ON_USB macro definita)<br>
  * *SIM800.ino* - funzioni relative alla gestione di SIM800L<br>
  * *SMS.ino* - funzioni per l'interpretazione dei comandi SMS<br>
  * *Phone_book.ino* - funzioni per gestire la ribrica su sim card attraverso SIM800L<br>
  * *Common.ino* - altre funzioni di uso vario.<br>
  * *\_GO_Defs.h* - dichiarazione di coastanti attraverso \#define<br>
  * *\_GO_GlobVar.h* - definizione/dichiarazione di variabili globali<br>
  * *\_GO_Lang_EN.h* - etichette messaggi in lingua inglese: per la localizzazione del software<br>
  * *\_GO_Lang_IT.h* - etichette messaggi in lingua italiana: per la localizzazione del software<br>
  * *\_GO_TestMsg.h* - definizione delle macro per i diversi livelli di debug<br>

