/* 
 * these functions drive accessing to SIM800 phonebook 
 */



void LookForExpiringUsers(){
String strT1;
char preGsmStatus;

  if (gsmStatus=='F')
    return;
  if (smsStatus=='S')
    return;
  expiringUsers=false;
  preGsmStatus=gsmStatus;
  _PRINTLN_TEST(__FUNCTION__);
  _PRINTLN_TEST_PHONEBOOK(__FUNCTION__);
  strT1=F("AT+CPBF");
  SIM800.println(strT1);
  _PRINTLN_TEST_SIM800_SERIAL("SIM800.println() msg: >"+strT1+"< gsmStatus= "+String(gsmStatus));

  while(!SIM800.available()) {_PRINTLN_TEST_SIM800_SERIAL("SIM800.available() loop10"); };
  while (ReadSIM800(&strT1, true)) {
    if (strT1 != "") {
      if ((strT1 == F("OK")) || (strT1.indexOf(F("ERROR")) != -1)){
        _PRINTLN_TEST_PHONEBOOK("LookForExpiringUsers read 'OK'");  
        break;      
      }
      if ((strT1.substring(0,5)=F("+CPBF"))) {
        if (strT1.indexOf('/')!=-1){
          expiringUsers=true;
          break;
        }
      }
    }
    while(!SIM800.available()) {_PRINTLN_TEST_SIM800_SERIAL("SIM800.available() loop9");};
  }
  _PRINTLN_TEST_PHONEBOOK("Test is:"+strT1);  

  while ((strT1 != F("OK")) && (strT1.indexOf(F("ERROR")) == -1)){
    while(!SIM800.available()) {   _PRINTLN_TEST_SIM800_SERIAL("SIM800.available() loop8"); }
    ReadSIM800(&strT1,true);
  }
  _PRINTLN_TEST_PHONEBOOK("LookForExpiringUsers finished");  
  
  dontGetTime=true;
  gsmStatus=preGsmStatus;
//  GsmCsq(true);
  return;
};


bool CleanSimCard(){
bool result;
  if (smsStatus=='S')
    return;
  result=CleanSimCard(false);
  return result;
}

