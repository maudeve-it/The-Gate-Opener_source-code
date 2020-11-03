//all the functions to interact with SIM800


bool ReadSIM800(String *strR) {
  return (ReadSIM800(strR, false));
}

bool ReadSIM800(String *strR, bool is_slow) {
  //read a string from SIM800 updating string *strR. Set *strR="" if nothing returned by SIM800
  //Return "false" if no_string received OR string over 255 char OR not ending with NL
  _PRINTLN_TEST(__FUNCTION__);
  unsigned long waitForAnsw, pause;
  char ch;
  byte counter = 1; //after 255 readings without a '\n' counter will be back to 0 and the function will return "false"

  *strR = "";
  if (is_slow)
    pause = GSM_SLOW_QA_DELAY;
  else
    pause = GSM_QA_DELAY;
  do {  
    if (SIM800.available()) {
      ch = SIM800.read();
      if ((ch != '\n') && (ch != '\r') && (ch != char(45)))
        *strR += ch;
      waitForAnsw = millis() + pause;
    };  
  } while ((ch != '\n') && (millis() < waitForAnsw) && (*strR != F("> ")));  // "> ": the only string I know not ending with a \n
  if ((ch != '\n') && (*strR != "") && (*strR != F("> ")) && (ch != char(45)))
    gsmStatus = 'F';
  _PRINTLN_TEST_SIM800_SERIAL("readSIM800 read: >"+*strR+"< gsmStatus= "+String(gsmStatus));
  return ((ch == '\n') || (*strR == F("> ")));
}




bool SIM800print(const String mess) {
  String txt1, txt2;
  return (SIM800print(mess, &txt1, &txt2, false)); //default: "false" means fast AT command
}

bool SIM800print(const String mess, String *answ) {
  String txt = "";
  return (SIM800print(mess, answ, &txt, false)); //default: "false" means fast AT command
}

bool SIM800print(const String mess, String *answ, String *answ2) {
  return (SIM800print(mess, answ, answ2, false)); //default: fast AT command
}

bool SIM800print(const String mess, String *answ, String *answ2, bool is_slow) {
/*
 * SEND A STRING (COMMAND) TO SIM800
 * return true if SIM800 reply "OK", false if "ERROR"
 * "*answ" and "*answ2" (optional) return other messages returned by SIM800 on a command ("OK" and "ERROR" NOT RETURNED)
 * set "is_slow" (optional) as true if "mess" command is SLOW waiting results for a long time (GSM_SLOW_QA_DELAY or GSM_QA_DELAY)
 * it sets gsmStatus global variable accordingly
 * 
 */
  String txt;
  unsigned long startmill;

  _PRINTLN_TEST(__FUNCTION__);

  *answ = "";
  *answ2 = "";
  txt = "";
  SIM800.println(mess);
  _PRINTLN_TEST_SIM800_SERIAL("SIM800print msg: >"+mess+"< gsmStatus= "+String(gsmStatus));
  startmill=millis();
  while ((!SIM800.available()) and ((millis()-startmill)<(is_slow ? GSM_SLOW_QA_DELAY : GSM_QA_DELAY))) {};
  while (ReadSIM800(&txt, is_slow)) {
    if ((txt == F("> ")) || (txt == F("OK")) || (txt.indexOf(F("ERROR")) != -1))
      break;
    if (txt != "") {
      *answ2 = *answ;
      *answ = txt;
      txt = "";
    };
  }
  _PRINTLN_TEST_SIM800_SERIAL("SIM800print exit with: >"+txt+"< gsmStatus= "+String(gsmStatus));
  if ((txt == F("OK")) || (txt == F("> ")))
    return true;
  return false;
}




void UpdateLocalClock() {
static unsigned long initTime = 0;
String currTime;

  if (smsStatus=='S')
    return;

  if ((millis()-initTime < GSM_CLK_TIMING) || (clkStatus == 'N'))
    return;
  _PRINTLN_TEST("UpdateLocalCLock");
  
  initTime=millis();
  SIM800print(F("AT+CCLK?"), &currTime);
  initTime=millis();
  if (currTime.substring(8, 10) == F("01")) //that means now is 2001??? clock wasn't set!
    return;
  currClock=currTime.substring(8, 22); 
  _PRINTLN_TEST_CSQ("Clock time (CCLK):"+currClock);

}




