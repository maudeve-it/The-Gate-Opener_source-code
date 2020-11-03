#ifndef GO_TESTMSG_H
#define GO_TESTMSG_H


#ifdef _TEST
#define _PRINT_TEST(txt) Serial.print(txt)
#define _PRINTLN_TEST(txt) Serial.println(txt)
#else
#define _PRINT_TEST(txt) 
#define _PRINTLN_TEST(txt) 
#endif

#ifdef _TEST_ON_USB
#define _PRINT_TEST_ON_USB(txt) Serial.print(txt)
#define _PRINTLN_TEST_ON_USB(txt) Serial.println(txt)
#else
#define _PRINT_TEST_ON_USB(txt) 
#define _PRINTLN_TEST_ON_USB(txt) 
#endif

#ifdef _TEST_SIM800
#define _PRINT_TEST_SIM800(txt) Serial.print(txt)
#define _PRINTLN_TEST_SIM800(txt) Serial.println(txt)
#else
#define _PRINT_TEST_SIM800(txt) 
#define _PRINTLN_TEST_SIM800(txt) 
#endif

#ifdef _TEST_CSQ
#define _PRINT_TEST_CSQ(txt) Serial.print(txt)
#define _PRINTLN_TEST_CSQ(txt) Serial.println(txt)
#else
#define _PRINT_TEST_CSQ(txt) 
#define _PRINTLN_TEST_CSQ(txt) 
#endif

#ifdef _TEST_LEDS
#define _PRINT_TEST_LEDS(txt) Serial.print(txt)
#define _PRINTLN_TEST_LEDS(txt) Serial.println(txt)
#else
#define _PRINT_TEST_LEDS(txt) 
#define _PRINTLN_TEST_LEDS(txt) 
#endif

#ifdef _TEST_SIM800_SERIAL
#define _PRINT_TEST_SIM800_SERIAL(txt) Serial.print(txt)
#define _PRINTLN_TEST_SIM800_SERIAL(txt) Serial.println(txt)
#else
#define _PRINT_TEST_SIM800_SERIAL(txt) 
#define _PRINTLN_TEST_SIM800_SERIAL(txt) 
#endif

#ifdef _TEST_TIMING
#define _PRINT_TEST_TIMING(txt) Serial.print(txt)
#define _PRINTLN_TEST_TIMING(txt) Serial.println(txt)
#else
#define _PRINT_TEST_TIMING(txt) 
#define _PRINTLN_TEST_TIMING(txt) 
#endif

#ifdef _TEST_SMS_MGMT
#define _PRINT_TEST_SMS_MGMT(txt) Serial.print(txt)
#define _PRINTLN_TEST_SMS_MGMT(txt) Serial.println(txt)
#else
#define _PRINT_TEST_SMS_MGMT(txt) 
#define _PRINTLN_TEST_SMS_MGMT(txt) 
#endif

#ifdef _TEST_PHONEBOOK
#define _PRINT_TEST_PHONEBOOK(txt) Serial.print(txt)
#define _PRINTLN_TEST_PHONEBOOK(txt) Serial.println(txt)
#else
#define _PRINT_TEST_PHONEBOOK(txt) 
#define _PRINTLN_TEST_PHONEBOOK(txt) 
#endif

#ifdef _TEST_SMS_COMMAND
#define _PRINT_TEST_SMS_COMMAND(txt) Serial.print(txt)
#define _PRINTLN_TEST_SMS_COMMAND(txt) Serial.println(txt)
#else
#define _PRINT_TEST_SMS_COMMAND(txt) 
#define _PRINTLN_TEST_SMS_COMMAND(txt) 
#endif

#ifdef _TEST_GATEUSERS
#define _PRINT_TEST_GATEUSERS(txt) Serial.print(txt)
#define _PRINTLN_TEST_GATEUSERS(txt) Serial.println(txt)
#else
#define _PRINT_TEST_GATEUSERS(txt) 
#define _PRINTLN_TEST_GATEUSERS(txt) 
#endif

#ifdef _TEST_HAND_RES
#define _PRINT_TEST_HAND_RES(txt) Serial.print(txt)
#define _PRINTLN_TEST_HAND_RES(txt) Serial.println(txt)
#else
#define _PRINT_TEST_HAND_RES(txt) 
#define _PRINTLN_TEST_HAND_RES(txt) 
#endif

#ifdef _TEST_EEPROM
#define _PRINT_TEST_EEPROM(txt) Serial.print(txt)
#define _PRINTLN_TEST_EEPROM(txt) Serial.println(txt)
#else
#define _PRINT_TEST_EEPROM(txt) 
#define _PRINTLN_TEST_EEPROM(txt) 
#endif

#ifdef _TEST_WDT
#define _PRINT_TEST_WDT(txt) Serial.print(txt)
#define _PRINTLN_TEST_WDT(txt) Serial.println(txt)
#else
#define _PRINT_TEST_WDT(txt) 
#define _PRINTLN_TEST_WDT(txt) 
#endif



#endif
