/*  SMS COMMAND MANAGER
 *  Commands available:
 *  
 * Command parameters <> description
 * 
 * APN server-name                 <> set the APN server address of the GSM operator
 * TIMEZONE sxx                    <> set the local timezone ("s" = sign (+ or -), xx indicates the timezone IN QUARTER OF AN HOUR (-47 to +48). E.g. CET is +08
 * NTP server-name                 <> set the NTP server address polled syncronizing internal clock.
 * NTP ip-address                  <> same as above
 * STATS                           <> report about system activity
 * MASTER phonenumber              <> register a new master on phonebook
 * DEL phonenumber                 <> delete a registred number. 
 * USER phonenumber [expiry_date]  <> expiry date format: YY/MM/GG 
 * LIST MASTERS                    <> reports number of masters registred and firsts N numbers 
 * LIST USERS                      <> reports number of usess registred and firsts N numbers 
 * 
 * Command are available only to MASTERs
 * 
*/



void TestSmsCommand(String *comm,String *param,String *caller){

/*
 * Debugging "simulator" of SMS messages. Calling SMScommand via Serial instead of GSM line
 * 
 */
  
String msgT,reply;
bool smsResult;
    if (*caller=="")
      *caller=F("3396815817"); // testing
    _PRINTLN_TEST(__FUNCTION__);
    _PRINTLN_TEST_SMS_COMMAND("TestSmsCommand: "+*comm+" / "+*param+" / "+*caller);

    smsResult = SMScommand(comm, param, &reply, caller);
    msgT="";
    if (!smsResult){
      msgT=F(_M_SM001);
      msgT+=*comm;
      msgT+=F(_M_SM003);
      if (reply=="")
        msgT+=F("!");
      else  
        msgT+=F(":");
    }
    Serial.println(msgT);
    if (reply!="")
      Serial.println(reply);
}