void GsmCsq() {                
//  GsmCsq(false); 
//}


//void GsmCsq(bool delaychecks) {    //if delaychecks=true skip checks and just reset timer for the next check                  
/*  VERIFY PHONE STATUS
 *   GSM_CLA_TIMING defines time interval to check
 *   Each time function is called makes 1 check, defined by "static byte phase"
 *   phase = 0: AT+CPBW (presence a working sim card) + AT+CPIN (sim card is not blocked by PIN or PUK)
 *   phase = 1: AT+CREG (SIM800 is registred in the network)
 *   phase = 2: AT+CSQ  (check signal quality)
 *   check results are reported on "gsmStatus" global variable
 *
 *   Calling GsmCsq without parameters or with a "true" patrameter, it will per form checks as above
 *   Calling GsmCsq with a "false" parameter will skip checks and just set timeout for the next time
 *           (used requesting a slow phone operation. It isn't able to perform checks until operation is finished)
 */

static unsigned long initTime = 0;
static int phase = 0;
String strT, csq;
long v;

//  if (delaychecks){   //when SIM800 is performing a "slow" command (e.g. sending an SMS) set "true" this parameter in order to pause checks and let SIM800 finishing command
//    _PRINT_TEST_CSQ(__FUNCTION__);
//    _PRINTLN_TEST_CSQ(phase);
//    _PRINT_TEST_CSQ("delaychecks");
//    initTime = millis();
//    phase=-1;          //phase -1 will use GSM_CLA_LONG_TIMING timer instead of GSM_CLA_TIMING
//    _PRINTLN_TEST_CSQ(phase);
//    return;
//  };

  if (((millis() - initTime) < GSM_CLA_TIMING)  && (gsmStatus != 'S'))   // if GSMcsq called by (first call after) setup: it could be before GSM_CLA_TIMING+1
    return;
  _PRINT_TEST_CSQ("smsStatus=");
  _PRINT_TEST_CSQ(smsStatus);
  if (smsStatus=='S')
    return;
//  if ((phase==-1) && ((millis() - initTime) < GSM_CLA_LONG_TIMING))       // see few rows above
//    return;
  if (SIM800.available())                     // phone running a job, skip checks to the next time
    return;
  _PRINTLN_TEST(__FUNCTION__);
  _PRINT_TEST_CSQ(__FUNCTION__);
  _PRINTLN_TEST_CSQ(phase);
  _PRINTLN_TEST_CSQ(millis()-initTime);
  
  if (SIM800.available())                     // phone running a job, skip checks to the next time
    return;
  if ((gsmStatus == 'F') && (phase>0)) {
    initTime = millis();
    phase=(++phase % CSQ_NUM_PHA);
    return;
  }
  if (phase!=-1) {
    _PRINTLN_TEST_CSQ(" gsmStatus="+String(gsmStatus));
    UpdateLocalClock();  
    _PRINT_TEST_CSQ("...updated ");
    _PRINTLN_TEST_CSQ(" gsmStatus="+String(gsmStatus));
  }
  switch(phase) {
//  case -1:
//    phase=0;
//    return;
  case 2:
    if (SIM800print(F("AT+CPBW=?"), &csq)) {
      if (csq.substring(0,5) != F("+CPBW")){
        gsmStatus = 'F';
        break;
      }
    } else      
      gsmStatus = 'F';
    break;
  case 1:
    if (SIM800print(F("AT+CREG?"), &csq))
      if ((csq.charAt(9) != '1') && (csq.charAt(9) != '5'))
        gsmStatus = 'E';
    break;
  case 0:
    if (gsmStatus == 'F')
      break;
    if (SIM800print(F("AT+CSQ"), &csq))
      v = csq.substring(csq.indexOf(':') + 1, csq.indexOf(',')).toInt(); //getting an 'int' quality value from the return string
    if (v <= 1)
      gsmStatus = 'E';
    else if (v <= 6)
      gsmStatus = 'D';
    else if (v <= 13)
      gsmStatus = 'C';
    else if (v <= 21)
      gsmStatus = 'B';
    else if (v <= 31)
      gsmStatus = 'A';
    else if (v <= 101)
      gsmStatus = 'E';
    else if (v <= 115)
      gsmStatus = 'D';
    else if (v <= 130)
      gsmStatus = 'C';
    else if (v <= 146)
      gsmStatus = 'B';
    else if (v <= 191)
      gsmStatus = 'A';
    else
      gsmStatus = 'E';
//    phase=1;
    break;
  }
  initTime = millis();
  phase=(++phase % CSQ_NUM_PHA);
  _PRINT_TEST_CSQ("gsmStatus= "+String(gsmStatus));
  _PRINTLN_TEST_CSQ(" finished. Next phase is:"+String(phase));
}



