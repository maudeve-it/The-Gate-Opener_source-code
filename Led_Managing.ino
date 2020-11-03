void GateComm() {
static unsigned long startTime = 0;
  /*  managing "Gate relay" controlling gate. That means:
   *  "close contact" if gateStatus is 'O'
   *  "open contact" if gateStatus is ' '
   *  command 'O' activate gate and sets a timeout to fall back to ' '
   */

  if ((gateStatus==' ') && ((millis() - startTime < GATE_TIMING) ))
    return;
  _PRINTLN_TEST(__FUNCTION__);
  if (gateStatus == ' ') {
    digitalWrite(GATE_PIN, !ACTIVE_GATE);
  } else if (gateStatus == 'O') {
    digitalWrite(GATE_PIN, ACTIVE_GATE);
    startTime = millis();
  };
  gateStatus = ' ';
}



void ClockLed(bool level){
  digitalWrite(SMS_R_LED_PIN, level);
  digitalWrite(SMS_G_LED_PIN, LOW);
  digitalWrite(SMS_B_LED_PIN, level);
  digitalWrite(GSM_R_LED_PIN, LOW);
  digitalWrite(GSM_G_LED_PIN, LOW);
}



void SmsLed() {
static unsigned long initTime=0, timer=0,timerToSetClk=0;// "timer" time to put led off (or changing it), "timerToSetClk" time retry a clock setup upon fault GPRS connection. Retry only (clockSetupTimes) times 
static byte clockSetupTimes=0; //number of retrying setting up clock. After MAX_CLK_STUP it will never retry anymore   
static char pre_status=' '; //smsStatus value in previous function call
  /*  managing "SMS/CALL Led" which is:
   *  total OFF if no activity on line
   *  fixed BLUE if receiving a phone call
   *  fixed GREEN if receiving an SMS
   *  fixed CYAN if sending an SMS
   *  led will turn off after SMS_LED_TIMING milliseconds
   *  when OFF, it will flash BLUE if cannot get GSM time updating SIM800 clock
   */

  if ((pre_status==smsStatus) && (millis()-initTime < timer))
    return;
  if ((smsStatus=='S') && (pre_status==smsStatus))
    smsStatus='Q';
  _PRINTLN_TEST(__FUNCTION__);
  #ifdef _TEST_LEDS
    if (pre_status!=smsStatus) {
      _PRINT_TEST_LEDS(__FUNCTION__);
      _PRINTLN_TEST_LEDS(smsStatus);
    }
  #endif

  switch (smsStatus) {
  case 'Q':                                                        //no activity on phone
    digitalWrite(SMS_G_LED_PIN, LOW);  
    digitalWrite(SMS_B_LED_PIN, LOW);  
    _PRINTLN_TEST_LEDS("clkStatus is:"+String(clkStatus));
    if (clkStatus=='N'){                                           //if no activity blink in case clock was not set 
      _PRINTLN_TEST_LEDS("SMS red pin is:"+String(digitalRead(SMS_R_LED_PIN)));
      digitalWrite(SMS_R_LED_PIN, !digitalRead(SMS_R_LED_PIN));
      _PRINTLN_TEST_LEDS("expiringusers is:"+String(expiringUsers));
      _PRINTLN_TEST_LEDS("SMS red pin is:"+String(digitalRead(SMS_R_LED_PIN)));
      timer = (expiringUsers ? (digitalRead(SMS_R_LED_PIN) ? CLK_EXP_ON : CLK_EXP_OFF) : (digitalRead(SMS_R_LED_PIN) ? CLK_NEXP_ON : CLK_NEXP_OFF) ) ;
//      timer = (digitalRead(SMS_R_LED_PIN) ? (expiringUsers ? CLK_EXP_ON : CLK_NEXP_ON) : (expiringUsers ? CLK_EXP_OFF : CLK_NEXP_OFF) ) ;
      _PRINTLN_TEST_LEDS("timer is:"+String(timer));
      _PRINTLN_TEST_LEDS("timertosetclock is:"+String(timerToSetClk));
      
      if (((millis()-timerToSetClk)>GSM_CLK_TIMING ) && (timerToSetClk!=0)){         // and go to check if reachable NTP
        if ((clockSetupTimes++)<MAX_CLK_STUP) {
          timerToSetClk=0;
          _PRINTLN_TEST_ON_USB(F(_M_LM001));
          _PRINTLN_TEST_CSQ("before clocksetup gsmStatus is: "+String(gsmStatus));
          digitalWrite(SMS_R_LED_PIN, LOW); //before entring in clocksetup 
          ClockSetup();
          _PRINTLN_TEST_CSQ("after clocksetup gsmStatus is: "+String(gsmStatus));
        } 
      }
      if (timerToSetClk==0)
        timerToSetClk=millis();
    } else {                                                      // clock is set, no more activity requested -> long delay
      timerToSetClk=0;
      if (digitalRead(SMS_R_LED_PIN))
        digitalWrite(SMS_R_LED_PIN, LOW);
      timer= LED_LONG_DELAY; 
    }
    digitalWrite(SMS_B_LED_PIN, digitalRead(SMS_R_LED_PIN));
    break;
  case 'I':                                                       // incoming call
    digitalWrite(SMS_G_LED_PIN, LOW);
    digitalWrite(SMS_B_LED_PIN, HIGH);
    digitalWrite(SMS_R_LED_PIN, LOW);
    timer=SMS_LED_TIMING;
    break;
  case 'R':                                                       //receiving sms
    digitalWrite(SMS_G_LED_PIN, HIGH);
    digitalWrite(SMS_B_LED_PIN, HIGH);
    digitalWrite(SMS_R_LED_PIN, HIGH);
    timer=SMS_LED_TIMING;
    break;
  case 'S':                                                       // sending sms
    digitalWrite(SMS_G_LED_PIN, LOW);
    digitalWrite(SMS_B_LED_PIN, HIGH);
    digitalWrite(SMS_R_LED_PIN, HIGH);
    timer=SMS_LED_LONG_TIMING;
    break;
  };  
  initTime=millis();
  if (smsStatus!='S')
    smsStatus = 'Q';
  pre_status = smsStatus;
}