bool CleanSimCard(bool anyRegistration){
/*  
 *  delete ONE expired registration on phonebook
 *  if "anyRegistration" execute cleaning of the first registration found, regardless expiring date
 *  if !anyRegistration comes from the main loop: it must perform checks for expired registrtion every CSC_TIMING millis()
 */
static unsigned long initTime=0; //start time for timer
String strT1, strT2, locClk, todelete;

  if (((millis()-initTime) < CSC_TIMING) && !anyRegistration)
    return false;
  _PRINTLN_TEST(__FUNCTION__);
  if (SIM800.available())
    if (anyRegistration) {
      while (SIM800.available())  //we are in simcard reset
        ReadSIM800(&strT1, true);
    } else {
      return false;  //there is a pending SIM800 communication: skip tests and back to main loop()
    };
  _PRINTLN_TEST_PHONEBOOK("Cleansimcard: millis= "+String(millis())+", initTime= "+String(initTime));
  _PRINTLN_TEST(__FUNCTION__);
  locClk = currClock.substring(0,8);
  _PRINTLN_TEST_PHONEBOOK("currClock:"+currClock);  
  _PRINTLN_TEST_PHONEBOOK("locCkl:"+locClk);  
  strT1=F("AT+CPBF=");
  SIM800.println(strT1);
  _PRINTLN_TEST_SIM800_SERIAL("SIM800.println() msg: >"+strT1+"< gsmStatus= "+String(gsmStatus));
  while(!SIM800.available()) { _PRINTLN_TEST_SIM800_SERIAL("SIM800.available() loop7");};
  todelete="";           // record # to delete
  while (ReadSIM800(&strT1, true)) {
    if (strT1 != "") {
      if ((strT1 == F("OK")) || (strT1.indexOf(F("ERROR")) != -1)){
        _PRINTLN_TEST_PHONEBOOK("CleanSimCard read 'OK'");  
        initTime=millis(); 
        break;      
      }
      if ((strT1.substring(0,5)=F("+CPBF"))) {
        strT2=strT1.substring(0,strT1.lastIndexOf('"'));
        strT2=strT2.substring(strT2.lastIndexOf('"')+1);
        strT2.trim();
        if ((strT2.length()>6) || (anyRegistration)){
          strT2=strT2.substring(5);
          _PRINTLN_TEST_PHONEBOOK("pos0A "+strT2);
          _PRINTLN_TEST_PHONEBOOK("pos0B "+locClk);
          if ((locClk>strT2) || (anyRegistration)){
            todelete="";
            todelete=strT1.substring(strT1.indexOf(' '));
            todelete.trim();
            todelete=todelete.substring(0,todelete.indexOf(','));
            todelete.trim();
            break;                               //found a record with expired date: exit!
                                                 //without setting the timer so: in each loop() will delete 1 record. When no more expired date (it'll read "OK") will set timetogo before exiting.
          } else
            expiringUsers=true;
        }
      }  
    }
    while(!SIM800.available()) {_PRINTLN_TEST_SIM800_SERIAL("SIM800.available() loop6");};
  }
  _PRINTLN_TEST_PHONEBOOK("Before. endOfList="+strT1);  

  while ((strT1 != F("OK")) && (strT1.indexOf(F("ERROR")) == -1)){
    while(!SIM800.available()) { _PRINTLN_TEST_SIM800_SERIAL("SIM800.available() loop5"); }
    ReadSIM800(&strT1,true);
  }

  _PRINTLN_TEST_PHONEBOOK("After. endOfList="+strT1);  

  if (todelete!="") {
    strT1=F("AT+CPBW=");
    strT1+=todelete;
    _PRINTLN_TEST_PHONEBOOK("Deleting registration. Command "+strT1);
    strT2="";
    SIM800print(strT1,&strT2);                  //delete simcard position
    _PRINTLN_TEST_PHONEBOOK("Deleting result; "+strT2);
  }
  dontGetTime=true;
  _PRINTLN_TEST_PHONEBOOK("Cleansimcard: next time= "+String(initTime+CSC_TIMING));
  return (todelete!="");
}




byte FindNumber(String *num, String *uname){
/*
 * returns the memory position of *num phonenumber
 * returns 0 if not found
 * uname returns the name
 * 
 */
int check;
String txt,txt1;
byte ini,fin,index;

  _PRINTLN_TEST(__FUNCTION__);
  *uname="";
  SIM800.println(F("AT+CPBF"));
  _PRINTLN_TEST_SIM800_SERIAL("SIM800.println() msg: >AT+CPBF< gsmStatus= "+String(gsmStatus));
  while (!SIM800.available()) {_PRINTLN_TEST_SIM800_SERIAL("SIM800.available() loop4");};
  check=-1;
  while (ReadSIM800(&txt, true)) {
    if (txt != "") {
      if (txt.substring(0,5)!=F("+CPBF"))
        break;      
      ini=txt.indexOf("\"");
      fin=txt.indexOf("\"",ini+1);
      txt1=txt.substring(ini+1,fin);
      check=txt1.indexOf(*num);
      if (check!=(-1))
        break;
      check=num->indexOf(txt1);
      if (check!=(-1))
        break;
    };
  }
  index=0;

  if (check!=-1){
    txt1=txt.substring(txt.indexOf(" ")+1,txt.indexOf(","));
    index=txt1.toInt();
    ini=txt.indexOf("\"")+1;
    ini=txt.indexOf("\"",ini)+1;
    ini=txt.indexOf("\"",ini)+1;
    fin=txt.indexOf("\"",ini);
    *uname=txt.substring(ini,fin);
    
  }
  if ((txt != F("OK")) && (txt.indexOf(F("ERROR")) == -1))
    while (ReadSIM800(&txt, true)) {
      if ((txt == F("OK")) || (txt.indexOf(F("ERROR")) != -1))
        break;
    };  
  return index;
}