bool CheckSIM800Requests() {
bool result;
  result=CheckSIM800Requests(false);
  return result;
}

bool CheckSIM800Requests(bool isSetup) {        // managing unsolicited messages receiving by SIM800
                                                // action proceed step by step: calling this function, action proceed by 1 step
                                                // cUPSstatus stores step to perform
                                                // only 'S' status perform all setup activities in just one step
  String scom, msgT, msgT2;
  char b;
  static char cUPSstatus = 'Q'; //phone is quiet
  /* cUPSstatus Q = Quiet: no activity on phone
                I = incoming call
                E = execute SMS command
                R = send an SMS with command results
                D = delete current SMS (position is "mesno")
                S = Setup (reset button pushed)
  */
  static unsigned long initTime;
  static String caller, callername, mesno, comm, param, reply;
  static bool smsResult;
  
  msgT2 = "";
  _PRINTLN_TEST(__FUNCTION__);

  if (isSetup){
    initTime=millis();
    cUPSstatus='S';
    rstStatus = 'S';
  }
  
  #ifdef _TEST_SMS_MGMT      
    if (cUPSstatus != 'Q') {           //SIM800 is quiet, if any message, it is a new alert
      Serial.println("Entering SMSmgm with status: "+String(cUPSstatus));
    }
  #endif
  #ifdef _TEST
    if (cUPSstatus != 'Q')            //SIM800 is quiet, if any message, it is a new alert
      Serial.println("CheckSIM800Requests: "+String(cUPSstatus));
  #endif
  if (cUPSstatus == 'S') {           //After resetting phonebook (reset button pushed) wait for a RING until override waittime (5 mins). Caller is registred as Master. 
    if (millis()-initTime>RST_SETUP_TIME) {
      _PRINTLN_TEST_HAND_RES("5 mins grace time; finished...");
      rstStatus = 'N';
      pinMode(RESET_PIN,INPUT);
      cUPSstatus = 'Q';
      #ifdef _TEST_WDT
        wdt_enable(WDTO_8S);    //enable watchdog for 8 secs timeout
        wdt_reset();   //reset watchdog
      #endif
      return false;      
    }
    if (!SIM800.available())          // there are no messages to read:
      return false;                  // exit
    if (!ReadSIM800(&msgT)) {
      return true;                                                //  something went wrong: retry
    }
    if (msgT == F("RING")) {                                   //ACTION: RINGING ALERT
      _PRINTLN_TEST_HAND_RES("Ringing...");
      msgT = "";
      do 
        ReadSIM800(&msgT);
      while (msgT.substring(0,5)!=F("+CLIP"));
      //extracting phone number
      _PRINTLN_TEST_HAND_RES("received message: "+msgT);
      msgT = msgT.substring(msgT.indexOf('"') + 1);
      caller = msgT.substring(0, msgT.indexOf('"'));      //get the caller
      _PRINTLN_TEST_HAND_RES("Ringing: caller is "+caller);
      delay(GSM_LET_CALL_RINGING);                        // wait a little to let caller understand pgone is ringing
      _PRINTLN_TEST_HAND_RES("Hanging up");
      smsResult = SIM800print(F("ATH"));                  //hang up
      _PRINTLN_TEST_SMS_MGMT("ATH result= "+String(smsResult));
      msgT=F("AT+CPBW=,\"");
      msgT+= caller; 
      msgT+= F("\",,\"MASTER\"");
      SIM800print(msgT,&msgT2);                         //register new Master
      delay(GSM_LET_CALL_RINGING);                        // not overlapping 2 commands
      msgT=F("AT+CMGS=\"");                                    // start a new output SMS
      msgT+= caller;
      msgT+= "\"";
      _PRINTLN_TEST_SMS_MGMT("Preparing reply command to SIM800:");
      _PRINTLN_TEST_SMS_MGMT("one: "+msgT);
      SIM800print(msgT);
      msgT=F(_M_SM036);
      msgT+=(caller);
      msgT2=(F(_M_SM037));
      msgT+=msgT2;
      _PRINTLN_TEST_SMS_MGMT("reply: " + msgT);
      SIM800print(msgT);
      SIM800print(String(char(26)));
      initTime=millis();                        //restarting 5 mins timer
      smsStatus = 'S';
    }
    return false;
  }
  if (cUPSstatus == 'Q') {           //SIM800 is quiet, if any message, it is a new alert
    if (!SIM800.available())          // there are no messages to read:
      return false;                  // exit
    _PRINTLN_TEST("CheckSIM800Requests: "+String(cUPSstatus)+" -> "+msgT);
    if (!ReadSIM800(&msgT)) {
      return true;                                                //  something went wrong: retry
    }
    if (msgT == F("RING")) {                                   //ACTION: RINGING ALERT
      _PRINTLN_TEST_SMS_MGMT("Ringing...");
      msgT = "";
      do 
        ReadSIM800(&msgT);
      while (msgT.substring(0,5)!=F("+CLIP"));
      //extracting phone number
      _PRINTLN_TEST_SMS_MGMT("received message: "+msgT);
      msgT = msgT.substring(msgT.indexOf('"') + 1);
      caller = msgT.substring(0, msgT.indexOf('"'));
      
      initTime = millis();   //wait before hanging up call: let the caller hearing ringing
      _PRINTLN_TEST_SMS_MGMT("Ringing: caller is "+caller);
      msgT = msgT.substring(0,msgT.lastIndexOf('"'));
      callername = msgT.substring(msgT.lastIndexOf('"')+1);
      if (callername!="") {      // caller has a name -> it is stored in phonebook
        cUPSstatus = 'I';                                      //proceed to: hangup call
        smsStatus = 'I';
        gateStatus = 'O';
      }
      _PRINTLN_TEST_SMS_MGMT("Ringing: caller name  is >"+callername+"<");
      
      return false;
    } else if (msgT.substring(0, 5) == F("+CPIN")) {            //ACTION: SIMCARD FAULT ALERT
      if (!(msgT != F("+CPIN: READY")))
        gsmStatus = 'E';                                           // no phone line
      cUPSstatus = 'Q';                                            // proceed to: stay quiet
      return false;
    } else if (msgT.substring(0, 5) == F("+CMTI")) {            //ACTION: NEW SMS ALERT
      mesno = msgT.substring(12);
      smsStatus = 'R';                                             // set "receiving SMS" led
      scom=F("AT+CMGR=");
      scom+= mesno;
      SIM800print(scom, &msgT, &msgT2, true);        //get SMS //slow command
      _PRINTLN_TEST_SMS_MGMT("Asked SMS content: "+scom);
      _PRINTLN_TEST_SMS_MGMT("returned: "+msgT);
      _PRINTLN_TEST_SMS_MGMT("returned: "+msgT2);

      msgT2 = msgT2.substring(msgT2.indexOf('"') + 1);
      msgT2 = msgT2.substring(msgT2.indexOf('"') + 1);
      msgT2 = msgT2.substring(msgT2.indexOf('"') + 1);
      caller = msgT2.substring(0, msgT2.indexOf('"'));
      msgT2 = msgT2.substring(msgT2.indexOf('"') + 1);
      msgT2 = msgT2.substring(msgT2.indexOf('"') + 1);
      if (msgT2.substring(0, msgT2.indexOf('"'))=="MASTER") {
        comm = msgT.substring(0, msgT.indexOf(' '));
        param = msgT.substring(comm.length());
        param.trim();
        _PRINTLN_TEST_SMS_MGMT("translated as... ");
        _PRINTLN_TEST_SMS_MGMT("caller: "+caller);
        _PRINTLN_TEST_SMS_MGMT("command: "+comm);
        _PRINTLN_TEST_SMS_MGMT("parameters: "+param);
        cUPSstatus = 'E';                                            // proceed to: execute SMS command
      } else
        cUPSstatus = 'Q';                                            // SMS sender is not a Master. Forget the message
    } else if ((msgT.substring(0, 5) == F("+CMGS")) || (msgT.substring(0, 5) == F("+CNTP"))) {           //ACTION: SIM800 IS CONFIRMING ABOUT PREVIOUSLY SENT SMS OR CLOCK SYNC
      return false;                                                // no action to perform
    }  else if ((msgT == F("OK")) || (msgT == F(""))) {        //ACTION: OTHER CONFIRMATIONS
      return false;                                               // no action to perform
    } else {                                                  //ACTION: NOT MANAGED ALERT
      _PRINT_TEST_SIM800_SERIAL(F(_M_PH001));
      _PRINTLN_TEST_SIM800_SERIAL(msgT);
      _PRINT_TEST_SIM800_SERIAL(F("gsmStatus="));
      _PRINTLN_TEST_SIM800_SERIAL(gsmStatus);
      _PRINT_TEST_ON_USB(F(_M_PH001));
      _PRINTLN_TEST_ON_USB(msgT);
      return true;                                                // keep reading from SIM800
    }
  } else if (cUPSstatus == 'E') {                             // EXECUTE COMMAND RECEIVED VIA SMS
    reply = "";
    _PRINTLN_TEST_SMS_MGMT("Before execution");
    _PRINTLN_TEST_SMS_MGMT("caller: "+caller);
    _PRINTLN_TEST_SMS_MGMT("command: "+comm);
    _PRINTLN_TEST_SMS_MGMT("parameters: "+param);
    _PRINTLN_TEST_SMS_MGMT("reply: "+reply);
    smsResult = SMScommand(&comm, &param, &reply, &caller);
    _PRINTLN_TEST_SMS_MGMT("After execution");
    _PRINTLN_TEST_SMS_MGMT("caller: "+caller);
    _PRINTLN_TEST_SMS_MGMT("command: "+comm);
    _PRINTLN_TEST_SMS_MGMT("parameters: "+param);
    _PRINTLN_TEST_SMS_MGMT("reply: "+reply);
    cUPSstatus = 'R';                                              // proceed to: send an SMS with reply
  } else if (cUPSstatus == 'R') {                              //SEND CALLER RESULT ABOUT COMMAND EXECUTION
    if (reply!=""){                                // if there is no reply, NO SMS is sent back to caller!
      msgT=F("AT+CMGS=\"");                                    // start a new output SMS
      msgT+= caller; 
      msgT+= "\"";
      _PRINTLN_TEST_SMS_MGMT("Preparing reply command to SIM800:");
      _PRINTLN_TEST_SMS_MGMT("one: "+msgT);
      SIM800print(msgT);
      _PRINTLN_TEST_SMS_MGMT("gsmStatus1: "+String(gsmStatus));
      if (!smsResult){                            // if SMScommand failed, reply is preceded by a "failed" warning
        msgT = F(_M_PH002);
        msgT += comm;
        msgT += F(_M_PH003);
        _PRINTLN_TEST_SMS_MGMT("Preparing reply command to SIM800:");
        _PRINTLN_TEST_SMS_MGMT("two: "+msgT);
        SIM800print(msgT);
        _PRINTLN_TEST_SMS_MGMT("gsmStatus2: "+String(gsmStatus));
      } 
      msgT="";
      for (b=0;b<reply.length();b++){
        if (reply.charAt(b)==char(10)){
          SIM800print(msgT);
          msgT="";
        }
        else
          if (reply.charAt(b)!=char(13))
            msgT+=reply.charAt(b);
      }
      _PRINTLN_TEST_SMS_MGMT("Preparing reply command to SIM800:");
      _PRINTLN_TEST_SMS_MGMT("three: "+msgT);
      if(msgT!="")
        SIM800print(msgT);
      _PRINTLN_TEST_SMS_MGMT("gsmStatus3: "+String(gsmStatus));
      SIM800print(String(char(26)));
      _PRINTLN_TEST_SMS_MGMT("gsmStatus4: "+String(gsmStatus));
      
    }
    cUPSstatus = 'Q';                            //Finished. "SENT" confirmation will return as a new unsolicited alert
    smsStatus = 'S';
  } else if (cUPSstatus == 'D') {         //DELETE EXECUTED SMS
    msgT=F("AT+CMGD=");
    msgT+= mesno;
    if (SIM800print(msgT)) {
      cUPSstatus = 'Q';
    }
  }
  else if (cUPSstatus == 'I') {           //hanging up incoming call 
    if (millis() - initTime <= GSM_LET_CALL_RINGING)
      return;
    _PRINTLN_TEST_SMS_MGMT("sending ATH");
    smsResult = SIM800print(F("ATH")); //hang up
    _PRINTLN_TEST_SMS_MGMT("ATH result= "+String(smsResult));
    cUPSstatus = 'Q';
  }
  return false;
}