void GsmLed() {
  /*  managing "GSM Status Led" which is:
   *  fixed GREEN if signal quality is gerade 'A'
   *  alternate GREEN/YELLOW if signal quality is grade 'B'
   *  fixed YELLOW if signal quality is gerade 'C'
   *  alternate YELLOW/RED if signal quality is grade 'D'
   *  fixed RED if signal quality is grade 'E'
   *  flashing RED if failure reading simcard ('F')
   *  
   *  Flag "checkRegistredUsers" make function to look for users in phonebook having expiry_date set.
   *  in this case properly set static variable "expiringUsers" that drives the information to the led
   *  
   */
static unsigned long timer, startTime, startTimeToSetup=0;  //"startTimeSetup" time retry a setup upon fault on device connection
static char pre_status=' ';

  if ((gsmStatus==pre_status) && ((millis() - startTime) < timer))
    return;
  _PRINTLN_TEST(__FUNCTION__);
  _PRINT_TEST_LEDS(__FUNCTION__);
  _PRINTLN_TEST_LEDS(gsmStatus);
  switch (gsmStatus) {
  case 'A':
    digitalWrite(GSM_R_LED_PIN, LOW);
    digitalWrite(GSM_G_LED_PIN, HIGH);
    timer = LED_LONG_DELAY;
    break;
  case 'B':
    digitalWrite(GSM_R_LED_PIN, !digitalRead(GSM_R_LED_PIN));
    digitalWrite(GSM_G_LED_PIN, HIGH);
    timer = (digitalRead(GSM_R_LED_PIN) ? LED_FLASH_ON : LED_FLASH_OFF );
    break;
  case 'C':
    digitalWrite(GSM_R_LED_PIN, HIGH);
    digitalWrite(GSM_G_LED_PIN, HIGH);
    timer = LED_LONG_DELAY;
    break;
  case 'D':
    digitalWrite(GSM_R_LED_PIN, HIGH);
    digitalWrite(GSM_G_LED_PIN, !digitalRead(GSM_G_LED_PIN));
    timer = (digitalRead(GSM_G_LED_PIN) ? LED_FLASH_OFF : LED_FLASH_ON );
    break;
  case 'E':
    digitalWrite(GSM_R_LED_PIN, HIGH);
    digitalWrite(GSM_G_LED_PIN, LOW);
    timer = LED_LONG_DELAY;
    break;
  case 'F':
    digitalWrite(GSM_R_LED_PIN, !digitalRead(GSM_R_LED_PIN));
    digitalWrite(GSM_G_LED_PIN, LOW);
    timer = LED_FLASH_ON;
    if ((millis() - startTimeToSetup > SIM800_DELAY_BEFORE_RESET) && (startTimeToSetup!=0)){
      _PRINTLN_TEST_ON_USB(_M_LM002);
      ResetPhone();
      dontGetTime=false;
    }
    if (startTimeToSetup==0)
      startTimeToSetup=millis();
    break;
  }
  startTime=millis();
  if ((gsmStatus!='F') and (startTimeToSetup!=0))
    startTimeToSetup=0;
  pre_status = gsmStatus;
}



