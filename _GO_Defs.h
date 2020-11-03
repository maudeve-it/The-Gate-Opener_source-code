#ifndef GO_DEFS_H
#define GO_DEFS_H


// PIN SETTINGS

#ifdef REV_A                // configuration used for Arduino Uno on Breadboard
  #define SIM800_RX 2       //softwareserialport to SIM800
  #define SIM800_TX 3       //softwareserialport to SIM800
  #define GSM_R_LED_PIN 4   //if HIGH led on
  #define GSM_G_LED_PIN 5   //if HIGH led on
  #define SMS_R_LED_PIN 6   //if HIGH led on
  #define SMS_G_LED_PIN 7   //if HIGH led on
  #define SMS_B_LED_PIN 8   //if HIGH led on
  #define GSM_RESET_PIN 9   //if HIGH reset SIM800 
  #define GATE_PIN 10       // if ACTIVE_GATE opening gate and led on
  #define GATE_LED_PIN 11   // if ACTIVE_GATE gate led on and gate keep closed
  #define RESET_PIN 12      // goes LOW pushing RESET button
  #define ACTIVE_GATE LOW
  #define INT_PIN 13         // pin used by power fail interrupt
#endif
#ifdef REV_B                // configuration for Arduino Nano on definitive PCB
  #define INT_PIN 2         // pin used by power fail interrupt
  #define SIM800_RX 4       //softwareserialport to SIM800
  #define SIM800_TX 5       //softwareserialport to SIM800
  #define GSM_R_LED_PIN 6   //if HIGH led on
  #define GSM_G_LED_PIN 7   //if HIGH led on
  #define SMS_R_LED_PIN 8   //if HIGH led on
  #define SMS_G_LED_PIN 9   //if HIGH led on
  #define SMS_B_LED_PIN 10  //if HIGH led on
  #define GSM_RESET_PIN 3   //if HIGH reset SIM800  
  #define RESET_PIN 11      // INPUT/OUTPUT pin: goes LOW pushing RESET button | write LOW on it to turn on Reset Led
  #define GATE_PIN 12       // if ACTIVE_GATE opening gate and led on
  #define GATE_LED_PIN 13   // if ACTIVE_GATE gate led on and gate keep closed
  #define ACTIVE_GATE HIGH
#endif
#ifdef REV_C                // just INTERMEDIATE testing on breadboard (changing while testing)
  #define INT_PIN 2         // pin used by power fail interrupt
  #define SIM800_RX 4       //softwareserialport to SIM800
  #define SIM800_TX 5       //softwareserialport to SIM800
  #define GSM_R_LED_PIN 6   //if HIGH led on
  #define GSM_G_LED_PIN 7   //if HIGH led on
  #define SMS_R_LED_PIN 8   //if HIGH led on
  #define SMS_G_LED_PIN 9   //if HIGH led on
  #define SMS_B_LED_PIN 10  //if HIGH led on
  #define GSM_RESET_PIN 3   //if HIGH reset SIM800  
  #define RESET_PIN 11      // INPUT/OUTPUT pin: goes LOW pushing RESET button | write LOW on it to turn on Reset Led
  #define GATE_PIN 12       // if ACTIVE_GATE opening gate and led on
  #define GATE_LED_PIN 13   // if ACTIVE_GATE gate led on and gate keep closed
  #define ACTIVE_GATE LOW
#endif

// GATE parameters
#define GATE_TIMING 5000                      // pulse duration for gate relay

// LED parameters
#define LED_FLASH_ON 150                      // GSM led blinking time
#define LED_FLASH_OFF 830                     // GSM led blinking time
#define CLK_EXP_ON 150                        // CLOCK (SMS) led blinking time when NTP is not available and there are USERS expiring (NTP mandatory)
#define CLK_EXP_OFF 340                       // CLOCK (SMS) led blinking time when NTP is not available and there are USERS expiring (NTP mandatory)
#define CLK_NEXP_ON 50                        // CLOCK (SMS) led blinking time when NTP is not available and there are NOT USERS expiring (NTP not required) 
#define CLK_NEXP_OFF 2900                     // CLOCK (SMS) led blinking time when NTP is not available and there are NOT USERS expiring (NTP not required) 
#define MAX_CLK_STUP 5                        //max number of retries NTP server