bool InitSIM800() {
  String strT = "";
  char ch = "";
  byte i = 0, ii; //two counters to limit retrying checking the phone reply
  unsigned long initTime;

  //testing communication and syncing autobaud
  _PRINTLN_TEST(__FUNCTION__);
  _PRINT_TEST_ON_USB(F(_M_PH004));
  _PRINTLN_TEST_ON_USB(" "+String(millis()));
  while ((i <= STUP_CHECKS) ) {
    if (SIM800print(F("AT"))) {
      break;
    }
    i++;
    delay(GSM_SETUP_DELAY);
  }
  gsmStatus = 'S'; // Phone is setting up
  if (i > STUP_CHECKS) {
    gsmStatus = 'F';
    return false;
  }
  _PRINTLN_TEST_SIM800("setup checks="+String(i));

  InitLed('1');
  //waiting for SIM800 slow wake-up messages
  _PRINTLN_TEST_ON_USB(F(_M_PH005));
  initTime = millis();
  while (millis() - initTime < GSM_WAKEUP_DELAY) {
    strT = "";
    if (SIM800.available()) {
      ReadSIM800(&strT);
      initTime = millis();
    }
    _PRINTLN_TEST_SIM800("wakeup message: "+strT);
  };
  
  InitLed('2');
  //setting SIM800 AT parameters
  _PRINTLN_TEST_ON_USB(F(_M_PH006));
  _PRINTLN_TEST_SIM800("gsmStatus=: "+String(gsmStatus));
  SIM800print(F("ATE0"));  //set Echo Off
  SIM800print(F("AT+CMGF=1"));  //set SMS messages as TXT format
  SIM800print(F("AT+CLIP=1"));  //phone must provide calling number after a RING
  #ifdef _TEST_SIM800
    SIM800print(F("AT+IPR=0"));  //Set baudrate = "autobaud" for the next time
  #endif
  #ifdef _TEST_ON_USB
    Serial.print(F(_M_PH0A7));
    if ((gsmStatus != 'F'))
      Serial.println(F(_M_PH007));
    else
      Serial.println(F(_M_PH008));
  #endif

  InitLed('3');
  SIM800print(F("AT+CMGDA=DEL ALL"));  //delete and previous SMS left on simcard 
  return (gsmStatus != 'F');
}