void InitLed(char stato) {
byte k;  
bool level;
  _PRINTLN_TEST(__FUNCTION__);
  _PRINT_TEST_LEDS(__FUNCTION__);
  _PRINTLN_TEST_LEDS(stato);
  dontGetTime = true;
  switch (stato) {
    case '5':
      level=LOW;
      digitalWrite(RESET_PIN, !level);
      digitalWrite(GATE_LED_PIN, (level ? ACTIVE_GATE : !ACTIVE_GATE));
      digitalWrite(SMS_G_LED_PIN, level);
      digitalWrite(SMS_R_LED_PIN, level);
      digitalWrite(SMS_B_LED_PIN, level);
      digitalWrite(GSM_G_LED_PIN, level);
      digitalWrite(GSM_R_LED_PIN, level);
      pinMode(RESET_PIN,INPUT);  
      break;
    case '4':
      digitalWrite(GSM_G_LED_PIN, LOW);
      digitalWrite(GSM_R_LED_PIN, LOW);
      break;
    case '3':
      digitalWrite(SMS_G_LED_PIN, LOW);
      digitalWrite(SMS_R_LED_PIN, LOW);
      digitalWrite(SMS_B_LED_PIN, LOW);
      break;
    case '2':
      digitalWrite(GATE_LED_PIN, (LOW ? ACTIVE_GATE : !ACTIVE_GATE));
      break;
    case '1':
      digitalWrite(RESET_PIN, HIGH);
      break;
    case '0':
      pinMode(RESET_PIN,OUTPUT);
      level=LOW;
      for (k=0;k<2;k++){
        digitalWrite(RESET_PIN, !level);
        digitalWrite(GATE_LED_PIN, (level ? ACTIVE_GATE : !ACTIVE_GATE));
        digitalWrite(SMS_G_LED_PIN, level);
        digitalWrite(SMS_R_LED_PIN, level);
        digitalWrite(SMS_B_LED_PIN, level);
        digitalWrite(GSM_G_LED_PIN, level);
        digitalWrite(GSM_R_LED_PIN, level);
        delay(SETUP_LED_DELAY);
        level=!level;
      }
      break;
  }
  delay(LED_FLASH_OFF);
}


/*
void Blinking(int mode){
byte k;
bool level,levelB;
  level=LOW;
  levelB=LOW;
  pinMode(RESET_PIN,OUTPUT);
  for (k=0;k<3;k++){
    _PRINTLN_TEST_LEDS("LEDloop n.: "+String(k));
    digitalWrite(RESET_PIN, !level);
    digitalWrite(SMS_R_LED_PIN, levelB);
    digitalWrite(SMS_B_LED_PIN, levelB);
    digitalWrite(SMS_G_LED_PIN, levelB);
    delay(LED_FLASH_ON);
    level=!level;
    (mode==2 ? levelB=LOW : levelB=level);
  };
  _PRINTLN_TEST_LEDS("LEDloop: finished ");
  pinMode(RESET_PIN,INPUT);
}

*/
