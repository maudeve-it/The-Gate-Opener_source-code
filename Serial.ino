/*
 * Serial (USB) communication procedures.
 * used only while debugging
 */



String GetFromSerial () {
String strT;
char ch="";
  strT=String("");
  if (Serial.available()) {
    while (ch!='\n') 
      if (Serial.available()) {
        ch = Serial.read();
        if ((ch!='\n') and (ch!='\r'))
          strT+=ch;  
      };
   }   
  return strT;
};




void SerialRead(){   //serial on USB
/* 
 *      COMMAND: MEANING 
 * (if _TEST_LEDS)
 *      A to F: set gsmStatus to the same value
 *      G: sets sets gateStatus to 'O' (open the gate) 
 *      Q,I,R,S: sets smsStatus to the same value 
 *      K:flip clkStatus 'S'/'N'
 *      .: resets phone
 * (if _TEST_EEPROM)
 *      T: dump EEPROM
 * (if _TEST_SMS_COMMAND)
 *      write on seriam monitor AT commands to SIM800 and get back results
 * 
 */

   



String scommand,str1,str2,str3; 
char command;              
static String prevcommand;
bool res;
byte k;
 
  command = ' ';
  scommand = GetFromSerial();
//  scommand.toUpperCase();
  if (scommand != "") {
    dontGetTime=true;
    if (scommand.length() == 1) {
      command = scommand.charAt(0);
    }
#ifdef _TEST_SMS_COMMAND    
    else if ((scommand.charAt(0)=='c') || (scommand.charAt(0)=='C') ){
      str1=scommand.substring(1);  // toglie 'C'
      str1.trim();
      str2=str1.substring(str1.indexOf(' ')); //str2 = parametro
      str2.trim();
      str1=str1.substring(0,str1.indexOf(' '));  // strT1 = comando
      str1.trim();
      str1.toUpperCase();
      str3="";
      if (str2.charAt(str2.length()-1)=='*') {   //se la stringa finisce con asterisco, l'ultimo parametro e' il telefono del chiamante
        str3=str2.substring(str1.lastIndexOf(' '));
        str3.trim();
        str2=str2.substring(0,str2.lastIndexOf(' '));
        str2.trim();
      }
      Serial.println(">"+str1+"<>"+str2+"<>"+str3+"<");
      TestSmsCommand(&str1,&str2,&str3);
    } 
#endif
//    else if ((scommand.charAt(0)=='p') || (scommand.charAt(0)=='P')){
///*
//      Serial.println("doShutdown = "+doShutdown);
//      Serial.println("countShutdowns = "+countShutdowns);
//      Serial.println("preShutdowns = "+preShutdowns);
//      Serial.println("postShutdowns = "+postShutdowns);
//*/
//      String clk = "";
//      for (k=0; k=14; k++) 
//        clk = clk + currClock.charAt(k);
//      Serial.println("currClock = "+clk);
//      
//    } else if ((scommand.charAt(0)=='w') || (scommand.charAt(0)=='W')){
//      str1=scommand.substring(1);
//      str1.trim();
//      str1.toUpperCase();
//      Serial.println(">"+str1+"<");
//      if (str1=="ON") {
//        #ifdef _TEST_WDT
//          wdt_enable(WDTO_8S);    //enable watchdog for 8 secs timeout
//          wdt_reset();   //reset watchdog
//        #endif
//      } else if (str1=="OFF")  {
//        #ifdef _TEST_WDT
//          wdt_disable();
//        #endif
//      }
//    } 

    else{
      prevcommand=scommand;
      Serial.println("-->"+scommand);
      SIM800.println(scommand);
      _PRINTLN_TEST_SIM800_SERIAL("SIM800.println() msg: >"+scommand+"< gsmStatus= "+String(gsmStatus));
      do
      {
        ReadSIM800(&scommand);
        if (scommand!="")
          Serial.println("<--"+scommand);
        delay(20);
      } while ((scommand!=F("OK")) && (scommand.indexOf(F("ERROR"))==-1));
    }
#ifdef _TEST_LEDS
    if (command == 'G')
      gateStatus = 'O';
    if (command=='K')
      (clkStatus=='S' ? clkStatus='N' : clkStatus='S');
    else if ((command >= 'A') && (command <= 'H'))
      gsmStatus = command;
    else if ((command >= 'I') && (command <= 'S'))
      smsStatus = command;
    else if (command == '.') {
      ResetPhone();
    }  
#endif
#ifdef _TEST_EEPROM
    if (command == 'T') {
      for (k=0;k<(NEXT_EE_ADDR/5);k++){
        Serial.print(String(k*5)+":  ");
        for (int j=0;j<5;j++){
          int a=EEPROM.read(k*5+j);
          Serial.print(String(a)+"<>"+String(char(a))+"  ");
        }
        Serial.println();
      }
    }
#endif
  }
}