void ClockSetup() {
  String scom, strT1, strT2, currTime;
  bool a;
  char b;
  unsigned long initTime;
  _PRINTLN_TEST(__FUNCTION__);

  if (smsStatus=='S')  //phone is sending SMSs. wait for the next time
    return;

  strT1 = "";
  clkStatus = 'N';

//  return;   //remove comment to test case of not working NTP clock sync
  
  //reading APN server from EEPROM
  for (int k = 0; k < 30; k++) {
    b = EEPROM.read(k + EE_APN_ADDR);
    if ((b == char(0)) || (b == char(255)))
      break;
    strT1 += String(b);
  }
  _PRINTLN_TEST_SIM800("APN server=: "+strT1);
  if (strT1 == "") //if APN not set in EEPROM do not update clock
    return;
  //reading TIMEZONE from EEPROM
  strT2 = "";
  for (int k = 0; k < 3; k++) {
    b = EEPROM.read(k + EE_TZ_ADDR);
    if ((b == char(0)) || (b == char(255))) //if TIMEZONE not set in EEPROM...
      break;
    strT2 += String(b);
  }
  
  scom=F("AT+CCLK=\"01/01/01,01:01:01");
  scom += strT2;
  scom += "\"";
  _PRINTLN_TEST_SIM800("TimeZone=: >"+strT2+"<");
  _PRINTLN_TEST_SIM800("first CCLK command=: >"+scom+"<");
  
  SIM800print(scom); // just to set Timezone in cclk, below here find date/time update
  if (strT2 == "") //don't need to check other conditions already verified on writing EEPROM
    return;  //if TIMEZONE not set in EEPROM do not update clock
  SIM800print(F("AT+SAPBR=3,1,\"Contype\",\"GPRS\""));
  scom =F("AT+SAPBR=3,1,\"APN\",\"");
  scom += strT1;
  scom += F("\"");
  SIM800print(scom);  
  SIM800print(F("AT+SAPBR=1,1"), &strT1, &strT2, true);
  _PRINTLN_TEST_SIM800("Started GPRS");
  SIM800print(F("AT+SAPBR=2,1"), &strT1); 
  // check GPRS connection
  strT1 = strT1.substring(strT1.indexOf('"') + 1);
  strT1 = strT1.substring(0, strT1.indexOf('"'));
  _PRINTLN_TEST_SIM800("IP address=: "+strT1);
  if (strT1 == F("0.0.0.0")) {
    return;
  }
  SIM800print(F("AT+CNTPCID=1")); //slow AT command?
  strT1 = "";
  clkStatus = 'N';
  for (int k = 0; k < 30; k++) {
    b = EEPROM.read(k + EE_NTP_ADDR);
    if ((b == char(0)) || (b == char(255)))
      break;
    strT1 += String(b);
  }
  _PRINTLN_TEST_SIM800("NTP server=: "+strT1);
  
  for (int k = 0; k < 3; k++) {
    char b = EEPROM.read(k + EE_TZ_ADDR);
    if ((b == char(0)) || (b == char(255)))
      break;
    strT2 += String(b);
  }
  if (strT2.charAt(0) == '+')
    strT2 = strT2.substring(1); //CNTP doesn't want the "+" sign"
  if (strT2.charAt(0) == '0')
    strT2 = strT2.substring(1); //and trailing zeros
  scom = F("AT+CNTP=\"");
  scom += strT1;
  scom += F("\",");
  scom += strT2;
  _PRINTLN_TEST_SIM800("CNTP cpmmand=: "+scom);
  SIM800print(scom, &strT1, &strT2, true); //clock setting. slow AT command!!!
  SIM800print(F("AT+CNTP"), &strT1, &strT2, true); //clock setting. slow AT command!!!
  initTime = millis();
  while (millis() - initTime < GSM_CLOCK_SETUP_DELAY) {
    strT1 = "";
    if (SIM800.available()) {
      ReadSIM800(&strT1);
      if (strT1.substring(0,5)==F("+CNTP"))
        break;
      initTime = millis();
    }
  }
  SIM800print(F("AT+SAPBR=0,1"), &strT1, &strT2, true); //slow AT command
  _PRINTLN_TEST_SIM800("Closed GPRS");
  SIM800print(F("AT+CCLK?"), &currTime);
  _PRINTLN_TEST_SIM800("Clock time (CCLK):"+currTime);
  if (currTime.substring(8, 10) == F("01")) //that means now is 2001??? clock wasn't set!
    return;   // do not set clkStatus as 'S'
  clkStatus = 'S';
}