#define LED_LONG_DELAY 60000                  // in case Status don't change, update leds this time
#define SMS_LED_TIMING 3000                   // SMS LED single blink duration 
#define SMS_LED_LONG_TIMING 60000             // Sending SMS can require a long time to the GSM network ending operation
#define SETUP_LED_DELAY 150                   // delay in LED checkup on setup
#define SIM800_DELAY_BEFORE_RESET 20000       // as per parameter name
#define RST_LED_F_BLINK 200                   // used on phonebook hand reset
#define RST_LED_S_BLINK 1800                  // used on phonebook hand reset
#define RST_BUT_DELAY 5000                    // used on phonebook hand reset
#define RST_SETUP_TIME 60000                  // time for phonebook setup (reset button pushed)
//#define RST_SETUP_TIME 300000    METTERE QUESTO DOPO I TEST             // time for phonebook setup (reset button pushed)
#define MAX_PHONENO_IN_LIST 5                 // sms lists this qty of phonenumbers
#define CSQ_NUM_PHA 3                         // total number of phases in GsmCsq


//PHONE parameters
//#define PH_BAUDRATE 9600
//#define PH_BAUDRATE 57600
#define PH_BAUDRATE 4800            //SIM800 serial speed
#define USB_BAUDRATE 115200         //USB serial speed
#define STUP_CHECKS 10              //counter for first SIM800 checking loop  (simple AT sending)
#define GSM_SETUP_DELAY 150
#define GSM_QA_DELAY 300            //waittime for normal communication Arduino<->Sim800
#define GSM_SLOW_QA_DELAY 2000      //waittime for slow commands communication Arduino<->Sim800
#define GSM_LET_CALL_RINGING 1000   //courtesy ringing to let caller understanding call is going fine
#define GSM_CLOCK_SETUP_DELAY 3000
#define GSM_WAKEUP_DELAY 5000
#define GSM_CLA_TIMING 5000         // Check GSM line status every...
//#define GSM_CLA_LONG_TIMING 60000   // in case of a slow command like sending SMS, delay GSM checks for...
#define GSM_CLK_TIMING 60000        // update local clock variable (currClock) every...     //  set this one when while testing TEST finished or saving status saving on powerdown (via interrupt)
//#define GSM_CLK_TIMING 3600000      // update local clock variable (currClock) every...     //  set this one when TEST finished or not managing status saving on powerdown
#define CSC_TIMING    120000            // cleaning SIM phonebook    //  set this one while TESTing                  
//#define CSC_TIMING 3600000          // cleaning SIM phonebook    //  set this one when TEST finished              

//EEPROM (memory address and defaults)
#define EE_APN_ADDR 0                     // APN EEPROM address
#define EE_TZ_ADDR 21                     // TIMEZONE EEPROM address
#define EE_NTP_ADDR 24                    // NTP EEPROM address
#define EE_DEF_NTP_SERV "time.inrim.it"   // NTP server name
#define WDOG_RES_COUNTER 45               // Watchdog resets counter
#define WDOG_RES_TIME 47                  // time of last Watchdog reset  
#define POW_RES_COUNTER 64                // Powerdown resets counter
#define POW_RES_TIME 66                   // time of last Powerdown reset
#define MAN_RES_COUNTER 83                // Manual resets counter
#define MAN_RES_TIME 85                   // time of last manual reset
#define NEXT_EE_ADDR 102                  // first free EEPROM location
#define SMS_PARAM_LEN 20                  //length of EEPROM parameters

#endif