bool SMScommand(String *comm,String *param,String *reply, String *caller){
/*
 * returning *reply="" means: DO NOT send a reply to caller (not authorized) 
 */
byte k;
bool test,isMaster=false;
String strT,strT1,strT2,strT3,strT4;
unsigned long curtime = millis();
char ch;

  _PRINTLN_TEST(__FUNCTION__);
  *reply = "";
  comm->toUpperCase();
//  k=FindNumber(caller,&strT2);
//  isMaster=(strT2==F("MASTER"));
//  #ifdef _TEST_SMS_COMMAND
//    Serial.println("TestSmsCommand: "+*comm+" / "+*param+" / "+*caller);
//    if (isMaster)
//      Serial.println("it is Master");
//    else
//      Serial.println("it is NOT a Master");
//  #endif
//  
//  if (!isMaster)
//    return;          //in not master do not proceed. Don't send any reply,
//    
  if (*comm==F("APN")){
    if (*param=="") {
      strT1=F(_M_SM0B5);
      strT2=F(_M_SM005);
      *reply=strT1+strT2;
      return true;
    } else if ((param->length())>SMS_PARAM_LEN ){
      strT1=F(_M_SM005);
      strT2=F(_M_SM0A5);
      *reply=strT1+strT2;
      return false;
    } else {
      for (k=0; k<(param->length()); k++){
        EEPROM.update(EE_APN_ADDR+k,param->charAt(k));
      }
      EEPROM.update(EE_APN_ADDR+(param->length()),char(0));
      strT1=F(_M_SM005);
      strT2=F(_M_SM012);
      *reply=strT1+strT2;
      
    }
    return true;

 
  } else if (*comm==F("TIMEZONE")){
    test=(((param->length())==3) && (String("+-").indexOf(param->charAt(0))!=-1) && (isDigit(param->charAt(1))) && (isDigit(param->charAt(2))));
    if (test && (param->toInt()>(-48)) && (param->toInt()<(49))) {
      for (k=0; k<3; k++)
        EEPROM.update(EE_TZ_ADDR+k,param->charAt(k));
      strT1==F(_M_SM045);
      strT2==F(_M_SM012);
      *reply=strT1+strT2;
      k=EEPROM.read(EE_NTP_ADDR);
      if ((k==0) || (k==255)){ // user is setting TIMEZONE: set also the default NTP server
        strT=F(EE_DEF_NTP_SERV);
        for (k=0; k<(strT.length());k++)
          EEPROM.update(EE_NTP_ADDR+k,strT.charAt(k));
        EEPROM.update(EE_NTP_ADDR+(strT.length()),char(0));
      }
      return true;
    } else {
      *reply=F(_M_SM008);
      *reply+=String(char(10));
      *reply+=F(_M_SM009);
      return false;
    }

  } else if (*comm==F("NTP")){
    if (*param=="") {
      strT1=F(_M_SM0B5);
      *reply=strT1;
      strT1=F(_M_SM011);
      *reply+=strT1;
      strT1=F(_M_SM010);
      *reply+=strT1;
      return false;
    } else if ((param->length())>SMS_PARAM_LEN ){
      strT1=F(_M_SM011);
      strT2=F(_M_SM0A5);
      *reply=strT1+strT2;
      return false;
    } else {
      for (k=0; k<(param->length());k++)
        EEPROM.update(EE_NTP_ADDR+k,param->charAt(k));
      EEPROM.update(EE_NTP_ADDR+(param->length()),char(0));
      strT1=F(_M_SM011);
      strT2=F(_M_SM012);
      *reply=strT1+strT2;
    }
    return true;

  } else if (*comm==F("DEL")){
    if (*param=="") { 
      strT1=F(_M_SM0B5);
      strT2=F(_M_SM014);
      *reply=strT1+strT2;
      return false;
    } else if ((param->length())>20){
      strT1=F(_M_SM014);
      strT2=F(_M_SM0A5);
      *reply=strT1+strT2;
      return false;
    } else {
      k=FindNumber(param,&strT1);
      _PRINTLN_TEST_PHONEBOOK("DEL command, found record:"+String(k));
      if (k==0){
        strT1=F(_M_SM014);
        strT2=F(_M_SM015);
        *reply=strT1+strT2;
        return false;
      } else {
        strT1=F("AT+CPBW=");
        strT1+= String(k);
        SIM800print(strT1);                  //delete simcard position
        LookForExpiringUsers();  // check if phonebook have expiring users: for the proper led blinking
        strT1=F(_M_SM014);
        strT2=F(_M_SM0A4);
        *reply=strT1+strT2;
        return true;
      }
    }
  } else if (*comm==F("MASTER")){
    if (*param=="") {
      strT1=F(_M_SM0B5);
      strT2=F(_M_SM014);
      *reply=strT1+strT2;
      return false;
    } else if ((param->length())>SMS_PARAM_LEN ){
      strT1=F(_M_SM014);
      strT2=F(_M_SM0A5);
      *reply=strT1+strT2;
      return false;
    } else {
      k=FindNumber(param,&strT1);
      if (strT1==F("MASTER")) {                                   //already registred as Master
        *reply=F(_M_SM035);
        return false;
      }
      if (k!=0) {                                               //already registred as user, updated as Master
        strT1=F("AT+CPBW=");
        strT1+=String(k)+",\"" + *param;
        strT1+=F("\",,\"MASTER\"");
        SIM800print(strT1,&strT);
      } else {                                                    //not yet registred
        strT1=F("AT+CPBW=,\"");
        strT1+= *param; 
        strT1+= F("\",,\"MASTER\"");
        SIM800print(strT1,&strT);
      }
      _PRINTLN_TEST_SMS_COMMAND("return: "+strT);
      *reply=F(_M_SM036);
      *reply+=(*param);
      strT=(F(_M_SM037));
      *reply+=strT;
      _PRINTLN_TEST_SMS_COMMAND("reply: " + *reply);
      return true;
    }


  } else if (*comm==F("LIST")){
    param->toUpperCase();
    if (*param=="MASTERS") 
      *param="MASTER";
    else if (*param!="USERS"){
      *reply=F(_M_SM033);
      return false;
    } else
      *param="USER";
    *reply="";
    strT1=F("AT+CPBF=");
    _PRINTLN_TEST_SMS_COMMAND("SIM800 command: "+strT1);
    
    SIM800.println(strT1);
    _PRINTLN_TEST_SIM800_SERIAL("SIM800.println() msg: >"+strT1+"< gsmStatus= "+String(gsmStatus));
    k=0;
    strT3=F(" - ");
    strT4=F("\n");
    byte tlen=param->length();
    while(!SIM800.available()) {_PRINTLN_TEST_SIM800_SERIAL("SIM800.available() loop3");};
    while (ReadSIM800(&strT1, true)) {
      _PRINTLN_TEST_SMS_COMMAND(String(k)+"<>"+strT1);
      if (strT1 != "") {
        if ((strT1 == F("OK")) || (strT1.indexOf(F("ERROR")) != -1)){
          _PRINTLN_TEST_SMS_COMMAND("Read 'OK' from SIM800");  
          break;      
        }
        if ((strT1.substring(0,5)=F("+CPBF"))) {
          strT2=strT1.substring(0,strT1.lastIndexOf('"'));      //stringing expiring date
          strT2=strT2.substring(strT2.lastIndexOf('"')+1);
          if (strT2.substring(0,tlen)==*param) {
            strT2=strT2.substring(tlen+1);
            if (strT2=="")
              strT=strT4;                   
            else
              strT=strT3+strT2+strT4;                            //expiring date string done.
            strT2=strT1.substring(strT1.indexOf('"')+1);         // Stringing phone no.
            strT2=strT2.substring(0,strT2.indexOf('"'));
            *reply+=strT2+strT;
            if (++k==MAX_PHONENO_IN_LIST) 
              break;
          }
        }
      }
      while(!SIM800.available()) {_PRINTLN_TEST_SIM800_SERIAL("SIM800.available() loop2");};
    }
    while ((strT1 != F("OK")) && (strT1.indexOf(F("ERROR")) == -1)){
      while(!SIM800.available()) { _PRINTLN_TEST_SIM800_SERIAL("SIM800.available() loop1"); }
      ReadSIM800(&strT1,true);
      if (strT1.substring(0,5)!="+CPBF")
        continue;
      strT2=strT1.substring(0,strT1.lastIndexOf('"'));
      strT2=strT2.substring(strT2.lastIndexOf('"')+1);
      if (strT2.substring(0,tlen)!=*param)
        continue;
      k++;
      _PRINTLN_TEST_SMS_COMMAND(String(k)+"<>"+strT1);
    }
    strT1="";
    if (k>MAX_PHONENO_IN_LIST) {
      strT1=F(_M_SM041);
      strT1+=String(MAX_PHONENO_IN_LIST);
      strT2=F(_M_SM046);
      strT1+=strT2;
    } 
    strT2=F(" ");
    strT1+=String(k)+strT2;
    strT2=F(_M_SM042);  
    strT1+=(*param)+strT2;
    *reply=strT1+*reply;
    return true;

  } else if (*comm==F("USER")){
      _PRINTLN_TEST_SMS_COMMAND("Entering USER");
    
      strT1 = param->substring(0, param->indexOf(' '));  //extracting phonenumber
      strT1.trim();
      strT2 = param->substring(strT1.length());  //extracting expiry date
      strT2.trim();
      if (strT1=="") {                             // no phone number
        strT1=F(_M_SM0B5);
                
        strT2=F(_M_SM014);
        *reply=strT1+strT2;
        return false;
      } 
      if ((strT1.length())>SMS_PARAM_LEN ){            //phone no. too long
        strT1=F(_M_SM014);
        strT2=F(_M_SM0A5);
        *reply=strT1+strT2;
        return false;
      } 
      _PRINTLN_TEST_SMS_COMMAND("Looking up into phonebook");
      
      k=FindNumber(&strT1,&strT3);
      _PRINTLN_TEST_SMS_COMMAND("FIndNumber returned position:"+String(k));
      
      if (strT3==F("MASTER")){                                                   // already registred as Master
        *reply=F(_M_SM038);
        return false;
      }

      _PRINTLN_TEST_SMS_COMMAND("before testing expiration");
      
      if (!((strT2.length()==0) || ((strT2.length()==8) && (strT2.charAt(2)=='/') && (strT2.charAt(5)=='/'))))  { 
        *reply=F(_M_SM039);   // expiry date must not exists or must be YY/MM/DD format
        return false;
      }
      #ifdef _TEST_SMS_COMMAND
        Serial.println("expiry date correct");
         Serial.println("Starting registration on phonebook");
         unsigned long before=millis();
      #endif

      if (!(strT2.length()==0))                                 //User has expiry date, so set "expiringUsers"
        expiringUsers=true; 

      if (k!=0) {                                                  //already registred as user, update it
        strT3=F("AT+CPBW=");
        strT3+=String(k)+",\"" + strT1;
        strT3+=F("\",,\"USER ");
        strT3+=strT2+"\"";
        SIM800print(strT3,&strT);
      } else {                                                    //not yet registred
        strT3=F("AT+CPBW=,\"");
        strT3+= strT1; 
        strT3+= F("\",,\"USER ");
        strT3+=strT2+"\"";
        SIM800print(strT3,&strT);
      }
      _PRINTLN_TEST_SMS_COMMAND("registred. Time is:"+String(millis()-before));
      *reply=F(_M_SM040);
      *reply+=(*param);
      strT=(F(_M_SM037));
      *reply+=strT;
      if ((param->indexOf("/")!=-1) && (!clkStatus)){
        strT=(F(_M_SM043));
        *reply+=strT;
      }
      return true;
        
  } else if (*comm==F("STATS")){ 
    _PRINTLN_TEST_SMS_COMMAND("Entering STATS");
    strT1 = F("/");
    strT2 = F("\n");
    strT=F(_M_SM024);
    strT+=String(gsmStatus);
    strT+=F("\"\n");
    ((clkStatus=='S') ? strT+=F(_M_SM025) : strT+=F(_M_SM026));
    strT+=F(_M_SM027);
    strT+=strT2;
    strT+=String(avgTime[G_LED])+strT1+ String(maxTime[G_LED]) +strT2;
    strT+=String(avgTime[S_LED])+strT1+ String(maxTime[S_LED]) +strT2;
    strT+=String(avgTime[GATE])+strT1+ String(maxTime[GATE]) +strT2;
    strT+=F(_M_SM029);
    strT+=String(avgTime[CSQ])+strT1+ String(maxTime[CSQ]) +strT2;
    strT+=F(_M_SM031);
    strT+=String(avgTime[ALERT])+strT1+ String(maxTime[ALERT]) +strT2;
    #ifdef _TEST_TIMING
      strT+=F("Reset  = ");
      strT+=String(avgTime[RES_B])+strT1+ String(maxTime[RES_B]) +strT2;
    #endif
    strT+=F(_M_SM032);
    strT+=String(avgTime[LOOP])+strT1+ String(maxTime[LOOP]) +strT2;
    strT+=F(_M_SM034);
    strT1=F(".");
    _PRINTLN_TEST_SMS_COMMAND(curtime);
    strT3+=String((curtime % 1000));
    strT3=strT1+strT3;
    curtime = (curtime-(curtime % 1000))/1000;
    _PRINTLN_TEST_SMS_COMMAND(curtime);
    strT1=F(":");
    strT3=String((curtime % 60))+strT3;
    strT3=strT1+strT3;
    curtime = (curtime-(curtime % 60))/60;
    _PRINTLN_TEST_SMS_COMMAND(curtime);
    strT3=String((curtime % 60))+strT3;
    strT3=strT1+strT3;
    curtime = (curtime-(curtime % 60))/60;
    _PRINTLN_TEST_SMS_COMMAND(curtime);
    strT1=F(",");
    strT3=String((curtime % 24))+strT3;
    strT3=strT1+strT3;
    curtime = (curtime-(curtime % 24))/24;
    _PRINTLN_TEST_SMS_COMMAND(curtime);
    strT3=String(curtime)+strT3;
    strT+=strT3+strT2;
    _PRINTLN_TEST_SMS_COMMAND("within SMScommand:");
    _PRINTLN_TEST_SMS_COMMAND(strT);
    *reply=strT;
    return true;

  
  
  } else if (*comm==F("CONFIG")){ 
    _PRINTLN_TEST_SMS_COMMAND("Entering STATS");
    strT2 = F("\n");
    strT=F(_M_SM005);
    strT3=F(_M_SM044);
    strT+=strT3;
    for (int k = 0; k < 30; k++) {
      ch = EEPROM.read(k + EE_APN_ADDR);
      if ((ch == char(0)) || (ch == char(255)))
        break;
      strT += String(ch);
    }
    strT+=strT2;
    strT1=F(_M_SM011);
    strT3=F(_M_SM044);
    strT+=strT1+strT3;
    for (int k = 0; k < 30; k++) {
      ch = EEPROM.read(k + EE_NTP_ADDR);
      if ((ch == char(0)) || (ch == char(255)))
        break;
      strT += String(ch);
    }
    strT+=strT2;
    strT1=F(_M_SM045);
    strT3=F(_M_SM044);
    strT+=strT1+strT3;
    for (int k = 0; k < 3; k++) {
      ch = EEPROM.read(k + EE_TZ_ADDR);
      strT += String(ch);
    }
    strT+=strT2;
    
    *reply=strT;
    return true;
  } else {
    *reply=F(_M_SM033);
    return false;
  }
}
