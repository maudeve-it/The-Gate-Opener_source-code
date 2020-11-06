// SELECT LANGUAGE
#define LANG_IT
//#define LANG_EN

//  Sw/Hw release
//#define REV_A              // configuration for Arduino Uno and Relay module on Breadboard 
#define REV_B                // configuration for Arduino Nano on definitive PCB
//#define REV_C              // just INTERMEDIATE testing on breadboard (changing while testing)


// Tests levels                 Each level defines specific debugging messages on usb port. Remove "//" to enable the debugging level 
//#define _TEST                 // general debugging level: traces on USB all functions called
//#define _TEST_ON_USB          // while running it checks and execute commands from pc keyboard via USB (SerialRead())
//#define _TEST_SIM800
//#define _TEST_LEDS            // testing leds by forcing led status via USB - NEEDS _TEST_ON_USB 
//#define _TEST_SIM800_SERIAL   // duplicate on USB all bytes received/sent to SIM800
//#define _TEST_TIMING          // check/show time needed by single functions
//#define _TEST_SMS_MGMT        // test how is sms receiving, reading and replying
//#define _TEST_PHONEBOOK
//#define _TEST_CSQ
//#define _TEST_HAND_RES
//#define _TEST_SMS_COMMAND     // test command execution received via sms - NEED _TEST_ON_USB 
//#define _TEST_GATEUSERS    
//#define _TEST_EEPROM
//#define _TEST_WDT


#include <EEPROM.h>
#include <SoftwareSerial.h>
#ifdef _TEST_WDT
  #include <avr/wdt.h>
#endif

#include "_GO_Lang_EN.h"
#include "_GO_Lang_IT.h"
#include "_GO_Defs.h"
#include "_GO_GlobVar.h"
#include "_GO_TestMsg.h"

SoftwareSerial SIM800(SIM800_RX, SIM800_TX); 

void setup() {
String strT, strT2;
  InitPins();
  InitLed('0');

  #ifdef _TEST_WDT
    wdt_disable();                                                //disable watchdog 
  #endif

  SIM800.begin(PH_BAUDRATE); 
  Serial.begin(USB_BAUDRATE);
  while (!Serial) {}
  _PRINTLN_TEST_ON_USB(F(_M_AC002));

//  EEPROM.get( POW_RES_COUNTER, countShutdowns);
//  attachInterrupt(digitalPinToInterrupt(INT_PIN),ISR_powerfail,FALLING);

  gsmStatus = 'S';                                                // go to S starting up
  smsStatus = 'Q'; 
  InitSIM800();
  InitLed('5');
  _PRINTLN_TEST_ON_USB(F(_M_AC004));
  _PRINTLN_TEST_CSQ("gsmStatus A is: "+String(gsmStatus));
  ClockSetup();
  _PRINTLN_TEST_CSQ("gsmStatus B is: "+String(gsmStatus));
  
  _PRINT_TEST_ON_USB(F(_M_AC0A4));
  if (clkStatus=='N')
    _PRINTLN_TEST_ON_USB(F(_M_AC005));
  else
    _PRINTLN_TEST_ON_USB(F(_M_AC006));

  _PRINTLN_TEST_ON_USB("localclock updated");

  _PRINTLN_TEST_CSQ("gsmStatus C is: "+String(gsmStatus));
  LookForExpiringUsers();                                         // check if phonebook have expiring users: for the proper led blinking
  _PRINTLN_TEST_CSQ("gsmStatus D is: "+String(gsmStatus));
  
  _PRINTLN_TEST_ON_USB("Expiring users check done");
  InitLed('0');
  InitLed('5');
  #ifdef _TEST_WDT
    wdt_enable(WDTO_8S);                                          //enable watchdog for 8 secs timeout
  #endif 
  _PRINTLN_TEST_CSQ("gsmStatus E is: "+String(gsmStatus));
  

}


void loop() {
  
  #ifdef _TEST_WDT
    wdt_reset();                                        //reset watchdog
  #endif
  #ifdef _TEST_ON_USB
    SerialRead();                                       // monitors USB serial port and manage commands received
  #endif

  iTime = micros();
  while (CheckSIM800Requests()) {                       //monitors messages received by SIM800 (phone calls, SMS, status messages, ...)
    iTime = micros();
  };
  SetTiming(micros()- iTime, ALERT);                    // SetTiming stores time spent (avg and max) for each loop phase. 


  if ((smsStatus == 'Q') && (!SIM800.available())){     //if there are no pending activities on SIM800 go on checking line and SIM800 functionality
    iTime = micros();
    GsmCsq();                                           //check signal quality, SIM800 and simcard
    SetTiming(micros()- iTime, CSQ);
  }

  iTime = micros();
  GateComm();                                           //monitors gate relay and gate led
  SetTiming(micros()- iTime, GATE);

  iTime = micros();
  SmsLed();                                             //drives calls/sms led
  SetTiming(micros()- iTime, S_LED);

  iTime = micros();
  GsmLed();                                             //drives line status led
  SetTiming(micros()- iTime, G_LED);

  iTime = micros();
  CheckHandReset();                                     //monitors reset button
  SetTiming(micros()- iTime, RES_B);

  if (smsStatus == 'Q')                                 //if there are no pending activities on SIM800
    CleanSimCard();                                     //monitors sim card phonebook cleaning for expired registrations
  
  oLoopTime = loopTime;
  loopTime = micros();
  SetTiming(loopTime - oLoopTime, LOOP);

}
