#ifndef GO_GLOBVAR_H
#define GO_GLOBVAR_H

#include "_GO_Defs.h"


  String currClock = "";     //stores current time from SIM800 clock (updated once per GSM_CLK_TIMING millis) 
  bool expiringUsers=false;  //this flag shows if any USER having expiring-date is in phonebook. Updated any USER insert/cancellation 
  
  char gsmStatus = 'S';   // GSM line signal level. 'S' is a dummy value: starting up
  /* gsmStatus A = excellent signal
   *           B = good signal
   *           C = discrete signal
   *           D = poor signal
   *           E = no signal / no GSM connection refused /no simcard 
   *           F = failure on device
   */
  char smsStatus = 'Q';   // shows what's running on the phone line
  /* smsStatus Q = quiet, no activity
   *           I = incoming call
   *           R = receiving SMS
   *           S = sending SMS
   */
  char gateStatus = ' ';  // shows what is the command running on the gate
  /* gateStatus 'O' = Open the gate!
   *           ' ' = no activity: do not open it
   */

  char clkStatus = 'N';   // Is the clock set to the current date/time?
  /* clkStatus N = current date/time not set (many reasons: "not yet declared APS and TIMEZONE via SMS" or "unavailable GPRS")
   *           S = clock is updated
   */

  char rstStatus = 'N';   // not RESETting phonebook
  /* rstStatus S = resetting phonebook for 5 mins after Reset button pushing
   *           N = not in Phonebook reset phase
   */


  // managing time checking going thruogh procedures/loops
  enum _step:char { ALERT, CSQ, GATE, S_LED, G_LED, RES_B, LOOP};
  long maxTime[7];
  long avgTime[7];

  long loopTime = 0, oLoopTime = 0, iTime;
  bool dontGetTime = false; // set it true if test/extraordinary actions taken. Do not consider current loop valuable in time checking.

#endif
